#ifndef TERRAIN_H
#define TERRAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "common.h"

#define TERRAIN_WIDTH  WORLD_WIDTH
#define TERRAIN_HEIGHT WORLD_HEIGHT

typedef struct {
    // The two source images
    SDL_Texture *background;  // Layer 1 - static beauty
    SDL_Texture *foreground;  // Layer 2 - destructible gameplay layer

    // The combined result texture (streamed)
    SDL_Texture *render_texture;

    // Destruction mask (1 = destroyed/hole, 0 = solid)
    uint8_t *destruction_mask;
    int width, height;

    // For optimization: track if we need to rebuild
    int dirty;

    // Surface cache (to avoid repeated pixel reads)
    SDL_Surface *bg_surface;
    SDL_Surface *fg_surface;
} Terrain;

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
