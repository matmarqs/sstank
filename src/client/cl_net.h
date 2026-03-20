#ifndef _CNET_H
#define _CNET_H

#include "../shared/struct/st_game.h"

// a CNet_Handler is pointer to a function f with type: void f(Game, data, len)
typedef int (*CNet_Handler)(Game *, void *, int);

void CNet_InitHandlers();
int CNet_RecvFromServer(Game *game, int timeout);
void CNet_SendInputToServer(Game *game);

#endif
