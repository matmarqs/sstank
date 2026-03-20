#ifndef _STRUCT_PROJECTILE_H
#define _STRUCT_PROJECTILE_H

#define MAX_PROJECTILES     10

typedef enum {
    PROJECTILE_ACTIVE,
    PROJECTILE_EXPLODING,
    PROJECTILE_INACTIVE,
} ProjectileState;

typedef struct {
    int type;
    float x, y;
    float vx, vy;
    float w, h;
    int owner;
    ProjectileState state;
} Projectile;

typedef struct {
    Projectile projectiles[MAX_PROJECTILES];
    int count;
} ProjectileSystem;

#endif
