#ifndef _STRUCT_TERRAIN_H
#define _STRUCT_TERRAIN_H

#include <SDL2/SDL.h>

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

#endif
