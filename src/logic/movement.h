#include "../struct/st_movement.h"
#include "../struct/st_player.h"
#include "../struct/st_terrain.h"

PlayerMoveState Movement_DeterminePlayerState(Terrain *terr, Player *p);
GroundContact Movement_CheckGroundContact(Terrain *terr, Player *p);
void Movement_UpdateGrounded(Terrain *terr, Player *p, float input_vx);
void Movement_UpdateFalling(Terrain *terr, Player *p, float input_vx);
