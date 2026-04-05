#include <SDL2/SDL_image.h> // IMG_LoadTexture

#include "cl_player.h"
#include "../shared/core_player.h"

static int loop_integer(int i, int n) {
    int period = 2 * (n - 1);  // For n=6, period = 10
    int t = i % period;
    if (t < n) {
        return t;           // 0,1,2,3,4,5
    } else {
        return period - t;  // 4,3,2,1
    }
}

int cl_player_Init(cl_player_t *cl_player, PlayerState *p, SDL_Renderer *renderer) {
    cl_player->state = p;
    for (int i = 0; i < PLAYER_NUM_SPRITES; i++) {
        if (cl_player->sprites_path[i]) {
            cl_player->sprites[i] = IMG_LoadTexture(renderer, cl_player->sprites_path[i]);
            if (!cl_player->sprites[i]) {
                Debug_Error("cl_player_Load error: loading image %s\n", cl_player->sprites_path[i]);
                return FAILURE;
            }
        }
    }
    int w, h;
    SDL_QueryTexture(cl_player->sprites[0], NULL, NULL, &w, &h);
    cl_player->w_render = BASE_PLAYER_WIDTH;
    cl_player->h_render = BASE_PLAYER_HEIGHT * ((float) h) / w;
    cl_player->curr_sprite = 0;
    cl_player->damage_timer = 0;
    return SUCCESS;
}

void cl_player_TakeDamage(cl_player_t *cl_player, float new_health) {
    cl_player->state->health = new_health;
    if (new_health <= 0) {
        cl_player->state->alive = FALSE;
    }
    cl_player->damage_timer = 60;
}

void cl_player_AnimationHandler(cl_player_t *cl_player, GameState *game) {
    // Animation
    int t = game->time;
    if (cl_player->state->vx == 0) {
        cl_player->curr_sprite = loop_integer(t / 8, 5);
    } else {
        cl_player->curr_sprite = 5 + loop_integer(t / 5, 7);
    }
}

void cl_player_Update(cl_player_t *cl_player, GameState *game) {
    cl_player->damage_timer--;
    cl_player->damage_timer = MAX(cl_player->damage_timer, 0);
    if (cl_player->state->health <= 0) {
        cl_player->state->alive = 0;
    }
    if (cl_player->state->alive) {
        //Player_MovementHandler(cl_player->state, game, actions, dt);
        cl_player_AnimationHandler(cl_player, game);
    }
}

void cl_player_Render(cl_player_t *cl_p, SDL_Renderer *renderer) {
    PlayerState *p = cl_p->state;
    if (p->alive) {
        if (cl_p->damage_timer > 0) {
            // Make sprite reddish
            SDL_SetTextureColorMod(cl_p->sprites[cl_p->curr_sprite], 255, 100, 100);
            cl_p->damage_timer--;
        } else {
            // Normal color
            SDL_SetTextureColorMod(cl_p->sprites[cl_p->curr_sprite], 255, 255, 255);
        }

        float x = p->x, y = p->y, w = cl_p->w_render, h = cl_p->h_render;

        SDL_Rect rect = { x, y, w, h };
        SDL_RenderCopyEx(renderer, cl_p->sprites[cl_p->curr_sprite],
                         NULL, &rect, 0, NULL, cl_p->sprite_inverted ? !p->facing_right : p->facing_right);

        // Reset color mod for other players (important!)
        SDL_SetTextureColorMod(cl_p->sprites[cl_p->curr_sprite], 255, 255, 255);
    }
}

void cl_player_Clean(cl_player_t *cl_player) {
    for (int i = 0; i < PLAYER_NUM_SPRITES; i++) {
        if (cl_player->sprites[i])
            SDL_DestroyTexture(cl_player->sprites[i]);
    }
}
