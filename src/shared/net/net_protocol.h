#ifndef _NET_PROTOCOL_H
#define _NET_PROTOCOL_H

#include <stdint.h>
#include <SDL2/SDL_net.h>

typedef enum : uint8_t {
    PACKET_SERVER_WELCOME_YOUR_ID,
    PACKET_SERVER_IS_FULL,
    PACKET_SERVER_WAIT_OTHER_PLAYER,
    PACKET_SERVER_OTHER_PLAYER_DISCONNECTED,
    PACKET_CLIENT_INPUT,
} PacketID;

typedef struct {
    TCPsocket socket;
    int id;
    int active;
} Client;


void NetProtocol_SendPacketToClient(Client *client, PacketID packet_id, void *data, int len_data);

#endif
