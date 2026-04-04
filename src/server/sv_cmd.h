#ifndef _SERVER_CMD_H
#define _SERVER_CMD_H

#include "sv_types.h"

int sv_cmd_PlayerShoot(sv_server_t *server, PlayerState *p, ProjectileSystem *ps,
                       int type, float angle, float power);
void sv_cmd_PlayerTakeDamage(sv_server_t *server, int player_id, float health);
void sv_cmd_PlayerBroadcastPositions(sv_server_t *server);
void sv_cmd_TerrainDestroy(sv_server_t *server, float cx, float cy, float r);

#endif
