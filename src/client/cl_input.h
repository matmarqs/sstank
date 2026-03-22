#ifndef _CLIENT_INPUT_H
#define _CLIENT_INPUT_H

#include "cl_types.h"

void Input_InitKeys(Input *input);
int Input_SetEvents(SDL_Event *ev, Input *input);
PlayerActions Input_GetActions(Input *raw);

#endif
