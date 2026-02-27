#include "camera.h"
#include "common.h"

// THIS CODE IS BUGGED. IT WAS SUPPOSED TO MAKE THE GAME WORK AT FULL SCREEN OR WHATEVER RESOLUTION

void Camera_Init(Camera *cam, int screen_w, int screen_h) {
    // Calculate scale to fit WORLD into screen while keeping aspect ratio
    float scale_x = screen_w / (float)WORLD_WIDTH;
    float scale_y = screen_h / (float)WORLD_HEIGHT;

    // Use the smaller scale to fit entire world
    cam->scale = (scale_x < scale_y) ? scale_x : scale_y;

    // Calculate viewport to center the world
    cam->viewport.w = WORLD_WIDTH * cam->scale;
    cam->viewport.h = WORLD_HEIGHT * cam->scale;
    cam->viewport.x = (screen_w - cam->viewport.w) / 2;
    cam->viewport.y = (screen_h - cam->viewport.h) / 2;

    cam->offset_x = 0;  // No panning initially
    cam->offset_y = 0;
}

// Convert screen coordinates (mouse clicks) to world coordinates
void Camera_ScreenToWorld(Camera *cam, int screen_x, int screen_y, 
                   float *world_x, float *world_y) {
    *world_x = (screen_x - cam->viewport.x) / cam->scale;
    *world_y = (screen_y - cam->viewport.y) / cam->scale;
}

// Convert world coordinates to screen coordinates (for rendering)
void Camera_WorldToScreen(Camera *cam, float world_x, float world_y,
                   int *screen_x, int *screen_y) {
    *screen_x = cam->viewport.x + world_x * cam->scale;
    *screen_y = cam->viewport.y + world_y * cam->scale;
}

void Camera_ToggleFullscreen(Camera *camera, SDL_Window *window) {
    Uint32 flags = SDL_GetWindowFlags(window);

    if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
        // Switch to windowed
        SDL_SetWindowFullscreen(window, 0);
        SDL_SetWindowSize(window, 1200, 900);
        SDL_SetWindowPosition(window, 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    } else {
        // Switch to fullscreen desktop (uses current monitor resolution)
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }

    // Get new window size and update camera
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    Camera_Init(camera, w, h);
}
