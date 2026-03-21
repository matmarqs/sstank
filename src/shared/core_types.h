#ifndef _CORE_TYPES_H
#define _CORE_TYPES_H

#include "base.h"

#define NUM_PLAYERS 2
#define MAX_PROJECTILES 10

typedef struct {
    uint8_t move_left : 1;   // 1 bit
    uint8_t move_right : 1;
    uint8_t jump : 1;
    uint8_t shoot : 1;
    uint8_t aim_up : 1;
    uint8_t aim_down : 1;
    // 2 bits unused
} PlayerActions;

typedef struct {
    // Destruction mask (1 = destroyed/hole, 0 = solid)
    uint8_t *destruction_mask;
    int width, height;
} Terrain;

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

typedef enum {
    GROUNDED,    // Standing on ground
    FALLING,     // In the air
} PlayerMoveState;

typedef struct {
    int has_ground;
    float ground_y;        // Y coordinate of ground below
} GroundContact;

typedef struct {
    float x, y;
    float vx, vy;
    float w, h;

    int alive;
    float health;

    int on_ground;
    int facing_right;
    int throw_cooldown;
} PlayerState;

typedef struct {
    int time;
    int w, h;
    Terrain terrain;
    PlayerState players[NUM_PLAYERS];
    ProjectileSystem projectile_sys;
} GameState;

#endif
