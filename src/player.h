#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"
#include <SDL2/SDL.h>
#include "renderer.h"
#include "input_mapper.h"

#define SPEED      (300)   /* speed in pixels/sec */
#define BASE_PLAYER_HEIGHT  (100)

#define NUM_SPRITES (3)

typedef struct {
    float x, y;
    float vx;

    float w, h;
    float w_over_h;
    int facing_right;

    int curr_sprite;

    InputMapper input_mapper;

    char *sprites_path[3];
    SDL_Texture *sprites[NUM_SPRITES];
} Player;

int Player_Load(Player *p, SDL_Renderer *renderer);
void Player_Update(Player *p, Game *game);
void Player_Draw(Player *p, SDL_Renderer *renderer);
void Player_Clean(Player *p);

#endif
