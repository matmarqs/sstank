#ifndef _LOGIC_PHYSICS_H
#define _LOGIC_PHYSICS_H

#include "struct_physics.h"
#include "struct_player.h"
#include "struct_terrain.h"

PlayerMoveState Physics_DeterminePlayerState(Terrain *terr, Player *p);
GroundContact Physics_CheckGroundContact(Terrain *terr, Player *p);
void Physics_UpdateGrounded(Terrain *terr, Player *p, float input_vx);
void Physics_UpdateFalling(Terrain *terr, Player *p, float input_vx);

#endif
