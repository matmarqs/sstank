#ifndef _CORE_TYPES_H
#define _CORE_TYPES_H

#include "base.h"

#define NUM_PLAYERS 2
#define MAX_PROJECTILES 10
#define MAX_POWER 500

#define PLAYER_SPEED 300   /* speed in pixels/sec */
#define BASE_PLAYER_HEIGHT 60
#define BASE_PLAYER_WIDTH BASE_PLAYER_HEIGHT

#define TERRAIN_WIDTH  WORLD_WIDTH
#define TERRAIN_HEIGHT WORLD_HEIGHT

#define PROJECTILE_WIDTH 80
#define PROJECTILE_HEIGHT PROJECTILE_WIDTH
#define BOMB_RADIUS PROJECTILE_WIDTH

typedef struct {
    uint8_t move_left : 1;   // 1 bit
    uint8_t move_right : 1;
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
    int explosion_timer;
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
    int id;

    float x, y;
    float vx, vy;
    float w, h;

    int alive;
    float health;

    int on_ground;
    int facing_right;
    int throw_cooldown;

    uint32_t last_processed_seq;
} PlayerState;

typedef struct {
    int time;
    int w, h;
    Terrain terrain;
    PlayerState players[NUM_PLAYERS];
    ProjectileSystem projectile_sys;
} GameState;

#endif
