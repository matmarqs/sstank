#ifndef _SERVER_TYPES_H
#define _SERVER_TYPES_H

#include "../shared/core_types.h"
#include "../shared/net_protocol.h"

#define PORT 5555
#define MAX_CLIENTS 2
#define TICK_RATE 60
#define TICK_TIME_MS (1000 / TICK_RATE)

typedef struct {
    sv_client_t clients[MAX_CLIENTS];
    TCPsocket socket;
    SDLNet_SocketSet socket_set;
    GameState game;  // Server maintains authoritative game state
    int game_running;
    uint32_t last_tick;
} sv_server_t;

#endif
