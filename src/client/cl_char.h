#ifndef _CLIENT_CHAR_H
#define _CLIENT_CHAR_H

#include "cl_types.h"

void ClientChar_Update(cl_state_t *client);
void ClientChar_RenderPowerGauge(cl_char_t *cl_char, SDL_Renderer *renderer);
void ClientChar_RenderAngle(cl_char_t *cl_char, SDL_Renderer *renderer);
void ClientChar_ShootingHandler(cl_state_t *client);
void ClientChar_Init(cl_state_t *client);

#endif
