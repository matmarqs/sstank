#ifndef _CLIENT_MAIN_H
#define _CLIENT_MAIN_H

#include "cl_types.h"

void Client_Init(cl_state_t *client, char *ip_addr);
SDL_Texture *Client_LoadImage(cl_state_t *client, char *img_path);
void Client_Load(cl_state_t *client);
int Client_Update(cl_state_t *client);
void Client_Render(cl_state_t *client);
void Client_Clean(cl_state_t *client, int exit_code);

#endif
