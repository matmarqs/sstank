#ifndef _CLIENT_NET_H
#define _CLIENT_NET_H

#include "cl_state_t.h"

// a ClientNet_Handler is pointer to a function f with type: void f(ClientState, data, len)
typedef int (*ClientNet_Handler)(ClientState *, void *, int);

void ClientNet_InitHandlers();
int ClientNet_RecvFromServer(ClientState *game, int timeout);
void ClientNet_SendInputToServer(ClientState *game);

#endif
