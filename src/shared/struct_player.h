#ifndef _STRUCT_PLAYER_H
#define _STRUCT_PLAYER_H

#include <SDL2/SDL.h>

#include "struct_input_mapper.h"

#define NUM_SPRITES 12

typedef struct {
    int id;

    float x, y;
    float vx, vy;

    int on_ground;

    int alive;

    int throwing;
    int power;

    float health;

    int damage_timer;

    int change_arm_timer;
    int curr_arm;

    float w, h;
    float w_render, h_render;

    int facing_right;
    int sprite_inverted;

    float angle;
    float angle_render;

    int curr_sprite;

    Input input;

    InputMapper input_mapper;

    int projectile_timer;

    char *sprites_path[NUM_SPRITES];
    SDL_Texture *sprites[NUM_SPRITES];
} Player;

#endif
