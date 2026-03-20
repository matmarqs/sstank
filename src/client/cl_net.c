#include "cl_net.h"

#include "../shared/net_protocol.h"
#include "../shared/struct_game.h"
#include "../shared/base_common.h"
#include "../shared/logic_projectile.h"
#include "../shared/logic_terrain.h"
#include <SDL2/SDL_net.h>

static ClientNet_Handler handlers[256];

static int ClientNet_H_PACKET_SV_WELCOME(Game *game, void *data, int len_data) {
    UNUSED(len_data);
    int offset = sizeof(uint8_t);
    game->my_player_id = *(int *)(data + offset);
    return 0;
}

static int ClientNet_H_PACKET_SV_FULL(Game *game, void *data, int len_data) {
    UNUSED(game);
    UNUSED(data);
    UNUSED(len_data);
    return 1;
}

static int ClientNet_H_NOOP(Game *game, void *data, int len_data) {
    UNUSED(game);
    UNUSED(data);
    UNUSED(len_data);
    return 0;
}

static int ClientNet_H_PACKET_SV_START(Game *game, void *data, int len_data) {
    UNUSED(game);
    UNUSED(data);
    UNUSED(len_data);
    game->start = 1;
    return 0;
}

static int ClientNet_H_PACKET_SV_DISCONNECT(Game *game, void *data, int len_data) {
    UNUSED(game);
    UNUSED(data);
    UNUSED(len_data);
    return 1;   // return 1 in order to quit
}

static int ClientNet_H_PACKET_SV_MESSAGE(Game *game, void *data, int len_data) {
    UNUSED(len_data);
    int offset = sizeof(uint8_t);
    ServerMessage packet = *(ServerMessage *)(data + offset);
    uint8_t type = packet.type;
    switch (type) {
        case SVMSG_PLAYER_POS:
            game->players[packet.data.player_pos.id].x = packet.data.player_pos.x;
            game->players[packet.data.player_pos.id].y = packet.data.player_pos.y;
            break;
        case SVMSG_PLAYER_HEALTH:
            game->players[packet.data.player_pos.id].health = packet.data.player_health.health;
            break;
        case SVMSG_PROJECTILE_NEW:
            Projectile_Throw(&game->projectile_sys, packet.data.projectile_new.type,
                             packet.data.projectile_new.x, packet.data.projectile_new.y,
                             packet.data.projectile_new.angle, packet.data.projectile_new.power,
                             packet.data.projectile_new.id);
            break;
        case SVMSG_TERRAIN_DESTROY:
            Terrain_DestroyCircle(&game->terrain,
                                  packet.data.terrain_destroy.x, packet.data.terrain_destroy.y,
                                  packet.data.terrain_destroy.radius);
            break;
        default:
            break;
    }
    return 0;
}

void ClientNet_InitHandlers() {
    handlers[PACKET_SV_WELCOME] = ClientNet_H_PACKET_SV_WELCOME;
    handlers[PACKET_SV_FULL] = ClientNet_H_PACKET_SV_FULL;
    handlers[PACKET_SV_WAITING] = ClientNet_H_NOOP;
    handlers[PACKET_SV_START] = ClientNet_H_PACKET_SV_START;
    handlers[PACKET_SV_DISCONNECT] = ClientNet_H_PACKET_SV_DISCONNECT;
    handlers[PACKET_SV_MESSAGE] = ClientNet_H_PACKET_SV_MESSAGE;
    handlers[PACKET_CL_MESSAGE] = ClientNet_H_NOOP;
    for (int i = PACKET_FAKE_MAX; i < 256; i++) {
        handlers[i] = ClientNet_H_NOOP;
    }
}

int ClientNet_RecvFromServer(Game *game, int timeout) {
    int active_socket = SDLNet_CheckSockets(game->server_socket_set, timeout);
    if (active_socket) {
        char buffer[4096];
        int num_bytes_received = SDLNet_TCP_Recv(game->server_socket, buffer, sizeof(buffer));
        uint8_t packet_id = *(uint8_t *)buffer;
        int done = handlers[packet_id](game, buffer, num_bytes_received);
        return done;
    }
    return 0;
}

void ClientNet_SendInputToServer(Game *game) {
    char data[1024];
    int len_data = 0;
    *(int *)data = game->my_player_id;
    len_data += sizeof(int);
    memcpy(data + len_data, &game->players[game->my_player_id].input, sizeof(Input));
    len_data += sizeof(Input);
    NetProtocol_SendPacketToServer(game->server_socket, PACKET_SV_MESSAGE, data, len_data);
}
