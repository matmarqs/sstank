#ifndef _CORE_TERRAIN_H
#define _CORE_TERRAIN_H

#include "core_types.h"

// Helper functions
int mask_index(int width, int x, int y);
int valid_coord(int x, int y, int width, int height);

// Core functions
int Terrain_Init(Terrain *terr);
int Terrain_Load(Terrain *terr, char *fg_path);
void Terrain_Clean(Terrain *terr);

// Destruction functions
void Terrain_DestroyCircle(Terrain *terr, float cx, float cy, float radius);

// Query functions
int Terrain_IsSolid(Terrain *terr, int x, int y);

#endif
