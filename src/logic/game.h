#ifndef _GAME_H
#define _GAME_H

#include <SDL2/SDL.h>
#include "../common/config.h"
#include "../struct/st_game.h"

void Game_Init(Game *game);
SDL_Texture *Game_LoadImage(Game *game, char *img_path);
void Game_Load(Game *game);
int Game_Update(Game *game);
void Game_Render(Game *game);
void Game_Clean(Game *game);

#endif
