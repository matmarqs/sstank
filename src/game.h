#ifndef GAME_H
#define GAME_H

#include "common.h"
#include "player.h"
#include "input.h"
#include "projectile.h"
#include "terrain.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdlib.h>

#define NUM_PLAYERS 2

struct Game {
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Event event;

    Terrain terrain;

    Input input;

    Player players[NUM_PLAYERS];

    ProjectileSystem projectile_sys;

    int w, h;

    int time;
};

void Game_Init(Game *game);
SDL_Texture *Game_LoadImage(Game *game, char *img_path);
void Game_Load(Game *game);
int Game_Update(Game *game);
void Game_Render(Game *game);
void Game_Clean(Game *game);

#endif
