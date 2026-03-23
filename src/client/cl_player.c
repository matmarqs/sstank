#include <SDL2/SDL_image.h> // IMG_LoadTexture

#include "cl_player.h"
#include "../shared/core_player.h"
#include "cl_input.h"

static int loop_integer(int i, int n) {
    int period = 2 * (n - 1);  // For n=6, period = 10
    int t = i % period;
    if (t < n) {
        return t;           // 0,1,2,3,4,5
    } else {
        return period - t;  // 4,3,2,1
    }
}

int ClientPlayer_Load(cl_player_t *cl_player, SDL_Renderer *renderer) {
    for (int i = 0; i < PLAYER_NUM_SPRITES; i++) {
        if (cl_player->sprites_path[i]) {
            cl_player->sprites[i] = IMG_LoadTexture(renderer, cl_player->sprites_path[i]);
            if (!cl_player->sprites[i]) {
                Debug_Error("ClientPlayer_Load error: loading image %s\n", cl_player->sprites_path[i]);
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

void ClientChar_Init(cl_state_t *client) {
    cl_char_t *cl_char = &client->cl_char;
    cl_char->id = client->my_player_id;
    cl_char->player = &client->cl_players[client->my_player_id];
    Input_InitKeys(&cl_char->input);
    cl_char->angle = 0;
    cl_char->angle_render = 0;
    cl_char->throwing = 0;
    cl_char->power = 0;
    cl_char->change_arm_timer = 0;
    cl_char->curr_arm = 0;
    cl_char->projectile_timer = 0;
}

void ClientPlayer_TakeDamage(cl_player_t *cl_player, float new_health) {
    cl_player->state->health = new_health;
    if (new_health <= 0) {
        cl_player->state->alive = FALSE;
    }
    cl_player->damage_timer = 60;
}

void ClientPlayer_AnimationHandler(cl_player_t *p, GameState *game) {
    // Animation
    int t = game->time;
    if (p->state->vx == 0) {
        p->curr_sprite = loop_integer(t / 8, 5);
    } else {
        p->curr_sprite = 5 + loop_integer(t / 5, 7);
    }
}

void ClientPlayer_Update(cl_player_t *p, GameState *game, PlayerActions actions, float dt) {
    p->damage_timer--;
    p->damage_timer = MAX(p->damage_timer, 0);
    if (p->state->health <= 0) {
        p->state->alive = 0;
    }
    if (p->state->alive) {
        Player_MovementHandler(p->state, game, actions, dt);
        ClientPlayer_AnimationHandler(p, game);
    }
}

void ClientPlayer_Render(cl_player_t *cl_p, SDL_Renderer *renderer) {
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

void ClientPlayer_Clean(cl_player_t *p) {
    for (int i = 0; i < PLAYER_NUM_SPRITES; i++) {
        if (p->sprites[i])
            SDL_DestroyTexture(p->sprites[i]);
    }
}
