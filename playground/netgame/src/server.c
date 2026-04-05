#include "shared.c"

#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <stdint.h>

#define MAX_PEDING_MESSAGES 10
typedef struct {
    int id;
    int active;
    TCPsocket sock;
    struct { uint8_t pending; uint8_t packet_id; cl_msg_t msg; } pending_msg[MAX_PEDING_MESSAGES];
} sv_client_t;

typedef struct {
    TCPsocket sock;
    SDLNet_SocketSet sockset;
    sv_client_t clients[MAX_CLIENTS];
    sh_game_t *game;
} sv_server_t;

void sv_clean(sv_server_t *sv) {
    UNUSED(sv);
    return;
}

void sv_init(sv_server_t *sv, sh_game_t *game) {
    if (SDLNet_Init() < 0) {
        printf("SDLNet_Init failed: %s\n", SDLNet_GetError());
        exit(1);
    }

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, NULL, PORT) < 0) {
        printf("SDLNet_ResolveHost failed: %s\n", SDLNet_GetError());
        exit(1);
    }
    printf("Server listening on port %d\n", (int) ntohs(ip.port));

    sv->sock = SDLNet_TCP_Open(&ip);
    if (!sv->sock) {
        printf("SDLNet_TCP_Open failed: %s\n", SDLNet_GetError());
        exit(1);
    }
    
    sv->sockset = SDLNet_AllocSocketSet(2);
    if (!sv->sockset) {
        printf("SDLNet_AllocSocketSet failed: %s\n", SDLNet_GetError());
        exit(1);
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        sv->clients[i].id = i;
        sv->clients[i].active = 0;
        sv->clients[i].sock = 0;
    }

    sv->game = game;
    sh_game_init(sv->game);
}

void sv_broadcast(sv_server_t *sv, uint8_t packet_id, void *data, int len_data) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        sh_send_packet(sv->clients[i].sock, packet_id, data, len_data);
    }
}

void sv_accept(sv_server_t *sv) {
    while (1) {
        // Check for new connection
        TCPsocket new_sock = SDLNet_TCP_Accept(sv->sock);
        sv_client_t *clients = sv->clients;
        if (new_sock) {
            // Find free slot
            int slot = -1;
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (!clients[i].active) {
                    slot = i;
                    break;
                }
            }
            if (slot >= 0) {
                clients[slot].sock = new_sock;
                clients[slot].id = slot;
                clients[slot].active = 1;
                SDLNet_TCP_AddSocket(sv->sockset, new_sock);
                printf("Client %d connected\n", slot);
                // Send welcome with ID
                sh_send_packet(clients[slot].sock, PACKET_SV_WELCOME, &slot, sizeof(slot));
            }
            else {
                // Server full
                sh_send_packet(clients[slot].sock, PACKET_SV_FULL, NULL, 0);
                SDLNet_TCP_Close(new_sock);
            }
        }
        int num_active_sockets = SDLNet_CheckSockets(sv->sockset, 60);
        char buffer[1024];
        if (num_active_sockets > 0) {
            // Check each client
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].active && SDLNet_SocketReady(clients[i].sock)) {
                    int bytes = SDLNet_TCP_Recv(clients[i].sock, buffer, sizeof(buffer));
                    if (bytes <= 0) {
                        // Client disconnected
                        printf("Client %d disconnected\n", i);
                        SDLNet_TCP_DelSocket(sv->sockset, clients[i].sock);
                        SDLNet_TCP_Close(clients[i].sock);
                        clients[i].active = 0;
                    }
                    else {
                        // Got data, but the other client is not ready yet.
                        sh_send_packet(clients[i].sock, PACKET_SV_WAITING, NULL, 0);
                    }
                }
            }
        }
        if (clients[0].active && clients[1].active) {
            sv_broadcast(sv, PACKET_SV_START, NULL, 0);
            printf("Both clients connected! Starting game!\n");
            break;
        }
        SDL_Delay(60);
    }
}

void sv_process_pending_msgs(sv_client_t *client, sh_player_t *player) {
    float delta_time = 1/60.f;
    for (int i = 0; i < MAX_PEDING_MESSAGES; i++) {
        if (!client->pending_msg[i].pending) {
            continue;
        }
        uint8_t packet_id = client->pending_msg[i].packet_id;
        cl_msg_t *msg = &client->pending_msg[i].msg;
        switch (packet_id) {
            case PACKET_CL_MSG_PLAYER_MOVE:
            {
                if (msg->data.move.left && !msg->data.move.right) {
                    player->x -= SPEED * delta_time;
                }
                else if (!msg->data.move.left && msg->data.move.right) {
                    player->x += SPEED * delta_time;
                }
                // bound collision
                if (player->x < 0) {
                    player->x = 0;
                }
                if (player->x > WORLD_WIDTH - player->w) {
                    player->x = WORLD_WIDTH - player->w;
                }
                if (player->y < 0) {
                    player->y = 0;
                }
                if (player->y > WORLD_HEIGHT - player->h) {
                    player->y = WORLD_HEIGHT - player->h;
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

void sv_handle_msg(sv_server_t *server, int client_id, void *data, int len_data) {
    if (client_id < 0 || client_id >= MAX_CLIENTS) return;
    int offset = 0;
    int msg_index = 0;
    while (offset < len_data) {
        uint8_t packet_id = *(uint8_t *) (data + offset);
        offset += 1;
        server->clients[client_id].pending_msg[msg_index].pending = 1;
        server->clients[client_id].pending_msg[msg_index].packet_id = packet_id;
        server->clients[client_id].pending_msg[msg_index].msg = *(cl_msg_t *) (data + offset);
        offset += sizeof(cl_msg_t);
        msg_index++;
    }
    while (msg_index < MAX_PEDING_MESSAGES) {
        server->clients[client_id].pending_msg[msg_index].pending = 0;
        msg_index++;
    }
}

void sv_broadcast_players_pos(sv_server_t *sv) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        sv_msg_t msg;
        msg.data.pos.id = i;
        msg.data.pos.x = sv->game->players[i].x;
        msg.data.pos.y = sv->game->players[i].y;
        sv_broadcast(sv, PACKET_SV_MSG_PLAYER_POS, &msg, sizeof(msg));
    }
}

int sv_recv_client_data(sv_server_t *sv) {
    static int last_time = 0;
    int current_time = SDL_GetTicks();
    char buffer[1024];
    sv_client_t *clients = sv->clients;
    // Check for activity
    int num_active_sockets = SDLNet_CheckSockets(sv->sockset, 0);
    if (num_active_sockets > 0) {
        // Check each client
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active && SDLNet_SocketReady(clients[i].sock)) {
                int bytes = SDLNet_TCP_Recv(clients[i].sock, buffer, sizeof(buffer));
                if (bytes <= 0) {
                    // Client disconnected
                    printf("Client %d disconnected\n", i);
                    SDLNet_TCP_DelSocket(sv->sockset, clients[i].sock);
                    SDLNet_TCP_Close(clients[i].sock);
                    clients[i].active = 0;
                }
                else {
                    // HERE WE PROCESS CLIENT INPUT
                    sv_handle_msg(sv, i, buffer, bytes);
                    if ((current_time - last_time) > 1000) {
                        printf("num_bytes_received = %d, client = %d\n", bytes, i);
                    }
                }
            }
        }
        if (!clients[0].active || !clients[1].active) {
            sv_broadcast(sv, PACKET_SV_DISCONNECT, NULL, 0);
            printf("Some player disconnected. Ending game!\n");
            return 0;
        }
    }
    if ((current_time - last_time) > 1000) {
        last_time = current_time;
    }
    return 1;
}

void sv_loop(sv_server_t *sv) {
    while (1) {
        if (!sv_recv_client_data(sv)) {
            break;
        }
        // Update all players based on their input state
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sv_process_pending_msgs(&sv->clients[i], &sv->game->players[i]);
        }
        sv_broadcast_players_pos(sv);  // ACTUAL UPDATE OF THE SERVER
        SDL_Delay(1000/20.0);
    }
}


int main(int argc, char *argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    sv_server_t sv;
    sh_game_t game;
    sv_init(&sv, &game);
    sv_accept(&sv);
    sv_loop(&sv);
    sv_clean(&sv);
    return 0;
}
