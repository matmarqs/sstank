#include <math.h>
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

void Physics_UpdateGrounded(Terrain *terr, PlayerState *p, float vx, float dt) {
    // No input? nothing to do
    if (vx == 0) {
        p->vx = 0;
        return;
    }
    // Try to move horizontally
    float new_x = p->x + vx / 60.0 * dt;
    // Check if we can stand at new position
    if (!Physics_CheckCollision(terr, new_x, p->y, p->w, p->h)) {
        // Free space? Just move there
        p->x = new_x;
        p->vx = vx;
        return;
    }
    // Can't move straight? Try stepping up (max 20 pixels)
    for (int step = 0; step <= 15; step++) {
        float try_y = p->y - step;
        if (!Physics_CheckCollision(terr, new_x, try_y, p->w, p->h)) {
            p->x = new_x;
            p->vx = vx;
            p->y = try_y;
            return;
        }
    }
    // Can't step up? Must be a wall
    p->vx = 0;
}

void Physics_UpdateFalling(Terrain *terr, PlayerState *p, float vx, float dt) {
    // Apply gravity (max fall speed)
    p->vy += GRAVITY / 60.0f;

    // Apply horizontal movement
    p->vx = vx / 60.0f;
    float new_x = p->x + p->vx * dt;

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

// Check if circle (explosion) intersects rectangle (player)
int Physics_CircleRectCollision(float circle_x, float circle_y, float radius,
                        float rect_x, float rect_y, float rect_w, float rect_h) {
    // Find closest point on rectangle to circle
    float closestruct_x = fmax(rect_x, fmin(circle_x, rect_x + rect_w));
    float closestruct_y = fmax(rect_y, fmin(circle_y, rect_y + rect_h));
    // Calculate distance from circle center to this closest point
    float dx = circle_x - closestruct_x;
    float dy = circle_y - closestruct_y;
    float distruct_sq = dx*dx + dy*dy;
    return distruct_sq <= radius * radius;
}
