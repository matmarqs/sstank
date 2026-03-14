#ifndef _STRUCT_GAME_H
#define _STRUCT_GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include "st_player.h"
#include "st_projectile.h"
#include "st_terrain.h"

#define NUM_PLAYERS 2

typedef struct Game {
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Event event;

    Terrain terrain;

    Input input;

    Player players[NUM_PLAYERS];

    ProjectileSystem projectile_sys;

    int w, h;

    int time;

    TCPsocket server_socket;
} Game;

#endif
