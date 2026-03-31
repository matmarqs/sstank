#ifndef _SERVER_CMD_H
#define _SERVER_CMD_H

#include "sv_types.h"

int sv_cmd_PlayerShoot(Server *server, PlayerState *p, ProjectileSystem *ps,
                       int type, float angle, float power);
void sv_cmd_PlayerTakeDamage(Server *server, int player_id, float health);
void sv_cmd_PlayerBroadcastPositions(Server *server);
void sv_cmd_TerrainDestroy(Server *server, float cx, float cy, float r);

#endif
