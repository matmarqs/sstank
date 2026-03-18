#include "../shared/net/net_protocol.h"
#include "../shared/struct/st_game.h"
#include "../shared/common/common.h"
#include "../shared/logic/projectile.h"
#include "../shared/logic/terrain.h"
#include "cnet.h"
#include <SDL2/SDL_net.h>

static CNet_Handler handlers[256];

static int CNet_Handler_PACKET_SERVER_WELCOME_YOUR_ID(Game *game, void *data, int len_data) {
    UNUSED(len_data);
    int offset = sizeof(PacketID);
    game->my_player_id = *(int *)(data + offset);
    return 0;
}

static int CNet_Handler_PACKET_SERVER_IS_FULL(Game *game, void *data, int len_data) {
    UNUSED(game);
    UNUSED(data);
    UNUSED(len_data);
    return 1;
}

static int CNet_Handler_NOOP(Game *game, void *data, int len_data) {
    UNUSED(game);
    UNUSED(data);
    UNUSED(len_data);
    return 0;
}

static int CNet_Handler_PACKET_SERVER_GAME_START(Game *game, void *data, int len_data) {
    UNUSED(game);
    UNUSED(data);
    UNUSED(len_data);
    game->start = 1;
    return 0;
}

static int CNet_Handler_PACKET_SERVER_OTHER_PLAYER_DISCONNECTED(Game *game, void *data, int len_data) {
    UNUSED(game);
    UNUSED(data);
    UNUSED(len_data);
    return 1;   // return 1 in order to quit
}

static int CNet_Handler_PACKET_CLIENT_INPUT(Game *game, void *data, int len_data) {
    UNUSED(len_data);
    int offset = sizeof(PacketID);
    int player_id = *(int *)(data + offset);
    offset += 4;
    memcpy(&game->players[player_id].input, data + offset, sizeof(Input));
    return 0;
}

static int CNet_Handler_PACKET_SERVER_MESSAGE(Game *game, void *data, int len_data) {
    UNUSED(len_data);
    int offset = sizeof(PacketID);
    UpdatePacket packet = *(UpdatePacket *)(data + offset);
    uint8_t type = packet.type;
    switch (type) {
        case UPDATE_PLAYER_POS:
            game->players[packet.data.player_pos.id].x = packet.data.player_pos.x;
            game->players[packet.data.player_pos.id].y = packet.data.player_pos.y;
            break;
        case UPDATE_PLAYER_HEALTH:
            game->players[packet.data.player_pos.id].health = packet.data.player_health.health;
            break;
        case UPDATE_PROJECTILE_NEW:
            Projectile_Throw(&game->projectile_sys, packet.data.projectile_new.type,
                             packet.data.projectile_new.x, packet.data.projectile_new.y,
                             packet.data.projectile_new.angle, packet.data.projectile_new.power,
                             packet.data.projectile_new.id);
            break;
        case UPDATE_TERRAIN_DESTROY:
            Terrain_DestroyCircle(&game->terrain,
                                  packet.data.terrain_destroy.x, packet.data.terrain_destroy.y,
                                  packet.data.terrain_destroy.radius);
            break;
        default:
            break;
    }
    return 0;
}

void CNet_InitHandlers() {
    handlers[PACKET_SERVER_WELCOME_YOUR_ID] = CNet_Handler_PACKET_SERVER_WELCOME_YOUR_ID;
    handlers[PACKET_SERVER_IS_FULL] = CNet_Handler_PACKET_SERVER_IS_FULL;
    handlers[PACKET_SERVER_WAIT_OTHER_PLAYER] = CNet_Handler_NOOP;
    handlers[PACKET_SERVER_GAME_START] = CNet_Handler_PACKET_SERVER_GAME_START;
    handlers[PACKET_SERVER_OTHER_PLAYER_DISCONNECTED] = CNet_Handler_PACKET_SERVER_OTHER_PLAYER_DISCONNECTED;
    handlers[PACKET_CLIENT_INPUT] = CNet_Handler_PACKET_CLIENT_INPUT;
    handlers[PACKET_SERVER_MESSAGE] = CNet_Handler_PACKET_SERVER_MESSAGE;
    for (int i = PACKET_MAX_FAKEPACKET; i < 256; i++) {
        handlers[i] = CNet_Handler_NOOP;
    }
}

int CNet_RecvFromServer(Game *game, int timeout) {
    int active_socket = SDLNet_CheckSockets(game->server_socket_set, timeout);
    if (active_socket) {
        char buffer[4096];
        int num_bytes_received = SDLNet_TCP_Recv(game->server_socket, buffer, sizeof(buffer));
        PacketID packet_id = *(PacketID *)buffer;
        int done = handlers[packet_id](game, buffer, num_bytes_received);
        return done;
    }
    return 0;
}

void CNet_SendInputToServer(Game *game) {
    char data[1024];
    int len_data = 0;
    *(int *)data = game->my_player_id;
    len_data += sizeof(int);
    memcpy(data + len_data, &game->players[game->my_player_id].input, sizeof(Input));
    len_data += sizeof(Input);
    NetProtocol_SendPacketToServer(game->server_socket, PACKET_CLIENT_INPUT, data, len_data);
}
