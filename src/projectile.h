#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "common.h"
#include <SDL2/SDL_render.h>

#define MAX_PROJECTILES     10
#define PROJECTILE_WIDTH    80
#define PROJECTILE_HEIGHT   PROJECTILE_WIDTH
#define PROJECTILE_NUM_SPRITES  8
#define BOMB_RADIUS PROJECTILE_WIDTH

typedef enum {
    PROJECTILE_ACTIVE,
    PROJECTILE_EXPLODING,
    PROJECTILE_INACTIVE,
} ProjectileState;

typedef struct {
    float x, y;
    float vx, vy;
    float angle;
    float w, h;
    int curr_sprite;
    ProjectileState state;
    int facing_right;
    int owner;
    int explosion_timer;
} Projectile;

typedef struct {
    SDL_Texture *sprites[PROJECTILE_NUM_SPRITES];
    Projectile projectiles[MAX_PROJECTILES];
    int count;
} ProjectileSystem;

int  Projectile_Load(ProjectileSystem *ps, SDL_Renderer *renderer);
void Projectile_Update(ProjectileSystem *ps, Game *game);
void Projectile_Draw(ProjectileSystem *ps, SDL_Renderer *renderer);
void Projectile_Clean(ProjectileSystem *ps);

void Projectile_Throw(ProjectileSystem *ps, float x, float y, float angle, float power, int owner);
void Projectile_RemoveInactive(ProjectileSystem *ps);

#endif
