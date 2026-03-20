#ifndef _STRUCT_PLAYER_H
#define _STRUCT_PLAYER_H

typedef struct {
    int id;

    float x, y;
    float vx, vy;

    int on_ground;

    int alive;

    int throw_cooldown;
    int power;

    float health;

    float w, h;

    int facing_right;
} PlayerState;

#endif
