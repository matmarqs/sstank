#ifndef _CLIENT_TERRAIN_H
#define _CLIENT_TERRAIN_H

#include "cl_types.h"

void cl_terrain_Clean(cl_terrain_t *cl_terr);
int cl_terrain_Init(cl_terrain_t *cl_terr, Terrain *terrain, SDL_Renderer *renderer);
int cl_terrain_Load(cl_terrain_t *cl_terr, char *bg_path, char *fg_path);
void cl_terrain_DestroyCircle(cl_terrain_t *cl_terr, float cx, float cy, float radius);
void cl_terrain_UpdateTexture(cl_terrain_t *cl_terr);
void cl_terrain_Render(cl_terrain_t *cl_terr, SDL_Renderer *renderer);

#endif
