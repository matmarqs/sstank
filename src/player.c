#include "player.h"
#include "debug.h"
#include "game.h"

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
}

void Player_Clean(Player *p) {
    for (int i = 0; i < NUM_SPRITES; i++) {
        if (p->sprites[i])
            SDL_DestroyTexture(p->sprites[i]);
    }
}
