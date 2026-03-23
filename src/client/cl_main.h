#ifndef _CLIENT_MAIN_H
#define _CLIENT_MAIN_H

#include "cl_types.h"

void cl__Init(cl_state_t *client, GameState *game, char *ip_addr);
int cl__Update(cl_state_t *client);
void cl__Render(cl_state_t *client);
void cl__Clean(cl_state_t *client, int exit_code);

#endif
