#include "net_util.h"
#include <stdio.h>

void NetUtil_IPint32ToChar(uint32_t ip, char ip_address[16]) {
    snprintf(ip_address, 16, "%d.%d.%d.%d",
             ((uint8_t *) &ip)[0], ((uint8_t *) &ip)[1], ((uint8_t *) &ip)[2], ((uint8_t *) &ip)[3]);
    ip_address[16] = '\0';
}

void NetUtil_ShowBytes(uint8_t *ptr, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        printf("%.2x ", ptr[i]); 
    }
    printf("\n");
}

void NetUtil_ShowBytesUntilNull(uint8_t *ptr, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        printf("%.2x ", ptr[i]); 
        if (ptr[i] == '\0')
            break;
    }
    printf("\n");
}
