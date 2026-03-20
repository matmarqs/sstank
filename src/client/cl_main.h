#ifndef _CLIENT_MAIN_H
#define _CLIENT_MAIN_H

#include <SDL2/SDL.h>

#include "../shared/base_config.h"
#include "../shared/struct_game.h"

void Client_Init(Game *game, char *ip_addr);
SDL_Texture *Client_LoadImage(Game *game, char *img_path);
void Client_Load(Game *game);
int Client_Update(Game *game);
void Client_Render(Game *game);
void Client_Clean(Game *game, int exit_code);

#endif
