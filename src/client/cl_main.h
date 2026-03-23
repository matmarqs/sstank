#ifndef _CLIENT_MAIN_H
#define _CLIENT_MAIN_H

#include "cl_types.h"

void Client_Init(cl_state_t *client, char *ip_addr);
int Client_Update(cl_state_t *client);
void Client_Render(cl_state_t *client);
void Client_Clean(cl_state_t *client, int exit_code);

#endif
