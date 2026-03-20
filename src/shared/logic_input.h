#ifndef _LOGIC_INPUT_H
#define _LOGIC_INPUT_H

#include <SDL2/SDL.h>
#include "struct_input.h"

int Input_SetEvents(SDL_Event *ev_ptr, Input *input);
void Input_InitKeys(Input *input);

#endif
