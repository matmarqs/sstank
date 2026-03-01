#include "player.h"
#include "debug.h"
#include "game.h"
#include "movement.h"
#include "projectile.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>

int Player_Load(Player *p, SDL_Renderer *renderer) {
    for (int i = 0; i < NUM_SPRITES; i++) {
        p->sprites[i] = IMG_LoadTexture(renderer, p->sprites_path[i]);
        if (!p->sprites[i]) {
            Debug_Error("Player_Load error: error loading image %s\n", p->sprites_path[i]);
            return FAILURE;
        }
    }
    int w, h;
    SDL_QueryTexture(p->sprites[0], NULL, NULL, &w, &h);
    p->w_over_h = w / (1.0 * h);

    p->h_render = BASE_PLAYER_HEIGHT;
    p->w_render = BASE_PLAYER_HEIGHT * p->w_over_h;

    float hitbox_factor = 2.0 / 3;
    p->w = p->w_render * hitbox_factor;
    p->h = p->h_render;

    p->x = (WORLD_WIDTH - p->w) / 2.0 - 50;    /* start sprite center of screen */
    p->y = (WORLD_HEIGHT - p->h) / 2.0 - 100;
    p->vx = 0; p->vy = 0;
    p->angle = 0;
    p->angle_render = 0;
    p->curr_sprite = 1;
    p->facing_right = TRUE;
    p->projectile_timer = 0;
    p->on_ground = 0;

    return SUCCESS;
}

void Player_Update(Player *p, Game *game) {
    Player_ShootingHandler(p, game);
    Player_MovementHandler(p, game);
    Player_AnimationHandler(p, game);
}

void Player_ShootingHandler(Player *p, Game *game) {
    // 1. Handle aiming (doesn't affect movement)
    if (p->input_mapper.increase_angle(&game->input)) {
        p->angle += (3/360.0) * 2 * CONST_PI;
        if (p->angle > CONST_PI/2) p->angle = CONST_PI/2;
    }
    if (p->input_mapper.decrease_angle(&game->input)) {
        p->angle -= (3/360.0) * 2 * CONST_PI;
        if (p->angle < 0) p->angle = 0;
    }
    p->angle_render = p->facing_right ? p->angle : CONST_PI - p->angle;

    // 2. Handle throwing
    p->projectile_timer--;
    if (p->projectile_timer <= 0)
        p->projectile_timer = 0;
    if (p->input_mapper.throw_projectile(&game->input) && p->projectile_timer == 0) {
        int center_x = p->x + p->w/2;
        int center_y = p->y + p->h/2;
        float dx = p->x - center_x;
        float dy = p->y - center_y;
        float radius = sqrt(dx*dx + dy*dy);
        float start_x = center_x + radius * cos(p->angle_render);
        float start_y = center_y + radius * (-sin(p->angle_render));

        Projectile_Throw(&game->projectile_sys, start_x, start_y, p->angle_render, 150, p->id);
        p->projectile_timer = 20;
    }
}

void Player_MovementHandler(Player *p, Game *game) {
    // 1. INPUT HANDLING (always runs, doesn't affect state)
    float input_vx = 0;
    if (p->input_mapper.move_left(&game->input)) {
        input_vx = -SPEED;
        p->facing_right = FALSE;
    }
    if (p->input_mapper.move_right(&game->input)) {
        input_vx = SPEED;
        p->facing_right = TRUE;
    }

    // Determine state and update
    PlayerMoveState state = Movement_DeterminePlayerState(&game->terrain, p);

    switch (state) {
        case GROUNDED:
            Movement_UpdateGrounded(&game->terrain, p, input_vx);
            break;
        case FALLING:
            Movement_UpdateFalling(&game->terrain, p, input_vx);
            break;
        default:
            break;
    }

    // World bounds
    if (p->x < 0) p->x = 0;
    if (p->x > WORLD_WIDTH - p->w) p->x = WORLD_WIDTH - p->w;
    if (p->y < 0) p->y = 0;
    if (p->y > WORLD_HEIGHT - p->h) p->y = WORLD_HEIGHT - p->h;
}

void Player_AnimationHandler(Player *p, Game *game) {
    // Animation
    if (p->vx == 0) {
        p->curr_sprite = 1;
    } else {
        int t = game->time % 30;
        p->curr_sprite = (t < 10) ? 0 : (t < 20) ? 1 : 2;
    }
}

void Player_Render(Player *p, SDL_Renderer *renderer) {
    float x = p->x, y = p->y, w = p->w_render, h = p->h_render, theta = p->angle_render;

    SDL_Rect rect = { x, y, w, h };
    SDL_RenderCopyEx(renderer, p->sprites[p->curr_sprite], NULL, &rect, 0, NULL, p->facing_right);

    int center_x = x + w/2;
    int center_y = y + h/2;
    float dx = x - center_x;
    float dy = y - center_y;
    float radius = sqrt(dx*dx + dy*dy);
    float norm = 50;

    float start_x = center_x + (radius) * cos(theta);
    float start_y = center_y + (radius) * (-sin(theta));   // y axis is inverted
    float end_x = center_x + (radius + norm) * cos(theta);
    float end_y = center_y + (radius + norm) * (-sin(theta));   // y axis is inverted

    circleRGBA(renderer, center_x, center_y, radius, 0, 0, 0, 255);

    //Debug_Info("theta = %f\n", theta * 360 / (2*CONST_PI));

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawLine(renderer, start_x, start_y, end_x, end_y);
}

void Player_Clean(Player *p) {
    for (int i = 0; i < NUM_SPRITES; i++) {
        if (p->sprites[i])
            SDL_DestroyTexture(p->sprites[i]);
    }
}
