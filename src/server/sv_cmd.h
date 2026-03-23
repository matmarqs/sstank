#ifndef _SERVER_CMD_H
#define _SERVER_CMD_H

#include "sv_types.h"

int sv_cmd_PlayerShoot(Server *server, PlayerState *p, ProjectileSystem *ps, int type, float angle, float power);

#endif
