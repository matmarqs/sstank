#ifndef _CORE_PLAYER_H
#define _CORE_PLAYER_H

#include "core_types.h"

void Player_Init(PlayerState *p, int player_id);
void Player_Teleport(PlayerState *p, float x, float y);
void Player_MovementHandler(PlayerState *p, GameState *game, PlayerActions actions, float dt);

#endif
