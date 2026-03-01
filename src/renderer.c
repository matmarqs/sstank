#include "debug.h"
#include "renderer.h"

SDL_Texture *Renderer_LoadImage(SDL_Renderer *renderer, char *img_path) {
    SDL_Texture *texture = IMG_LoadTexture(renderer, img_path);
    if (!texture) {
        Debug_Error("LoadImage error: cannot find %s", img_path);
        return NULL;
    }
    return texture;
}
