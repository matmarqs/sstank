#ifndef _CLIENT_STATE_T_H
#define _CLIENT_STATE_T_H

#include "../shared/struct_game.h"

typedef struct {
    GameState game;

    int start;

    int my_player_id;

    Input input;

    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Event event;

    TCPsocket server_socket;
    SDLNet_SocketSet server_socket_set;
} ClientState;

#endif
