#ifndef _CLIENT_PROJECTILE_H
#define _CLIENT_PROJECTILE_H

#include "cl_types.h"

int ClientProjectile_Load(cl_projectile_sys_t *cl_ps, SDL_Renderer *renderer);
void ClientProjectile_Throw(cl_projectile_sys_t *ps, int type, float x, float y, float angle, float power, int owner);
void ClientProjectile_Update(cl_projectile_sys_t *ps, GameState *game);
void ClientProjectile_Render(cl_projectile_sys_t *ps, SDL_Renderer *renderer);
void ClientProjectile_Clean(cl_projectile_sys_t *cl_ps);

#endif
