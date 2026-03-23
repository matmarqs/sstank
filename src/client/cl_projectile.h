#ifndef _CLIENT_PROJECTILE_H
#define _CLIENT_PROJECTILE_H

#include "cl_types.h"

int cl_projectile_Load(cl_projectile_sys_t *cl_ps, ProjectileSystem *ps, SDL_Renderer *renderer);
void cl_projectile_Throw(cl_projectile_sys_t *ps, int type, float x, float y, float angle, float power, int owner);
void cl_projectile_Update(cl_projectile_sys_t *ps, GameState *game);
void cl_projectile_Render(cl_projectile_sys_t *ps, SDL_Renderer *renderer);
void cl_projectile_Clean(cl_projectile_sys_t *cl_ps);

#endif
