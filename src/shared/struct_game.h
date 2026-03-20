#ifndef _STRUCT_GAME_H
#define _STRUCT_GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include "struct_player.h"
#include "struct_projectile.h"
#include "struct_terrain.h"

#define NUM_PLAYERS 2

typedef struct {
    int time;
    int w, h;
    Terrain terrain;
    Player players[NUM_PLAYERS];
    ProjectileSystem projectile_sys;
} GameState;

#endif
