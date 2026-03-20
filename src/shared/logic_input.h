#ifndef _INPUT_H
#define _INPUT_H

#include <SDL2/SDL.h>
#include "../struct/st_input.h"

int Input_SetEvents(SDL_Event *ev_ptr, Input *input);
void Input_InitKeys(Input *input);

#endif
