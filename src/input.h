#ifndef INPUT_H
#define INPUT_H

#include "common.h"
#include <SDL2/SDL_events.h>
#include "camera.h"

#define KEY_UP    (SDL_SCANCODE_UP)
#define KEY_LEFT  (SDL_SCANCODE_LEFT)
#define KEY_DOWN  (SDL_SCANCODE_DOWN)
#define KEY_RIGHT (SDL_SCANCODE_RIGHT)
#define KEY_ENTER (SDL_SCANCODE_KP_ENTER)

#define KEY_W (SDL_SCANCODE_W)
#define KEY_A (SDL_SCANCODE_A)
#define KEY_S (SDL_SCANCODE_S)
#define KEY_D (SDL_SCANCODE_D)
#define KEY_SPACE (SDL_SCANCODE_SPACE)

typedef struct {
    int w, a, s, d, space;
    int up, left, down, right, enter;
} Input;

int Input_SetEvents(SDL_Event *ev_ptr, Input *input, Camera *camera, SDL_Window *window);
void Input_InitKeys(Input *input);

#endif
