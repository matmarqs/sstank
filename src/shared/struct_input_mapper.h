#ifndef _STRUCT_INPUT_MAPPER_H
#define _STRUCT_INPUT_MAPPER_H

#include "struct_input.h"

typedef struct {
    int (*move_left)(Input *input);
    int (*move_right)(Input *input);
    int (*increase_angle)(Input *input);
    int (*decrease_angle)(Input *input);
    int (*throw_projectile)(Input *input);
    int (*cycle_arm)(Input *input);
} InputMapper;

#endif
