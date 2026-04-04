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
};

enum {
    SVMSG_PLAYER_POS,
    SVMSG_PLAYER_HEALTH,
    SVMSG_PROJECTILE_NEW,
    SVMSG_TERRAIN_DESTROY,
    SVMSG_GAME_OVER,
};

typedef struct {
    uint8_t type;
    union {
        struct { uint8_t id; float x; float y; } player_pos;
        struct { uint8_t id; float health; } player_health;
        struct { uint8_t owner_id; float x, y, angle, power; int type; } projectile_new;
        struct { float x, y; float radius; } terrain_destroy;
        struct { int winner; } game_over;
    } data;
} sv_msg_t;

enum {
    CLMSG_MOVE,
    CLMSG_PROJECTILE,
};

typedef struct {
    uint8_t type;
    union {
        struct { uint8_t left, right; } move;
        struct { int type; float angle, power;} projectile;
    } data;
} cl_msg_t;

#define EACH_CLIENT_MAX_MESSAGES 10
typedef struct {
    int id;
    int active;
    TCPsocket socket;
    struct { uint8_t pending; cl_msg_t content; } msgs[EACH_CLIENT_MAX_MESSAGES];
} sv_client_t;

void net_SendPacketToClient(sv_client_t *client, uint8_t packet_id, void *data, int len_data);
void net_SendPacketToServer(TCPsocket socket_to_send, uint8_t packet_id, void *data, int len_data);

#endif
