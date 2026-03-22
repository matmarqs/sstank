#ifndef _CLIENT_TERRAIN_H
#define _CLIENT_TERRAIN_H

#include "cl_types.h"

void ClientTerrain_Clean(cl_terrain_t *cl_terr);
int ClientTerrain_Init(cl_terrain_t *cl_terr, SDL_Renderer *renderer);
int ClientTerrain_Load(cl_terrain_t *cl_terr, char *bg_path, char *fg_path);
void ClientTerrain_UpdateTexture(cl_terrain_t *cl_terr);
void ClientTerrain_Render(cl_terrain_t *cl_terr, SDL_Renderer *renderer);

#endif
