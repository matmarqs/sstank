#ifndef _CLIENT_CHAR_H
#define _CLIENT_CHAR_H

#include "cl_types.h"

void cl_char_Init(cl_char_t *cl_char_t, cl_player_t *cl_player, TCPsocket server);

void cl_char_Update(cl_char_t *cl_char_t);
void cl_char_MovingHandler(cl_char_t *cl_char);
void cl_char_ShootingHandler(cl_char_t *cl_char);

void cl_char_RenderPowerGauge(cl_char_t *cl_char, SDL_Renderer *renderer);
void cl_char_RenderAngle(cl_char_t *cl_char, SDL_Renderer *renderer);

#endif
