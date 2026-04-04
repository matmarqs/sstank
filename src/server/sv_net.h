#ifndef _SERVER_NET_H
#define _SERVER_NET_H

#include "sv_types.h"

void sv_net_Init(sv_server_t *server);
void sv_net_AcceptClients(sv_server_t *server);
void sv_net_Broadcast(sv_server_t *server, uint8_t packet_id, void *data, int len);

#endif
