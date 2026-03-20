#ifndef _LOGIC_PLAYER_H
#define _LOGIC_PLAYER_H

#include <SDL2/SDL.h>
#include "struct_player.h"
#include "struct_game.h"

#define SPEED 300   /* speed in pixels/sec */
#define BASE_PLAYER_HEIGHT 60
#define BASE_PLAYER_WIDTH BASE_PLAYER_HEIGHT

#define MAX_POWER 500

int Player_Load(Player *p, SDL_Renderer *renderer);
void Player_Update(Player *p, GameState *game);

void Player_RenderPowerGauge(Player *p, SDL_Renderer *renderer);
void Player_Render(Player *p, SDL_Renderer *renderer);

void Player_Teleport(Player *p, float x, float y);

void Player_Clean(Player *p);

void Player_ShootingHandler(Player *p, GameState *game);
void Player_MovementHandler(Player *p, GameState *game);
void Player_AnimationHandler(Player *p, GameState *game);

#endif
