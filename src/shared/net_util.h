#ifndef _NET_UTILS_H
#define _NET_UTILS_H

#include <stdint.h>

typedef unsigned char byte;

void NetUtil_IPint32ToChar(uint32_t ip, char ip_address[16]);
void NetUtil_ShowBytes(byte *ptr, uint32_t size);
void NetUtil_ShowBytesUntilNull(byte *ptr, uint32_t size);

#endif
