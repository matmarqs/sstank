#ifndef _CLIENT_MAIN_H
#define _CLIENT_MAIN_H

#include <SDL2/SDL.h>

#include "cl_types.h"
#include "../shared/base_config.h"

void Client_Init(ClientState *client, char *ip_addr);
SDL_Texture *Client_LoadImage(ClientState *client, char *img_path);
void Client_Load(ClientState *client);
int Client_Update(ClientState *client);
void Client_Render(ClientState *client);
void Client_Clean(ClientState *client, int exit_code);

#endif
