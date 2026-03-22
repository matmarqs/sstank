#ifndef _CLIENT_PLAYER_H
#define _CLIENT_PLAYER_H

#include "cl_types.h"

int ClientPlayer_Load(cl_player_t *cl_player, SDL_Renderer *renderer);
void ClientPlayer_AnimationHandler(cl_player_t *p, GameState *game);
void ClientPlayer_Update(cl_player_t *p, GameState *game, PlayerActions actions, float dt);
void ClientPlayer_RenderPowerGauge(cl_char_t *cl_char, SDL_Renderer *renderer);
void ClientPlayer_RenderAngle(cl_char_t *cl_char, SDL_Renderer *renderer);
void ClientPlayer_Render(cl_player_t *cl_p, SDL_Renderer *renderer);
void ClientPlayer_ShootingHandler(cl_char_t *cl_char, GameState *game);
void ClientPlayer_Clean(cl_player_t *p);

#endif
