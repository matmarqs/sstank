#include "terrain.h"
#include "debug.h"
#include "renderer.h"
#include <math.h>
#include <string.h>

static inline int mask_index(int width, int x, int y) {
    return y * width + x;
}

static inline int valid_coord(int x, int y, int width, int height) {
    return (x >= 0 && x < width && y >= 0 && y < height);
}

int Terrain_Init(Terrain *terr, SDL_Renderer *renderer) {
    terr->width = TERRAIN_WIDTH;
    terr->height = TERRAIN_HEIGHT;

    // Allocate destruction mask (1 byte per pixel)
    terr->destruction_mask = calloc(terr->width * terr->height, sizeof(uint8_t));
    if (!terr->destruction_mask) {
        Debug_Error("Failed to allocate destruction mask");
        return FAILURE;
    }

    // Create render texture for streaming
    terr->render_texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,  // KEY: Streaming access!
        terr->width, terr->height);

    if (!terr->render_texture) {
        Debug_Error("Failed to create render texture: %s", SDL_GetError());
        free(terr->destruction_mask);
        return FAILURE;
    }

    Debug_Info("Terrain initialized: %dx%d", terr->width, terr->height);
    return SUCCESS;
}

int Terrain_LoadFromPNG(Terrain *terr, SDL_Renderer *renderer, char *bg_path, char *fg_path) {
    // STEP 1: Load as surfaces (CPU memory)
    SDL_Surface *bg_surface = IMG_Load(bg_path);
    if (!bg_surface) {
        Debug_Error("Failed to load background: %s", bg_path);
        return FAILURE;
    }

    SDL_Surface *fg_surface = IMG_Load(fg_path);
    if (!fg_surface) {
        Debug_Error("Failed to load foreground: %s", fg_path);
        SDL_FreeSurface(bg_surface);
        return FAILURE;
    }

    // STEP 2: Convert to consistent format (RGBA8888)
    terr->bg_surface = SDL_ConvertSurfaceFormat(bg_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    terr->fg_surface = SDL_ConvertSurfaceFormat(fg_surface, SDL_PIXELFORMAT_RGBA8888, 0);

    SDL_FreeSurface(bg_surface);
    SDL_FreeSurface(fg_surface);

    // STEP 3: Create textures for rendering (GPU memory)
    terr->background = SDL_CreateTextureFromSurface(renderer, terr->bg_surface);
    terr->foreground = SDL_CreateTextureFromSurface(renderer, terr->fg_surface);

    // STEP 4: Initialize destruction mask from foreground alpha
    int pixels_solid_count = 0;
    Uint32 *fg_pixels = (Uint32*)terr->fg_surface->pixels;
    SDL_PixelFormat *format = terr->fg_surface->format;

    for (int y = 0; y < terr->height; y++) {
        for (int x = 0; x < terr->width; x++) {
            int idx = y * terr->width + x;
            Uint32 pixel = fg_pixels[idx];

            // CORRECT WAY: Use SDL_GetRGBA to extract components
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, format, &r, &g, &b, &a);

            // Debug print occasionally (not every pixel!)
            if (x == 0 && y == 0) {
                Debug_Info("First pixel: R=%d G=%d B=%d A=%d", r, g, b, a);
            }
            if (x == 100 && y == 100) {
                Debug_Info("Pixel at (100,100): R=%d G=%d B=%d A=%d", r, g, b, a);
            }

            // Transparent pixels in foreground = holes from the start
            terr->destruction_mask[idx] = (a < 128) ? 1 : 0;

            if (a >= 128) {
                pixels_solid_count++;
            }
        }
    }
    Debug_Info("Loaded pixels_solid_count = %d", pixels_solid_count);

    // Verify dimensions
    if (terr->bg_surface->w != terr->width || 
        terr->bg_surface->h != terr->height) {
        Debug_Warn("Background dimensions %dx%d don't match expected %dx%d",
                  terr->bg_surface->w, terr->bg_surface->h,
                  terr->width, terr->height);
    }

    terr->dirty = 1;
    Debug_Info("Terrain loaded: bg=%s fg=%s", bg_path, fg_path);
    return SUCCESS;
}

void Terrain_UpdateTexture(Terrain *terr) {
    if (!terr->dirty) return;

    // Lock the texture for direct pixel access
    void *pixels;
    int pitch;
    SDL_LockTexture(terr->render_texture, NULL, &pixels, &pitch);

    // Get pointers to source surfaces
    Uint32 *bg_pixels = (Uint32*)terr->bg_surface->pixels;
    Uint32 *fg_pixels = (Uint32*)terr->fg_surface->pixels;
    Uint32 *target = (Uint32*)pixels;

    // Combine based on destruction mask
    int pixels_per_row = pitch / sizeof(Uint32);

    for (int y = 0; y < terr->height; y++) {
        for (int x = 0; x < terr->width; x++) {
            int idx = mask_index(terr->width, x, y);

            if (terr->destruction_mask[idx]) {
                // Destroyed: show background
                target[mask_index(pixels_per_row, x, y)] = bg_pixels[mask_index(terr->width, x, y)];
            } else {
                // Solid: show foreground
                target[mask_index(pixels_per_row, x, y)] = fg_pixels[mask_index(terr->width, x, y)];
            }
        }
    }

    // Unlock texture - changes are now in GPU memory
    SDL_UnlockTexture(terr->render_texture);

    terr->dirty = 0;
    Debug_Info("Texture updated");
}

void Terrain_Clean(Terrain *terr) {
    if (terr->background) SDL_DestroyTexture(terr->background);
    if (terr->foreground) SDL_DestroyTexture(terr->foreground);
    if (terr->render_texture) SDL_DestroyTexture(terr->render_texture);
    if (terr->bg_surface) SDL_FreeSurface(terr->bg_surface);
    if (terr->fg_surface) SDL_FreeSurface(terr->fg_surface);
    if (terr->destruction_mask) free(terr->destruction_mask);

    Debug_Info("Terrain destroyed");
}

void Terrain_Render(Terrain *terr, SDL_Renderer *renderer) {
    // Update texture if anything changed
    Terrain_UpdateTexture(terr);

    // Draw the combined texture
    SDL_RenderCopy(renderer, terr->render_texture, NULL, NULL);
}


// Optional: Save current terrain to PNG (for debugging)
void Terrain_DebugSave(Terrain *terr, char *filename) {
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0,
        terr->width, terr->height, 32, SDL_PIXELFORMAT_RGBA8888);

    void *pixels;
    int pitch;
    SDL_LockTexture(terr->render_texture, NULL, &pixels, &pitch);
    memcpy(surface->pixels, pixels, pitch * terr->height);
    SDL_UnlockTexture(terr->render_texture);

    IMG_SavePNG(surface, filename);
    SDL_FreeSurface(surface);
    Debug_Info("Saved terrain to %s", filename);
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
        terr->dirty = 1;
        Debug_Info("Destroyed %d pixels at (%d,%d) radius %d",
                  pixels_destroyed, cx, cy, radius);
    }
}

void Terrain_DestroyRect(Terrain *terr, float x, float y, float w, float h) {
    int min_x = MAX(0, x);
    int max_x = MIN(terr->width - 1, x + w);
    int min_y = MAX(0, y);
    int max_y = MIN(terr->height - 1, y + h);

    int pixels_destroyed = 0;

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            int idx = mask_index(terr->width, x, y);
            if (!terr->destruction_mask[idx]) {
                terr->destruction_mask[idx] = 1;
                pixels_destroyed++;
            }
        }
    }

    if (pixels_destroyed > 0) {
        terr->dirty = 1;
        Debug_Info("Destroyed %d pixels in rect (%d,%d %dx%d)",
                  pixels_destroyed, x, y, w, h);
    }
}

int Terrain_IsSolid(Terrain *terr, int x, int y) {
    if (!valid_coord(x, y, terr->width, terr->height)) return 0;
    return !terr->destruction_mask[mask_index(terr->width, x, y)];
}

int Terrain_CheckCollision(Terrain *terr, float x, float y, float w, float h, float *hit_x, float *hit_y) {
    int min_x = MAX(0, (int)x);
    int max_x = MIN(terr->width - 1, (int)(x + w));
    int min_y = MAX(0, (int)y);
    int max_y = MIN(terr->height - 1, (int)(y + h));

    for (int ty = min_y; ty <= max_y; ty++) {
        for (int tx = min_x; tx <= max_x; tx++) {
            if (Terrain_IsSolid(terr, tx, ty)) {
                *hit_x = tx;
                *hit_y = ty;
                return 1;  // Collision!
            }
        }
    }

    return 0;
}
