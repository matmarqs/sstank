#include <SDL2/SDL_image.h>
#include "core_terrain.h"

int mask_index(int width, int x, int y) {
    return y * width + x;
}

int valid_coord(int x, int y, int width, int height) {
    return (x >= 0 && x < width && y >= 0 && y < height);
}

int Terrain_Init(Terrain *terr) {
    terr->width = TERRAIN_WIDTH;
    terr->height = TERRAIN_HEIGHT;

    // Allocate destruction mask (1 byte per pixel)
    terr->destruction_mask = calloc(terr->width * terr->height, sizeof(uint8_t));
    if (!terr->destruction_mask) {
        Debug_Error("Failed to allocate destruction mask");
        return FAILURE;
    }

    return SUCCESS;
}

int Terrain_Load(Terrain *terr, char *fg_path) {
    SDL_Surface *fg_surface = IMG_Load(fg_path);
    if (!fg_surface) {
        Debug_Error("Failed to load foreground: %s", fg_path);
        return FAILURE;
    }

    SDL_Surface *fg = SDL_ConvertSurfaceFormat(fg_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(fg_surface);

    // STEP 4: Initialize destruction mask from foreground alpha
    int pixels_solid_count = 0;
    Uint32 *fg_pixels = (Uint32*)fg->pixels;
    SDL_PixelFormat *format = fg->format;

    for (int y = 0; y < terr->height; y++) {
        for (int x = 0; x < terr->width; x++) {
            int idx = y * terr->width + x;
            Uint32 pixel = fg_pixels[idx];

            // CORRECT WAY: Use SDL_GetRGBA to extract components
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, format, &r, &g, &b, &a);

            // Transparent pixels in foreground = holes from the start
            terr->destruction_mask[idx] = (a < 128) ? 1 : 0;

            if (a >= 128) {
                pixels_solid_count++;
            }
        }
    }
    Debug_Info("Loaded pixels_solid_count = %d", pixels_solid_count);

    // Verify dimensions
    if (fg->w != terr->width || fg->h != terr->height) {
        Debug_Warn("Foreground dimensions %dx%d don't match expected %dx%d",
                  fg->w, fg->h, terr->width, terr->height);
    }

    SDL_FreeSurface(fg);

    return SUCCESS;
}

void Terrain_Clean(Terrain *terr) {
    if (terr->destruction_mask) free(terr->destruction_mask);
}

void Terrain_DestroyCircle(Terrain *terr, float cx, float cy, float radius) {
    // Calculate bounds to loop over
    int min_x = MAX(0, cx - radius);
    int max_x = MIN(terr->width - 1, cx + radius);
    int min_y = MAX(0, cy - radius);
    int max_y = MIN(terr->height - 1, cy + radius);

    int pixels_destroyed = 0;

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            int dx = x - cx;
            int dy = y - cy;

            // Circle test
            if (dx*dx + dy*dy <= radius*radius) {
                int idx = mask_index(terr->width, x, y);
                if (!terr->destruction_mask[idx]) {
                    terr->destruction_mask[idx] = 1;
                    pixels_destroyed++;
                }
            }
        }
    }

    if (pixels_destroyed > 0) {
        Debug_Info("Destroyed %d pixels at (%.2f,%.2f) radius %.2f",
                  pixels_destroyed, cx, cy, radius);
    }
}

int Terrain_IsSolid(Terrain *terr, int x, int y) {
    if (!valid_coord(x, y, terr->width, terr->height)) return 0;
    return !terr->destruction_mask[mask_index(terr->width, x, y)];
}
