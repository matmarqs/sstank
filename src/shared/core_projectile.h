#ifndef _CORE_PROJECTILE_H
#define _CORE_PROJECTILE_H

#include <SDL2/SDL.h>

#include "core_types.h"

int  Projectile_Load(ProjectileSystem *ps);
void Projectile_Update(ProjectileSystem *ps, GameState *game);
void Projectile_Clean(ProjectileSystem *ps);

int Projectile_Throw(ProjectileSystem *ps, int type, float x, float y, float angle, float power, int owner);

#endif
