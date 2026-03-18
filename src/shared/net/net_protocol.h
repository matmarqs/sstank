#ifndef _NET_PROTOCOL_H
#define _NET_PROTOCOL_H

#include <stdint.h>
#include <SDL2/SDL_net.h>

enum {
    PACKET_SERVER_WELCOME_YOUR_ID,
    PACKET_SERVER_IS_FULL,
    PACKET_SERVER_WAIT_OTHER_PLAYER,
    PACKET_SERVER_GAME_START,
    PACKET_SERVER_OTHER_PLAYER_DISCONNECTED,
    PACKET_CLIENT_INPUT,
    PACKET_SERVER_MESSAGE,
    PACKET_MAX_FAKEPACKET,
};
typedef uint8_t PacketID;

enum {
    UPDATE_PLAYER_POS,
    UPDATE_PLAYER_HEALTH,
    UPDATE_PROJECTILE_NEW,
    UPDATE_TERRAIN_DESTROY,
};
typedef uint8_t MessageType;

typedef struct {
    MessageType type;
    uint32_t sequence;
    union {
        struct { uint8_t id; float x; float y; } player_pos;
        struct { uint8_t id; float health; } player_health;
        struct { uint8_t id; float x, y; float angle, power; float type; } projectile_new;
        struct { float x, y; float radius; } terrain_destroy;
    } data;
} UpdatePacket;

typedef struct {
    TCPsocket socket;
    int id;
    int active;
} Client;

void NetProtocol_SendPacketToClient(Client *client, uint8_t packet_id, void *data, int len_data);
void NetProtocol_SendPacketToServer(TCPsocket socket_to_send, uint8_t packet_id, void *data, int len_data);

#endif
