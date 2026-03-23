#include "sv_main.h"
#include "sv_cmd.h"
#include "sv_net.h"

#include "../shared/core_game.h"
#include "../shared/core_player.h"
#include "../shared/core_projectile.h"
#include <stdlib.h>

void Server_Init(Server *server) {
    Server_InitNet(server);
    Game_Init(&server->game);
    server->game_running = 0;
    server->last_tick = SDL_GetTicks();
    Debug_Info("Server initialized");
}

void sv_cmd_SendPositions(Server *server) {
    // Send player positions
    for (int i = 0; i < MAX_CLIENTS; i++) {
        ServerMessage sv_msg;
        sv_msg.type = SVMSG_PLAYER_POS;
        sv_msg.data.player_pos.id = i;
        sv_msg.data.player_pos.x = server->game.players[i].x;
        sv_msg.data.player_pos.y = server->game.players[i].y;
        Server_Broadcast(server, PACKET_SV_MESSAGE, &sv_msg, sizeof(ServerMessage));
        // Send health separately
        sv_msg.type = SVMSG_PLAYER_HEALTH;
        sv_msg.data.player_health.id = i;
        sv_msg.data.player_health.health = server->game.players[i].health;
        Server_Broadcast(server, PACKET_SV_MESSAGE, &sv_msg, sizeof(ServerMessage));
    }
}

void Server_HandleClientMessage(Server *server, int player_id, ClientMessage *cl_msg) {
    if (!server->game_running) return;
    if (player_id < 0 || player_id >= MAX_CLIENTS) return;
    PlayerState *player = &server->game.players[player_id];
    uint32_t timestamp;
    PlayerActions actions;
    int proj_id;
    switch (cl_msg->type) {
        case CLMSG_PLAYER_MOVE:
            // Apply movement with server authority
            timestamp = cl_msg->timestamp;
            actions.move_left = cl_msg->data.player_move.left;
            actions.move_right = cl_msg->data.player_move.right;
            Player_MovementHandler(player, &server->game, actions, SDL_GetTicks()-timestamp);
            break;
        case CLMSG_PLAYER_THROW:
            // Create projectile on server
            sv_cmd_PlayerShoot(server, player, &server->game.projectile_sys,
                               cl_msg->data.projectile_throw.type,
                               cl_msg->data.projectile_throw.angle,
                               cl_msg->data.projectile_throw.power);
            break;
    }
}

void Server_Update(Server *server) {
    if (!server->game_running) return;
    // Update cooldowns
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (server->game.players[i].throw_cooldown > 0) {
            server->game.players[i].throw_cooldown--;
        }
    }
    // Update projectiles (server authoritative)
    Projectile_Update(&server->game.projectile_sys, &server->game);
    // Check for terrain destruction from explosions
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        Projectile *p = &server->game.projectile_sys.projectiles[i];
        if (p->state == PROJECTILE_EXPLODING && p->explosion_timer == 10) {  // Just exploded
            float cx = p->x + p->w/2.0;
            float cy = p->y + p->h/2.0;

            // Broadcast terrain destruction to clients
            ServerMessage sv_msg;
            sv_msg.type = SVMSG_TERRAIN_DESTROY;
            sv_msg.data.terrain_destroy.x = cx;
            sv_msg.data.terrain_destroy.y = cy;
            sv_msg.data.terrain_destroy.radius = BOMB_RADIUS;
            Server_Broadcast(server, PACKET_SV_MESSAGE, &sv_msg, sizeof(ServerMessage));
        }
    }
    // Check win condition
    int alive_count = 0;
    int last_alive = -1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (server->game.players[i].alive && server->game.players[i].health > 0) {
            alive_count++;
            last_alive = i;
        }
    }
    if (alive_count <= 1 && server->game_running) {
        // Game over, broadcast winner
        ServerMessage sv_msg;
        sv_msg.type = SVMSG_GAME_OVER;
        sv_msg.data.game_over.winner = last_alive;
        Server_Broadcast(server, PACKET_SV_MESSAGE, &sv_msg, sizeof(ServerMessage));
        server->game_running = 0;
        Debug_Info("Game over! Winner: Player %d", last_alive);
        Server_Broadcast(server, PACKET_SV_DISCONNECT, NULL, 0);
    }
}


void Server_Loop(Server *server) {
    char buffer[1024];
    Client *clients = server->clients;
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
                            ClientMessage cl_msg = *(ClientMessage *)(buffer + 1);
                            Server_HandleClientMessage(server, i, &cl_msg);
                        }
                        Server_Update(server);  // ACTUAL UPDATE OF THE SERVER
                    }
                }
            }
            if (!clients[0].active || !clients[1].active) {
                Server_Broadcast(server, PACKET_SV_DISCONNECT, NULL, 0);
                Debug_Info("Some player disconnected. Ending game!");
                break;
            }
        }
    }
}

void Server_Clean(Server *server, int exit_code) {
    Client *clients = server->clients;
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
    Server server;
    Server_Init(&server);
    Server_AcceptClients(&server);
    Server_Loop(&server);
    Server_Clean(&server, EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
