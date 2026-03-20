#include "logic_movement.h"
#include "logic_terrain.h" // Terrain_IsSolid, ....

// ===== STATE DETERMINATION =====
PlayerMoveState Movement_DeterminePlayerState(Terrain *terr, Player *p) {
    // Check both bottom corners for ground
    float feet_y = p->y + p->h;
    int left_ground = Terrain_IsSolid(terr, p->x, feet_y + 1);
    int right_ground = Terrain_IsSolid(terr, p->x + p->w, feet_y + 1);

    return (left_ground || right_ground) ? GROUNDED : FALLING;
}

void Movement_UpdateGrounded(Terrain *terr, Player *p, float input_vx) {
    // No input? nothing to do
    if (input_vx == 0) {
        p->vx = 0;
        return;
    }

    // Try to move horizontally
    float new_x = p->x + input_vx / 60.0f;

    // Check if we can stand at new position
    if (!Terrain_CheckCollision(terr, new_x, p->y, p->w, p->h)) {
        // Free space? Just move there
        p->x = new_x;
        p->vx = input_vx;
        return;
    }

    // Can't move straight? Try stepping up (max 20 pixels)
    for (int step = 0; step <= 15; step++) {
        float try_y = p->y - step;
        if (!Terrain_CheckCollision(terr, new_x, try_y, p->w, p->h)) {
            p->x = new_x;
            p->vx = input_vx;
            p->y = try_y;
            return;
        }
    }

    // Can't step up? Must be a wall
    p->vx = 0;
}

void Movement_UpdateFalling(Terrain *terr, Player *p, float input_vx) {
    // Apply gravity (max fall speed)
    p->vy += GRAVITY / 60.0f;

    // Apply horizontal movement
    p->vx = input_vx / 60.0f;
    float new_x = p->x + p->vx;

    if (p->vx != 0 && !Terrain_CheckCollision(terr, new_x, p->y, p->w, p->h)) {
        p->x = new_x;
    }

    // Apply vertical movement
    float new_y = p->y + p->vy / 60.0f;

    // Check collision at new position
    if (Terrain_CheckCollision(terr, p->x, new_y, p->w, p->h)) {
        // Find exact ground level
        float ground_level = -1;
        for (int y = p->y; y <= new_y + 1; y++) {
            if (Terrain_CheckCollision(terr, p->x, y, p->w, p->h)) {
                ground_level = y - 1;
                break;
            }
        }

        if (ground_level >= 0) {
            p->y = ground_level;
            p->vy = 0;
            p->on_ground = 1;
        }
    } else {
        p->y = new_y;
    }
}
