#ifndef _CLIENT_INPUT_H
#define _CLIENT_INPUT_H

#include "cl_types.h"

void cl_input_InitKeys(cl_input_t *input);
int cl_input_GetEvents(SDL_Event *ev, cl_input_t *input);
PlayerActions cl_input_GetActions(cl_input_t *raw);

#endif
