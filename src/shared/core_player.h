#ifndef _CORE_PLAYER_H
#define _CORE_PLAYER_H

#include "core_types.h"

#define SPEED 300   /* speed in pixels/sec */
#define BASE_PLAYER_HEIGHT 60
#define BASE_PLAYER_WIDTH BASE_PLAYER_HEIGHT

#define MAX_POWER 500

void Player_Init(PlayerState *p);
void Player_Update(PlayerState *p, GameState *game);
void Player_Teleport(PlayerState *p, float x, float y);
void Player_MovementHandler(PlayerState *p, GameState *game);

#endif
