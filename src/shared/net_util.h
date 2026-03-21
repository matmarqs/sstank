#ifndef _NET_UTIL_H
#define _NET_UTIL_H

#include <stdint.h>

void NetUtil_IPint32ToChar(uint32_t ip, char ip_address[16]);
void NetUtil_ShowBytes(uint8_t *ptr, uint32_t size);
void NetUtil_ShowBytesUntilNull(uint8_t *ptr, uint32_t size);

#endif
