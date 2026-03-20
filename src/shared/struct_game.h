#ifndef _STRUCT_GAME_H
#define _STRUCT_GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include "struct_player.h"
#include "struct_projectile.h"
#include "struct_terrain.h"

#define NUM_PLAYERS 2

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Event event;

    Terrain terrain;

    Input input;

    int my_player_id;
    int start;

    Player players[NUM_PLAYERS];

    ProjectileSystem projectile_sys;

    int w, h;

    int time;

    TCPsocket server_socket;
    SDLNet_SocketSet server_socket_set;
} Game;

#endif
