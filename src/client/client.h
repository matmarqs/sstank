#ifndef _GAME_H
#define _GAME_H

#include <SDL2/SDL.h>

#include "../shared/common/config.h"
#include "../shared/struct/st_game.h"

void Client_Init(Game *game);
SDL_Texture *Client_LoadImage(Game *game, char *img_path);
void Client_Load(Game *game);
int Client_Update(Game *game);
void Client_Render(Game *game);
void Client_Clean(Game *game);

#endif
