#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"
#include <SDL2/SDL.h>
#include "renderer.h"
#include "input_mapper.h"

#define SPEED      (300)   /* speed in pixels/sec */
#define BASE_PLAYER_HEIGHT  (60)

#define NUM_SPRITES (3)

typedef struct {
    int id;

    float x, y;
    float vx, vy;

    float w, h;
    float w_over_h;
    int facing_right;

    float angle;
    float angle_render;

    int curr_sprite;

    InputMapper input_mapper;

    int projectile_timer;

    char *sprites_path[3];
    SDL_Texture *sprites[NUM_SPRITES];
} Player;

int Player_Load(Player *p, SDL_Renderer *renderer);
void Player_Update(Player *p, Game *game);
void Player_Draw(Player *p, SDL_Renderer *renderer);
void Player_Clean(Player *p);

#endif
