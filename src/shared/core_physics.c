#include "base.h"
#include "core_physics.h"
#include "core_terrain.h" // Terrain_IsSolid

int Physics_CheckCollision(Terrain *terr, float x, float y, float w, float h) {
    int min_x = MAX(0, (int)x);
    int max_x = MIN(terr->width - 1, (int)(x + w));
    int min_y = MAX(0, (int)y);
    int max_y = MIN(terr->height - 1, (int)(y + h));
    for (int ty = min_y; ty <= max_y; ty++) {
        for (int tx = min_x; tx <= max_x; tx++) {
            if (Terrain_IsSolid(terr, tx, ty)) {
                return 1;  // Collision!
            }
        }
    }
    return 0;
}

PlayerMoveState Physics_DeterminePlayerState(Terrain *terr, PlayerState *p) {
    // Check both bottom corners for ground
    float feet_y = p->y + p->h;
    int left_ground = Terrain_IsSolid(terr, p->x, feet_y + 1);
    int right_ground = Terrain_IsSolid(terr, p->x + p->w, feet_y + 1);
    return (left_ground || right_ground) ? GROUNDED : FALLING;
}

void Physics_UpdateGrounded(Terrain *terr, PlayerState *p, float dx) {
    // No input? nothing to do
    if (dx == 0) {
        p->vx = 0;
        return;
    }
    // Try to move horizontally
    float new_x = p->x + dx;
    // Check if we can stand at new position
    if (!Physics_CheckCollision(terr, new_x, p->y, p->w, p->h)) {
        // Free space? Just move there
        p->x = new_x;
        p->vx = dx;
        return;
    }
    // Can't move straight? Try stepping up (max 20 pixels)
    for (int step = 0; step <= 15; step++) {
        float try_y = p->y - step;
        if (!Physics_CheckCollision(terr, new_x, try_y, p->w, p->h)) {
            p->x = new_x;
            p->vx = dx;
            p->y = try_y;
            return;
        }
    }
    // Can't step up? Must be a wall
    p->vx = 0;
}

void Physics_UpdateFalling(Terrain *terr, PlayerState *p, float input_vx) {
    // Apply gravity (max fall speed)
    p->vy += GRAVITY / 60.0f;

    // Apply horizontal movement
    p->vx = input_vx / 60.0f;
    float new_x = p->x + p->vx;

    if (p->vx != 0 && !Physics_CheckCollision(terr, new_x, p->y, p->w, p->h)) {
        p->x = new_x;
    }

    // Apply vertical movement
    float new_y = p->y + p->vy / 60.0f;

    // Check collision at new position
    if (Physics_CheckCollision(terr, p->x, new_y, p->w, p->h)) {
        // Find exact ground level
        float ground_level = -1;
        for (int y = p->y; y <= new_y + 1; y++) {
            if (Physics_CheckCollision(terr, p->x, y, p->w, p->h)) {
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
