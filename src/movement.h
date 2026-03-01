#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "player.h"
#include "terrain.h"

typedef enum {
    GROUNDED,    // Standing on ground
    FALLING,     // In the air
} PlayerMoveState;

typedef struct {
    int has_ground;
    float ground_y;        // Y coordinate of ground below
} GroundContact;

PlayerMoveState Movement_DeterminePlayerState(Terrain *terr, Player *p);
GroundContact Movement_CheckGroundContact(Terrain *terr, Player *p);
void Movement_UpdateGrounded(Terrain *terr, Player *p, float input_vx);
void Movement_UpdateFalling(Terrain *terr, Player *p, float input_vx);

#endif
