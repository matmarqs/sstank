#ifndef _CLIENT_NET_H
#define _CLIENT_NET_H

#include "cl_types.h"

// a ClientNet_Handler is pointer to a function f with type: void f(cl_state_t, data, len)
typedef int (*ClientNet_Handler)(cl_state_t *, void *, int);

void ClientNet_InitHandlers();
int ClientNet_RecvFromServer(cl_state_t *game, int timeout);
void ClientNet_SendInputToServer(cl_state_t *game);
void ClientNet_InitSockets(cl_state_t *client, char *ip_addr);
void ClientNet_SendActions(cl_state_t *client, PlayerActions actions);
void ClientNet_SendThrow(cl_state_t *client, float angle, float power, int type);

#endif
