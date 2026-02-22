#ifndef GAME_H
#define GAME_H

#include "common.h"
#include "player.h"
#include "input.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdlib.h>

#define WIN_WIDTH  (1200)
#define WIN_HEIGHT (900)

#define GAME_TITLE  "Projectile Game"

struct Game {
    SDL_Window *win;
    SDL_Renderer *renderer;

    SDL_Event event;

    Input input;

    Player players[2];

    int time;
};

void Game_Init(Game *game);
SDL_Texture *Game_LoadImage(Game *game, char *img_path);
void Game_Load(Game *game);
int Game_Update(Game *game);
void Game_Draw (Game *game);
void Game_Clean(Game *game);

#endif
