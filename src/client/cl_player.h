#ifndef _CLIENT_PLAYER_H
#define _CLIENT_PLAYER_H

#include "cl_types.h"

int ClientPlayer_Load(cl_player_t *cl_player, SDL_Renderer *renderer);
void ClientPlayer_AnimationHandler(cl_player_t *p, GameState *game);
void ClientPlayer_Update(cl_player_t *p, GameState *game, PlayerActions actions, float dt);
void ClientPlayer_Render(cl_player_t *cl_p, SDL_Renderer *renderer);
void ClientPlayer_Clean(cl_player_t *p);
void ClientPlayer_TakeDamage(cl_player_t *cl_player, float new_health);

#endif
