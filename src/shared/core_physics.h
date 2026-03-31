#ifndef _CORE_PHYSICS_H
#define _CORE_PHYSICS_H

#include "core_types.h"

int Physics_CheckCollision(Terrain *terr, float x, float y, float w, float h);

PlayerMoveState Physics_DeterminePlayerState(Terrain *terr, PlayerState *p);
void Physics_UpdateGrounded(Terrain *terr, PlayerState *p, float vx, float dt);
void Physics_UpdateFalling(Terrain *terr, PlayerState *p, float vx, float dt);
int Physics_CircleRectCollision(float circle_x, float circle_y, float radius,
				float rect_x, float rect_y, float rect_w, float rect_h);

#endif
