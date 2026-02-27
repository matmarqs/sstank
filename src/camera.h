#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2/SDL.h>

typedef struct {
    // World-to-screen transformation
    float scale;           // Zoom factor
    SDL_Rect viewport;     // Where on screen to draw the world
    int offset_x, offset_y; // Panning (if you want scrolling)
} Camera;

void Camera_Init(Camera *cam, int screen_w, int screen_h);
void Camera_ScreenToWorld(Camera *cam, int screen_x, int screen_y, float *world_x, float *world_y);
void Camera_WorldToScreen(Camera *cam, float world_x, float world_y, int *screen_w, int *screen_h);
void Camera_ToggleFullscreen(Camera *cam, SDL_Window *window);

#endif
