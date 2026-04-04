#include "cl_net.h"

#include "../shared/net_protocol.h"
#include "cl_player.h"
#include "cl_projectile.h"
#include "cl_terrain.h"

static cl_net_Handler handlers[256];

int packet_sizes[256] = {
    [PACKET_SV_WELCOME] = 1 + sizeof(int), // 4 bytes (client id)
    [PACKET_SV_FULL] = 1,
    [PACKET_SV_WAITING] = 1,
    [PACKET_SV_START] = 1,
    [PACKET_SV_DISCONNECT] = 1,
    [PACKET_SV_MESSAGE] = 1 + sizeof(sv_msg_t),
    [PACKET_CL_MESSAGE] = 1 + sizeof(cl_msg_t),
};

static int cl_net_H_PACKET_SV_WELCOME(cl_state_t *client, void *data, int len_data) {
    if (len_data != packet_sizes[PACKET_SV_WELCOME]) {
        return 0;
    }
    UNUSED(len_data);
    int offset = sizeof(uint8_t);
    client->my_player_id = *(int *)(data + offset);
    return 0;
}

static int cl_net_H_PACKET_SV_FULL(cl_state_t *client, void *data, int len_data) {
    UNUSED(client);
    UNUSED(data);
    UNUSED(len_data);
    return 1;
}

static int cl_net_H_NOOP(cl_state_t *client, void *data, int len_data) {
    UNUSED(client);
    UNUSED(data);
    UNUSED(len_data);
    return 0;
}

static int cl_net_H_PACKET_SV_START(cl_state_t *client, void *data, int len_data) {
    UNUSED(client);
    UNUSED(data);
    UNUSED(len_data);
    client->start = 1;
    return 0;
}

static int cl_net_H_PACKET_SV_DISCONNECT(cl_state_t *client, void *data, int len_data) {
    UNUSED(client);
    UNUSED(data);
    UNUSED(len_data);
    return 1;   // return 1 in order to quit
}

static int cl_net_H_PACKET_SV_MESSAGE(cl_state_t *client, void *data, int len_data) {
    if (len_data != packet_sizes[PACKET_SV_MESSAGE]) {
        return 0;
    }
    GameState *game = client->game;
    sv_msg_t packet = *(sv_msg_t *)(data + 1);
    uint8_t type = packet.type;
    switch (type) {
        case SVMSG_PLAYER_POS:
            game->players[packet.data.player_pos.id].x = packet.data.player_pos.x;
            game->players[packet.data.player_pos.id].y = packet.data.player_pos.y;
            break;
        case SVMSG_PLAYER_HEALTH:
            cl_player_TakeDamage(&client->cl_players[packet.data.player_health.id],
                                    packet.data.player_health.health);
            break;
        case SVMSG_PROJECTILE_NEW:
            cl_projectile_Throw(&client->cl_projectile_sys, packet.data.projectile_new.type,
                             packet.data.projectile_new.x, packet.data.projectile_new.y,
                             packet.data.projectile_new.angle, packet.data.projectile_new.power,
                             packet.data.projectile_new.owner_id);
            break;
        case SVMSG_TERRAIN_DESTROY:
            cl_terrain_DestroyCircle(&client->cl_terrain,
                                  packet.data.terrain_destroy.x, packet.data.terrain_destroy.y,
                                  packet.data.terrain_destroy.radius);
            client->cl_terrain.dirty = 1;
            break;
        case SVMSG_GAME_OVER:
            Debug_Info("Game Over! Player %d wins!", packet.data.game_over.winner);
            return 1;
            break;
        default:
            break;
    }
    return 0;
}

void cl_net_InitHandlers() {
    for (int i = 0; i < 256; i++) {
        handlers[i] = cl_net_H_NOOP;
    }
    handlers[PACKET_SV_WELCOME] = cl_net_H_PACKET_SV_WELCOME;
    handlers[PACKET_SV_FULL] = cl_net_H_PACKET_SV_FULL;
    handlers[PACKET_SV_WAITING] = cl_net_H_NOOP;
    handlers[PACKET_SV_START] = cl_net_H_PACKET_SV_START;
    handlers[PACKET_SV_DISCONNECT] = cl_net_H_PACKET_SV_DISCONNECT;
    handlers[PACKET_SV_MESSAGE] = cl_net_H_PACKET_SV_MESSAGE;
}

int cl_net_RecvFromServer(cl_state_t *client, int timeout) {
    static int last_time;
    int active_socket = SDLNet_CheckSockets(client->server_socket_set, timeout);
    if (active_socket) {
        char buffer[4096];
        int num_bytes_received = SDLNet_TCP_Recv(client->server_socket, buffer, sizeof(buffer));
        if ((SDL_GetTicks() - last_time) > 1000) {
            printf("num_bytes_received = %d\n", num_bytes_received);
            last_time = SDL_GetTicks();
        }
        int offset = 0;
        while (offset < num_bytes_received) {
            uint8_t packet_id = *(uint8_t *)(buffer + offset);
            if (num_bytes_received - offset < packet_sizes[packet_id]) break;
            int done = handlers[packet_id](client, buffer + offset, packet_sizes[packet_id]);
            if (done) return 1;
            offset += packet_sizes[packet_id];
        }
    }
    return 0;
}

void cl_net_InitSockets(cl_state_t *client, char *ip_addr) {
    // Networking init
    if (SDLNet_Init() < 0) {
        Debug_Error("SDLNet_Init failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, ip_addr, SERVER_PORT)) {
        Debug_Error("SDLNet_ResolveHost failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    client->server_socket = SDLNet_TCP_Open(&ip);
    if (!client->server_socket) {
        Debug_Error("SDLNet_TCP_Open failed: %s", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    client->server_socket_set = SDLNet_AllocSocketSet(1);
    if (!client->server_socket_set) {
        if (client->server_socket) SDLNet_TCP_Close(client->server_socket);
    }
    SDLNet_TCP_AddSocket(client->server_socket_set, client->server_socket);
    Debug_Info("Connected to server with IP %s port %d", ip_addr, ip.port);
}

void cl_net_SendMovement(TCPsocket server, PlayerActions actions) {
    cl_msg_t cl_msg;
    cl_msg.type = CLMSG_MOVE;
    cl_msg.data.move.left = actions.move_left;
    cl_msg.data.move.right = actions.move_right;
    net_SendPacketToServer(server, PACKET_CL_MESSAGE, &cl_msg, sizeof(cl_msg_t));
}

void cl_net_SendThrow(TCPsocket server, float angle, float power, int type) {
    cl_msg_t cl_msg;
    cl_msg.type = CLMSG_PROJECTILE;
    cl_msg.data.projectile.type = type;
    cl_msg.data.projectile.angle = angle;
    cl_msg.data.projectile.power = power;
    net_SendPacketToServer(server, PACKET_CL_MESSAGE, &cl_msg, sizeof(cl_msg_t));
}
