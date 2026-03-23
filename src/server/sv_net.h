#ifndef _SERVER_NET_H
#define _SERVER_NET_H

#include "sv_types.h"

void Server_InitNet(Server *server);
void Server_AcceptClients(Server *server);
void Server_Broadcast(Server *server, uint8_t packet_id, void *data, int len);

#endif
