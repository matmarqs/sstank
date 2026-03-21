#ifndef _CORE_PROJECTILE_H
#define _CORE_PROJECTILE_H

#include <SDL2/SDL.h>

#include "core_types.h"

#define PROJECTILE_WIDTH 80
#define PROJECTILE_HEIGHT PROJECTILE_WIDTH
#define BOMB_RADIUS PROJECTILE_WIDTH

int  Projectile_Load(ProjectileSystem *ps, SDL_Renderer *renderer);
void Projectile_Update(ProjectileSystem *ps, GameState *game);
void Projectile_Render(ProjectileSystem *ps, SDL_Renderer *renderer);
void Projectile_Clean(ProjectileSystem *ps);

void Projectile_Throw(ProjectileSystem *ps, int type, float x, float y, float angle, float power, int owner);
void Projectile_RemoveInactive(ProjectileSystem *ps);

#endif
