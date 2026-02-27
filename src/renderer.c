#include "debug.h"
#include "renderer.h"

SDL_Texture *Renderer_LoadImage(SDL_Renderer *renderer, char *img_path) {
    SDL_Surface *surface = IMG_Load(img_path);
    if (!surface) {
        Debug_Error("LoadImage error: cannot find %s", img_path);
        SDL_Quit();
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}
