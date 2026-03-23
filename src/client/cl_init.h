#ifndef _CLIENT_MANAGER_H
#define _CLIENT_MANAGER_H

#include "cl_types.h"

/* helpers */
void cl_init_SpritesPlayer0(cl_player_t *p);
void cl_init_SpritesPlayer1(cl_player_t *p);
void cl_init_SpritesPlayers(cl_player_t p[]);

/* main functions */
void cl_init_Net(cl_state_t *client, char *ip_addr);
void cl_init_Rendering(cl_state_t *client);
void cl_init_Terrain(cl_state_t *client);
void cl_init_Players(cl_state_t *client);
void cl_init_Projectiles(cl_state_t *client);
void cl_init_Clean(cl_state_t *client, int exit_code);

#endif
