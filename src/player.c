#include "player.h"
#include "debug.h"
#include "game.h"
#include "common.h"
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

    p->x = (WIN_WIDTH - p->w) / 2.0;    /* start sprite center of screen */
    p->y = (WIN_HEIGHT - p->h) / 2.0;
    p->vx = 0;
    p->angle = 0;
    p->curr_sprite = 1;
    p->facing_right = TRUE;

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
        p->angle -= (2/360.0) * 2 * CONST_PI;
    }
    if ( p->input_mapper.decrease_angle(&game->input) ) {
        p->angle += (2/360.0) * 2 * CONST_PI;
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
    if (p->x >= WIN_WIDTH  - p->w) p->x = WIN_WIDTH  - p->w;
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

    float start_x = center_x + (1.0 * radius) * cos(p->angle);
    float start_y = center_y + (1.0 * radius) * sin(p->angle);
    float end_x = center_x + (1.0 * radius + norm) * cos(p->angle);
    float end_y = center_y + (1.0 * radius + norm) * sin(p->angle);

    Debug_Info("x = %f, y = %f, start_x = %f, start_y = %f\n", p->x, p->y, start_x, start_y);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawLine(renderer, start_x, start_y, end_x, end_y);
}

void Player_Clean(Player *p) {
    for (int i = 0; i < NUM_SPRITES; i++) {
        if (p->sprites[i])
            SDL_DestroyTexture(p->sprites[i]);
    }
}
