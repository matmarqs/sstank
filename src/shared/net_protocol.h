#ifndef _NET_PROTOCOL_H
#define _NET_PROTOCOL_H

#include <stdint.h>
#include <SDL2/SDL_net.h>

enum {
    PACKET_SV_WELCOME,
    PACKET_SV_FULL,
    PACKET_SV_WAITING,
    PACKET_SV_START,
    PACKET_SV_DISCONNECT,
    PACKET_SV_MESSAGE,
    PACKET_CL_MESSAGE,
    PACKET_FAKE_MAX,
};

enum {
    SVMSG_PLAYER_POS,
    SVMSG_PLAYER_HEALTH,
    SVMSG_PROJECTILE_NEW,
    SVMSG_TERRAIN_DESTROY,
};

typedef struct {
    uint8_t type;
    uint32_t sequence;
    union {
        struct { uint8_t id; float x; float y; } player_pos;
        struct { uint8_t id; float health; } player_health;
        struct { uint8_t id; float x, y, angle, power; int type; } projectile_new;
        struct { float x, y; float radius; } terrain_destroy;
    } data;
} ServerMessage;

enum {
    CLMSG_PLAYER_INPUT,
    CLMSG_PLAYER_THROW,
};

typedef struct {
    uint8_t type;
    union {
        struct { uint8_t left, right; } player_input;
        struct { float angle, power; int type; } projectile_throw;
    } data;
} ClientMessage;

typedef struct {
    TCPsocket socket;
    int id;
    int active;
} Client;

void NetProtocol_SendPacketToClient(Client *client, uint8_t packet_id, void *data, int len_data);
void NetProtocol_SendPacketToServer(TCPsocket socket_to_send, uint8_t packet_id, void *data, int len_data);

#endif
