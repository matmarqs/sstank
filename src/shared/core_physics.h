#ifndef _CORE_PHYSICS_H
#define _CORE_PHYSICS_H

#include "core_types.h"

int Physics_CheckCollision(Terrain *terr, float x, float y, float w, float h);

PlayerMoveState Physics_DeterminePlayerState(Terrain *terr, PlayerState *p);
void Physics_UpdateGrounded(Terrain *terr, PlayerState *p, float input_vx);
void Physics_UpdateFalling(Terrain *terr, PlayerState *p, float input_vx);

float Physics_FindGroundY(Terrain *terr, float center_x, float current_y);
float Physics_GetGroundY(Terrain *terr, float x);
float Physics_GetGroundYBelow(Terrain *terr, float x, float start_y);
int Physics_IsGroundedAt(Terrain *terr, float x, float y, float height);

#endif
