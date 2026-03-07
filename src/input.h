#ifndef INPUT_H
#define INPUT_H

#include "common.h"
#include <SDL2/SDL_events.h>

typedef struct {
    int w, a, s, d, space, l_shift;
    int up, left, down, right, enter, r_ctrl;
} Input;

int Input_SetEvents(SDL_Event *ev_ptr, Input *input);
void Input_InitKeys(Input *input);

#endif
