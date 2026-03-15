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
    PACKET_MAX_FAKEPACKET,
};

typedef uint8_t PacketID;

typedef struct {
    TCPsocket socket;
    int id;
    int active;
} Client;

void NetProtocol_SendPacketToClient(Client *client, uint8_t packet_id, void *data, int len_data);
void NetProtocol_SendPacketToServer(TCPsocket socket_to_send, uint8_t packet_id, void *data, int len_data);

#endif
