#ifndef _TERRAIN_H
#define _TERRAIN_H

#include <SDL2/SDL.h>

#include "../struct/st_terrain.h"
#include "../common/config.h"

#define TERRAIN_WIDTH  WORLD_WIDTH
#define TERRAIN_HEIGHT WORLD_HEIGHT

// Core functions
int  Terrain_Init(Terrain *terr, SDL_Renderer *renderer);
int  Terrain_LoadFromPNG(Terrain *terr, SDL_Renderer *renderer, char *bg_path, char *fg_path);
void Terrain_Clean(Terrain *terr);

// Rendering
void Terrain_Render(Terrain *terr, SDL_Renderer *renderer);

// Debug
void Terrain_DebugSave(Terrain *terr, char *filename);

// Destruction functions
void Terrain_DestroyCircle(Terrain *terr, float cx, float cy, float radius);
void Terrain_DestroyRect(Terrain *terr, float x, float y, float w, float h);
// Query functions
int Terrain_IsSolid(Terrain *terr, int x, int y);
int Terrain_CheckCollision(Terrain *terr, float x, float y, float w, float h);

float Terrain_FindGroundY(Terrain *terr, float center_x, float current_y);
float GetGroundY(Terrain *terr, float x);

float GetGroundYBelow(Terrain *terr, float x, float start_y);
int IsGroundedAt(Terrain *terr, float x, float y, float height);

#endif
