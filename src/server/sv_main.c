#include "../shared/net_protocol.h"
#include "../shared/core_game.h"
#include "../shared/core_player.h"
#include "../shared/core_projectile.h"

#define PORT 5555
#define MAX_CLIENTS 2
#define TICK_RATE 60
#define TICK_TIME_MS (1000 / TICK_RATE)

typedef struct {
    Client clients[MAX_CLIENTS];
    TCPsocket socket;
    SDLNet_SocketSet socket_set;
    GameState game;  // Server maintains authoritative game state
    int game_running;
    uint32_t last_tick;
} Server;

void Server_InitNet(Server *server) {
    // Create socket
    if (SDLNet_Init() < 0) {
        Debug_Error("SDLNet_Init failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, NULL, PORT) < 0) {
        Debug_Error("SDLNet_ResolveHost failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    Debug_Info("Server listening on port %d", PORT);
    server->socket = SDLNet_TCP_Open(&ip);
    if (!server->socket) {
        Debug_Error("SDLNet_TCP_Open failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    server->socket_set = SDLNet_AllocSocketSet(MAX_CLIENTS);
    if (!server->socket_set) {
        Debug_Error("SDLNet_AllocSocketSet failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < MAX_CLIENTS; i++) {
        server->clients[i].socket = 0;
        server->clients[i].id = i;
        server->clients[i].active = 0;
    }
}

void Server_Init(Server *server) {
    // Initialize authoritative game state
    Game_Init(&server->game);

    server->game_running = 0;
    server->last_tick = SDL_GetTicks();

    Debug_Info("Server initialized");
}

void Server_Broadcast(Server *server, uint8_t packet_id, void *data, int len) {
    Client *clients = server->clients;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            NetProtocol_SendPacketToClient(&clients[i], packet_id, data, len);
        }
    }
}

//void Server_SendGameState(Server *server) {
//    // Send player positions
//    for (int i = 0; i < MAX_CLIENTS; i++) {
//        ServerMessage sv_msg;
//        sv_msg.type = SVMSG_PLAYER_POS;
//        sv_msg.data.player_pos.id = i;
//        sv_msg.data.player_pos.x = server->game.players[i].x;
//        sv_msg.data.player_pos.y = server->game.players[i].y;
//        Server_Broadcast(server, PACKET_SV_MESSAGE, &sv_msg, sizeof(ServerMessage));
//        // Send health separately
//        sv_msg.type = SVMSG_PLAYER_HEALTH;
//        sv_msg.data.player_health.id = i;
//        sv_msg.data.player_health.health = server->game.players[i].health;
//        Server_Broadcast(server, PACKET_SV_MESSAGE, &sv_msg, sizeof(ServerMessage));
//    }
//}

void Server_ProcessPlayerInput(Server *server, int player_id, ClientMessage *cl_msg) {
    if (!server->game_running) return;
    if (player_id < 0 || player_id >= MAX_CLIENTS) return;

    PlayerState *player = &server->game.players[player_id];
    uint32_t timestamp;
    PlayerActions actions;

    switch (cl_msg->type) {
        case CLMSG_PLAYER_MOVE:
            // Apply movement with server authority
            timestamp = cl_msg->timestamp;
            actions.move_left = cl_msg->data.player_move.left;
            actions.move_right = cl_msg->data.player_move.right;
            Player_MovementHandler(player, &server->game, actions, 1.0f/TICK_RATE);
            // World bounds check
            if (player->x < 0) player->x = 0;
            if (player->x > WORLD_WIDTH - player->w) player->x = WORLD_WIDTH - player->w;
            if (player->y > WORLD_HEIGHT) {
                player->y = WORLD_HEIGHT;
                player->alive = 0;
            }
            break;

        case CLMSG_PLAYER_THROW:
            // Validate cooldown
            if (player->throw_cooldown > 0) {
                return;  // Still on cooldown
            }

            // Get projectile start position (from player center)
            float center_x = player->x + player->w/2;
            float center_y = player->y + player->h/2;
            float radius = 40;  // Distance from center to arm
            float start_x = center_x + radius * cos(cl_msg->data.projectile_throw.angle);
            float start_y = center_y + radius * (-sin(cl_msg->data.projectile_throw.angle));

            // Create projectile on server
            int proj_id = Projectile_Throw(&server->game.projectile_sys, 
                                          cl_msg->data.projectile_throw.type,
                                          start_x, start_y,
                                          cl_msg->data.projectile_throw.angle,
                                          cl_msg->data.projectile_throw.power,
                                          player_id);

            if (proj_id >= 0) {
                player->throw_cooldown = 30;  // 0.5 second cooldown at 60fps

                // Broadcast projectile to all clients
                ServerMessage sv_msg;
                sv_msg.type = SVMSG_PROJECTILE_NEW;
                sv_msg.data.projectile_new.id = player_id;
                sv_msg.data.projectile_new.x = start_x;
                sv_msg.data.projectile_new.y = start_y;
                sv_msg.data.projectile_new.angle = cl_msg->data.projectile_throw.angle;
                sv_msg.data.projectile_new.power = cl_msg->data.projectile_throw.power;
                sv_msg.data.projectile_new.type = cl_msg->data.projectile_throw.type;
                Server_Broadcast(server, PACKET_SV_MESSAGE, &sv_msg, sizeof(ServerMessage));

                Debug_Info("Player %d threw projectile type %d with power %.0f", 
                          player_id, cl_msg->data.projectile_throw.type, 
                          cl_msg->data.projectile_throw.power);
            }
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

void Server_AcceptClients(Server *server) {
    while (1) {
        // Check for new connection
        TCPsocket client_socket = SDLNet_TCP_Accept(server->socket);
        Client *clients = server->clients;
        if (client_socket) {
            // Find free slot
            int slot = -1;
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (!clients[i].active) {
                    slot = i;
                    break;
                }
            }
            if (slot >= 0) {
                clients[slot].socket = client_socket;
                clients[slot].id = slot;
                clients[slot].active = 1;
                SDLNet_TCP_AddSocket(server->socket_set, client_socket);
                Debug_Info("Client %d connected", slot);
                // Send welcome with ID
                NetProtocol_SendPacketToClient(&clients[slot], PACKET_SV_WELCOME, &slot, 4);
            }
            else {
                // Server full
                NetProtocol_SendPacketToClient(&clients[slot], PACKET_SV_FULL, NULL, 0);
                SDLNet_TCP_Close(client_socket);
            }
        }
        int num_active_sockets = SDLNet_CheckSockets(server->socket_set, 50);
        char buffer[1024];
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
                        // Got data, but the other client is not ready yet.
                        NetProtocol_SendPacketToClient(&clients[i], PACKET_SV_WAITING, NULL, 0);
                    }
                }
            }
        }
        if (clients[0].active && clients[1].active) {
            Server_Broadcast(server, PACKET_SV_START, NULL, 0);
            Debug_Info("Both clients connected! Starting game!");
            break;
        }
        SDL_Delay(10);
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
                            uint8_t packet_type = cl_msg.type;
                            switch (packet_type) {
                                case CLMSG_PLAYER_MOVE:
                                    break;
                                case CLMSG_PLAYER_THROW:
                                    break;
                                default:
                                    break;
                            }
                        }
                        ServerMessage sv_msg;
                        NetProtocol_SendPacketToClient(&clients[i], PACKET_SV_MESSAGE, &sv_msg, sizeof(ServerMessage));
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

void Server_Clean(Server *server) {
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
}

int main(int argc, char *argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    Server server;
    Server_Init(&server);
    Server_AcceptClients(&server);
    Server_Loop(&server);
    Server_Clean(&server);
    return 0;
}
