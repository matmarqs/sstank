#ifndef _CLIENT_NET_H
#define _CLIENT_NET_H

#include "../shared/struct_game.h"

// a ClientNet_Handler is pointer to a function f with type: void f(Game, data, len)
typedef int (*ClientNet_Handler)(Game *, void *, int);

void ClientNet_InitHandlers();
int ClientNet_RecvFromServer(Game *game, int timeout);
void ClientNet_SendInputToServer(Game *game);

#endif
