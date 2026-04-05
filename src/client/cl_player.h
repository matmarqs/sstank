#ifndef _CLIENT_PLAYER_H
#define _CLIENT_PLAYER_H

#include "cl_types.h"

int cl_player_Init(cl_player_t *cl_player, PlayerState *p, SDL_Renderer *renderer);
void cl_player_MovementHandler(cl_player_t *cl_player, float x, float y);
void cl_player_AnimationHandler(cl_player_t *p, GameState *game);
void cl_player_Update(cl_player_t *p, GameState *game);
void cl_player_Render(cl_player_t *cl_p, SDL_Renderer *renderer);
void cl_player_Clean(cl_player_t *p);
void cl_player_TakeDamage(cl_player_t *cl_player, float new_health);

#endif
