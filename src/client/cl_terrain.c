#include <SDL2/SDL_image.h>

#include "cl_types.h"
#include "../shared/core_terrain.h"

void ClientTerrain_Clean(cl_terrain_t *cl_terr) {
    if (cl_terr->render_texture) SDL_DestroyTexture(cl_terr->render_texture);
    if (cl_terr->bg_surface) SDL_FreeSurface(cl_terr->bg_surface);
    if (cl_terr->fg_surface) SDL_FreeSurface(cl_terr->fg_surface);
    Terrain_Clean(&cl_terr->terrain);
    Debug_Info("Terrain destroyed");
}

int ClientTerrain_Init(cl_terrain_t *cl_terr, SDL_Renderer *renderer) {
    Terrain_Init(&cl_terr->terrain);

    int width = cl_terr->terrain.width;
    int height = cl_terr->terrain.height;

    // Create render texture for streaming
    cl_terr->render_texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,  // KEY: Streaming access!
        width, height);

    if (!cl_terr->render_texture) {
        Debug_Error("Failed to create render texture: %s", SDL_GetError());
        ClientTerrain_Clean(cl_terr);
        return FAILURE;
    }

    return SUCCESS;
}

int ClientTerrain_Load(cl_terrain_t *cl_terr, SDL_Renderer *renderer, char *bg_path, char *fg_path) {
    Terrain_Load(&cl_terr->terrain, fg_path);

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
    cl_terr->bg_surface = SDL_ConvertSurfaceFormat(bg_surface, SDL_PIXELFORMAT_RGBA8888, 0);
    cl_terr->fg_surface = SDL_ConvertSurfaceFormat(fg_surface, SDL_PIXELFORMAT_RGBA8888, 0);

    SDL_FreeSurface(bg_surface);
    SDL_FreeSurface(fg_surface);

    return SUCCESS;
}

void ClientTerrain_UpdateTexture(cl_terrain_t *cl_terr) {
    if (!cl_terr->dirty) return;

    // Lock the texture for direct pixel access
    void *pixels;
    int pitch;
    SDL_LockTexture(cl_terr->render_texture, NULL, &pixels, &pitch);

    // Get pointers to source surfaces
    Uint32 *bg_pixels = (Uint32*)cl_terr->bg_surface->pixels;
    Uint32 *fg_pixels = (Uint32*)cl_terr->fg_surface->pixels;
    Uint32 *target = (Uint32*)pixels;

    // Combine based on destruction mask
    int pixels_per_row = pitch / sizeof(Uint32);

    Terrain *terr = &cl_terr->terrain;

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
    SDL_UnlockTexture(cl_terr->render_texture);

    cl_terr->dirty = 0;
    Debug_Info("Texture updated");
}

void ClientTerrain_Render(cl_terrain_t *cl_terr, SDL_Renderer *renderer) {
    // Update texture if anything changed
    ClientTerrain_UpdateTexture(cl_terr);
    // Draw the combined texture
    SDL_RenderCopy(renderer, cl_terr->render_texture, NULL, NULL);
}
