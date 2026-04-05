#include "sv_main.h"
#include "sv_cmd.h"
#include "sv_net.h"
#include "sv_logic.h"

#include "../shared/core_game.h"
#include "../shared/core_player.h"
#include "sv_types.h"

void sv__Init(sv_server_t *server) {
    sv_net_Init(server);
    Game_Init(&server->game);
    server->game_running = 1;
    Debug_Info("Server initialized");
}

void sv__ProcessMessages(sv_server_t *server) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        sv_msg_queue_t *queue = &server->clients[i].queue;
        while (!sv_msg_is_empty(queue)) {
            cl_msg_t msg;
            sv_msg_dequeue(queue, &msg);
            uint8_t type = msg.type;
            PlayerState *player = &server->game.players[i];
            PlayerActions actions;
            switch (type) {
                case CLMSG_MOVE:
                    // Apply movement with server authority
                    actions.move_left = msg.data.move.left;
                    actions.move_right = msg.data.move.right;
                    Player_MovementHandler(player, &server->game, actions, 1/60.0f);
                    break;
                case CLMSG_PROJECTILE:
                    // Create projectile on server
                    sv_cmd_PlayerShoot(server, player, &server->game.projectile_sys,
                                       msg.data.projectile.type,
                                       msg.data.projectile.angle,
                                       msg.data.projectile.power);
                    break;
            }
        }
    }
}

int sv__Update(sv_server_t *server) {
    if (!server->game_running) return 0;
    sv__ProcessMessages(server);
    // Update cooldowns
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (server->game.players[i].throw_cooldown > 0) {
            server->game.players[i].throw_cooldown--;
        }
    }
    // Broadcast server positions
    sv_cmd_PlayerBroadcastPositions(server);
    // Update projectiles (server authoritative)
    sv_logic_ProjectileUpdate(server);

    // Check win condition
    int alive_count = 0;
    int last_alive = -1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (server->game.players[i].alive && server->game.players[i].health > 0) {
            alive_count++;
            last_alive = i;
        }
    }
    if (alive_count <= 1) {
        // Game over, broadcast winner
        sv_msg_t sv_msg = {0};
        sv_msg.type = SVMSG_GAME_OVER;
        sv_msg.data.game_over.winner = last_alive;
        sv_net_Broadcast(server, PACKET_SV_MESSAGE, &sv_msg, sizeof(sv_msg_t));
        server->game_running = 0;
        Debug_Info("Game over! Winner: Player %d", last_alive);
        sv_net_Broadcast(server, PACKET_SV_DISCONNECT, NULL, 0);
        return 0;
    }
    return 1;
}

void sv__EnqueueMessages(sv_server_t *server, int client_id, void *data, int len_data) {
    if (client_id < 0 || client_id >= MAX_CLIENTS) return;
    int offset = 0;
    int packet_size = 1 + sizeof(cl_msg_t);
    while (offset < len_data) {
        if (len_data - offset < packet_size) break;
        cl_msg_t msg = *(cl_msg_t *) (data + offset + 1);
        sv_msg_enqueue(&server->clients[client_id].queue, msg);
        offset += packet_size;
    }
}

int sv__HandleNetwork(sv_server_t *sv) {
    static int last_time = 0;
    int current_time = SDL_GetTicks();
    char buffer[4096];
    sv_client_t *clients = sv->clients;
    // Check for activity
    int num_active_sockets = SDLNet_CheckSockets(sv->socket_set, 0);
    if (num_active_sockets > 0) {
        // Check each client
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active && SDLNet_SocketReady(clients[i].socket)) {
                int bytes = SDLNet_TCP_Recv(clients[i].socket, buffer, sizeof(buffer));
                if (bytes <= 0) {
                    // Client disconnected
                    Debug_Info("Client %d disconnected", i);
                    SDLNet_TCP_DelSocket(sv->socket_set, clients[i].socket);
                    SDLNet_TCP_Close(clients[i].socket);
                    clients[i].active = 0;
                }
                else {
                    // HERE WE PROCESS CLIENT INPUT
                    sv__EnqueueMessages(sv, i, buffer, bytes);
                    if ((current_time - last_time) > 1000) {
                        //Debug_HexDump(buffer, bytes, "Received %d bytes from client %d", bytes, i);
                    }
                }
            }
        }
        if (!clients[0].active || !clients[1].active) {
            sv_net_Broadcast(sv, PACKET_SV_DISCONNECT, NULL, 0);
            printf("Some player disconnected. Ending game!");
            return 0;
        }
    }
    if ((current_time - last_time) > 1000) {
        last_time = current_time;
    }
    return 1;
}

void sv__Loop(sv_server_t *server) {
    while (1) {
        if (!sv__HandleNetwork(server) || !sv__Update(server)) {
            break;
        }
        SDL_Delay(1000/60.0);
    }
}

void sv__Clean(sv_server_t *server, int exit_code) {
    sv_client_t *clients = server->clients;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            SDLNet_TCP_Close(clients[i].socket);
        }
    }
    SDLNet_TCP_Close(server->socket);
    SDLNet_FreeSocketSet(server->socket_set);
    Game_Clean(&server->game);
    SDLNet_Quit();
    exit(exit_code);
}

int main(int argc, char *argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    sv_server_t server;
    sv__Init(&server);
    sv_net_AcceptClients(&server);
    sv__Loop(&server);
    sv__Clean(&server, EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
