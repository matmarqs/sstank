#ifndef _STRUCT_PHYSICS_H
#define _STRUCT_PHYSICS_H

typedef enum {
    GROUNDED,    // Standing on ground
    FALLING,     // In the air
} PlayerMoveState;

typedef struct {
    int has_ground;
    float ground_y;        // Y coordinate of ground below
} GroundContact;

#endif
