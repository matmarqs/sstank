#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>

SDL_Texture *Renderer_LoadImage(SDL_Renderer *renderer, char *img_path);

#endif
