#ifndef INPUT_MAPPER_H
#define INPUT_MAPPER_H

#include "input.h"

typedef struct {
    int (*move_left)(Input *input);
    int (*move_right)(Input *input);
} InputMapper;

#endif
