#ifndef _STRUCT_PROJECTILE_H
#define _STRUCT_PROJECTILE_H

#include <SDL2/SDL.h>

#define MAX_PROJECTILES     10
#define PROJECTILE_NUM_SPRITES  8

typedef enum {
    PROJECTILE_ACTIVE,
    PROJECTILE_EXPLODING,
    PROJECTILE_INACTIVE,
} ProjectileState;

typedef struct {
    int type;
    float x, y;
    float vx, vy;
    float angle;
    float w, h;
    ProjectileState state;
    int facing_left;
    int owner;
    int explosion_timer;
} Projectile;

typedef struct {
    Projectile projectiles[MAX_PROJECTILES];
    SDL_Texture *sprites[PROJECTILE_NUM_SPRITES];
    int count;
} ProjectileSystem;

#endif
