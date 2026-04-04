#include "sv_main.h"
#include "sv_cmd.h"
#include "sv_net.h"
#include "sv_logic.h"

#include "../shared/core_game.h"
#include "../shared/core_player.h"

void sv__Init(sv_server_t *server) {
    sv_net_Init(server);
    Game_Init(&server->game);
    server->game_running = 1;
    server->last_tick = SDL_GetTicks();
    Debug_Info("Server initialized");
}

void sv__HandleClientMessage(sv_server_t *server, int player_id, cl_msg_t *cl_msg) {
    Debug_HexDump(cl_msg, sizeof(cl_msg_t), "Received data from client %d", player_id);
    if (!server->game_running) return;
    if (player_id < 0 || player_id >= MAX_CLIENTS) return;
    PlayerState *player = &server->game.players[player_id];
    PlayerActions actions;
    switch (cl_msg->type) {
        case CLMSG_MOVE:
            // Apply movement with server authority
            actions.move_left = cl_msg->data.move.left;
            actions.move_right = cl_msg->data.move.right;
            Player_MovementHandler(player, &server->game, actions, 1/60.0f);
            break;
        case CLMSG_PROJECTILE:
            // Create projectile on server
            sv_cmd_PlayerShoot(server, player, &server->game.projectile_sys,
                               cl_msg->data.projectile.type,
                               cl_msg->data.projectile.angle,
                               cl_msg->data.projectile.power);
            break;
    }
}

void sv__Update(sv_server_t *server) {
    if (!server->game_running) return;
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
        sv_msg_t sv_msg;
        sv_msg.type = SVMSG_GAME_OVER;
        sv_msg.data.game_over.winner = last_alive;
        sv_net_Broadcast(server, PACKET_SV_MESSAGE, &sv_msg, sizeof(sv_msg_t));
        server->game_running = 0;
        Debug_Info("Game over! Winner: Player %d", last_alive);
        sv_net_Broadcast(server, PACKET_SV_DISCONNECT, NULL, 0);
    }
}


void sv__Loop(sv_server_t *server) {
    char buffer[1024];
    sv_client_t *clients = server->clients;
    while (1) {
        // Check for activity
        int num_active_sockets = SDLNet_CheckSockets(server->socket_set, 50);
        if (num_active_sockets > 0) {
            // Check each client
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].active && SDLNet_SocketReady(clients[i].socket)) {
                    int bytes = SDLNet_TCP_Recv(clients[i].socket, buffer, sizeof(buffer));
                    if (bytes <= 0) {
                        // Client disconnected
                        Debug_Info("Client %d disconnected", i);
                        SDLNet_TCP_DelSocket(server->socket_set, clients[i].socket);
                        SDLNet_TCP_Close(clients[i].socket);
                        clients[i].active = 0;
                    }
                    else {
                        // HERE WE PROCESS CLIENT INPUT
                        uint8_t packet_id = *(uint8_t *)buffer;
                        if (packet_id == PACKET_CL_MESSAGE) {
                            cl_msg_t cl_msg = *(cl_msg_t *)(buffer + 1);
                            sv__HandleClientMessage(server, i, &cl_msg);
                        }
                    }
                }
            }
            Debug_Info("UPDATING");
            sv__Update(server);  // ACTUAL UPDATE OF THE SERVER
            if (!clients[0].active || !clients[1].active) {
                sv_net_Broadcast(server, PACKET_SV_DISCONNECT, NULL, 0);
                Debug_Info("Some player disconnected. Ending game!");
                break;
            }
        }
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
