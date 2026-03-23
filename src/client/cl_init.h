#ifndef _CLIENT_INIT_H
#define _CLIENT_INIT_H

#include "cl_types.h"

void ClientInit_Player0(cl_player_t *p);
void ClientInit_Player1(cl_player_t *p);
void ClientInit_Players(cl_player_t p[]);

void ClientInit_Rendering(cl_state_t *client);

#endif
