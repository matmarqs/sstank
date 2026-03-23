#ifndef _CLIENT_NET_H
#define _CLIENT_NET_H

#include "cl_types.h"

// a cl_net_Handler is pointer to a function f with type: void f(cl_state_t, data, len)
typedef int (*cl_net_Handler)(cl_state_t *, void *, int);

void cl_net_InitHandlers();
int cl_net_RecvFromServer(cl_state_t *game, int timeout);
void cl_net_SendInputToServer(cl_state_t *game);
void cl_net_InitSockets(cl_state_t *client, char *ip_addr);
void cl_net_SendActions(cl_state_t *client, PlayerActions actions);
void cl_net_SendThrow(TCPsocket server, float angle, float power, int type);

#endif
