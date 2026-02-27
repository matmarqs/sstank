#include "player.h"
#include "debug.h"
#include "game.h"
#include "common.h"
#include "projectile.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_render.h>

int Player_Load(Player *p, SDL_Renderer *renderer) {
    for (int i = 0; i < NUM_SPRITES; i++) {
        p->sprites[i] = Renderer_LoadImage(renderer, p->sprites_path[i]);
        if (!p->sprites[i]) {
            Debug_Error("Player_Load error: error loading image %s\n", p->sprites_path[i]);
            return FAILURE;
        }
    }
    p->h = BASE_PLAYER_HEIGHT;
    p->w = BASE_PLAYER_HEIGHT * p->w_over_h;

    p->x = (WORLD_WIDTH - p->w) / 2.0;    /* start sprite center of screen */
    p->y = (WORLD_HEIGHT - p->h) / 2.0;
    p->vx = 0; p->vy = 0;
    p->angle = 0;
    p->angle_render = 0;
    p->curr_sprite = 1;
    p->facing_right = TRUE;
    p->projectile_timer = 0;

    return SUCCESS;
}

void Player_Update(Player *p, Game *game) {
    /* determine velocity */
    p->vx = 0;    /* 0 in cases where (left <- && right ->) */
    if ( p->input_mapper.move_left(&game->input) ) {
        p->vx = -SPEED;
        p->facing_right = FALSE;
    }
    if ( p->input_mapper.move_right(&game->input) ) {
        p->vx =  SPEED;
        p->facing_right = TRUE;
    }
    if ( p->input_mapper.increase_angle(&game->input) ) {
        p->angle += (3/360.0) * 2 * CONST_PI;
        if (p->angle > CONST_PI/2) {
            p->angle = CONST_PI/2;
        }
    }
    if ( p->input_mapper.decrease_angle(&game->input) ) {
        p->angle -= (3/360.0) * 2 * CONST_PI;
        if (p->angle < 0) {
            p->angle = 0;
        }
    }

    p->angle_render = p->facing_right ? p->angle : CONST_PI - p->angle;

    int center_x = p->x + p->w/2;
    int center_y = p->y + p->h/2;
    float dx = p->x - center_x;
    float dy = p->y - center_y;
    float radius = sqrt(dx*dx + dy*dy);
    float theta = p->angle_render;

    float start_x = center_x + (radius) * cos(theta);
    float start_y = center_y + (radius) * (-sin(theta));   // y axis is inverted

    if (--p->projectile_timer <= 0) p->projectile_timer = 0;

    if ( p->input_mapper.throw_projectile(&game->input) ) {
        if (p->projectile_timer == 0) {
            Projectile_Throw(&game->projectile_sys, start_x, start_y, p->angle_render, 150, p->id);
            p->projectile_timer = 20;
        }
        else {
            Debug_Info("Cannot throw projectile: projectile_timer = %d", p->projectile_timer);
        }
    }

    if (p->vx == 0) {
        p->curr_sprite = 1;
    }
    else {
        int t = game->time % 30;
        p->curr_sprite = (t < 10) ? 0 : (t < 20) ? 1 : 2;
    }

    /* update positions */
    p->x += p->vx / 60;

    /* collision detection with bounds */
    if (p->x <= 0) p->x = 0;
    if (p->x >= WORLD_WIDTH  - p->w) p->x = WORLD_WIDTH  - p->w;
}

void Player_Draw(Player *p, SDL_Renderer *renderer) {
    SDL_Rect rect = { p->x, p->y, p->w, p->h };
    SDL_RenderCopyEx(renderer, p->sprites[p->curr_sprite], NULL, &rect, 0, NULL, p->facing_right);

    int center_x = p->x + p->w/2;
    int center_y = p->y + p->h/2;
    float dx = p->x - center_x;
    float dy = p->y - center_y;
    float radius = sqrt(dx*dx + dy*dy);
    float norm = 50;
    float theta = p->angle_render;

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
