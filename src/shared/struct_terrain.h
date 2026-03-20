#ifndef _STRUCT_TERRAIN_H
#define _STRUCT_TERRAIN_H

#include <stdint.h>

typedef struct {
    // Destruction mask (1 = destroyed/hole, 0 = solid)
    uint8_t *destruction_mask;
    int width, height;
} Terrain;

#endif
