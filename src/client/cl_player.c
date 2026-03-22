#include <SDL2/SDL_image.h> // IMG_LoadTexture
#include <SDL2/SDL2_gfxPrimitives.h> // boxRGBA

#include "cl_player.h"
#include "../shared/core_player.h"
#include "../shared/core_projectile.h"

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
    Player_Init(&cl_player->state);

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

void ClientPlayer_TakeDamage(cl_player_t *cl_player, float new_health) {
    cl_player->state.health = new_health;
    if (new_health <= 0) {
        cl_player->state.alive = FALSE;
    }
    cl_player->damage_timer = 60;
}

void ClientPlayer_AnimationHandler(cl_player_t *p, GameState *game) {
    // Animation
    int t = game->time;
    if (p->state.vx == 0) {
        p->curr_sprite = loop_integer(t / 8, 5);
    } else {
        p->curr_sprite = 5 + loop_integer(t / 5, 7);
    }
}

void ClientPlayer_Update(cl_player_t *p, GameState *game, PlayerActions actions, float dt) {
    p->damage_timer--;
    p->damage_timer = MAX(p->damage_timer, 0);
    if (p->state.health <= 0) {
        p->state.alive = 0;
    }
    if (p->state.alive) {
        Player_MovementHandler(&p->state, game, actions, dt);
        ClientPlayer_AnimationHandler(p, game);
    }
}

void ClientChar_Update(cl_char_t *cl_char, GameState *game) {
    ClientPlayer_ShootingHandler(cl_char, game);
}

void ClientPlayer_RenderPowerGauge(cl_char_t *cl_char, SDL_Renderer *renderer) {
    PlayerState *p = &cl_char->player.state;
    int gauge_x = p->x - p->w/2;
    int gauge_y = p->y - p->w/2;  // Above the visor

    int gauge_width = 100;
    int gauge_height = 15;
    int border_size = 2;

    // Draw background (empty gauge)
    boxRGBA(renderer, 
            gauge_x, gauge_y, 
            gauge_x + gauge_width, gauge_y + gauge_height,
            50, 50, 50, 255);  // Dark gray

    // Draw border
    rectangleRGBA(renderer, 
                  gauge_x, gauge_y, 
                  gauge_x + gauge_width, gauge_y + gauge_height,
                  200, 200, 200, 255);

    // Draw fill based on power
    if (cl_char->power > 0) {
        float normalized_power = (float)cl_char->power / MAX_POWER;  // 0.0 to 1.0
        int fill_width = normalized_power * gauge_width;       // 0 to gauge_width

        // Color based on power level
        Uint8 r, g, b;
        float power_percent = normalized_power * 100;  // 0-100%

        if (power_percent < 30) {
            r = 0; g = 255; b = 0;    // Green
        } else if (power_percent < 70) {
            r = 255; g = 255; b = 0;  // Yellow
        } else {
            r = 255; g = 0; b = 0;    // Red
        }

        // Draw the filled portion (with border inset)
        boxRGBA(renderer, 
                gauge_x + border_size, 
                gauge_y + border_size,
                gauge_x + fill_width - border_size, 
                gauge_y + gauge_height - border_size,
                r, g, b, 255);
    }

    // If charging, show pulsing effect
    if (cl_char->throwing) {
        int pulse = (SDL_GetTicks() / 100) % 2;
        if (pulse) {
            // White overlay
            boxRGBA(renderer,
                   gauge_x, gauge_y,
                   gauge_x + gauge_width, gauge_y + gauge_height,
                   255, 255, 255, 50);
        }
    }
}

void ClientPlayer_RenderAngle(cl_char_t *cl_char, SDL_Renderer *renderer) {
    cl_player_t *cl_p = &cl_char->player;
    PlayerState *p = &cl_char->player.state;
    if (p->alive) {
        float x = p->x, y = p->y, w = cl_p->w_render, h = cl_p->h_render, theta = cl_char->angle_render;

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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        thickLineRGBA(renderer, start_x, start_y, end_x, end_y, 5, 0, 0, 0, 255);

        // Draw a circle around the player
        //circleRGBA(renderer, center_x, center_y, radius, 0, 0, 0, 255);
        //Debug_Info("theta = %f\n", theta * 360 / (2*CONST_PI));
    }
}

void ClientPlayer_Render(cl_player_t *cl_p, SDL_Renderer *renderer) {
    PlayerState *p = &cl_p->state;
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

void ClientPlayer_ShootingHandler(cl_char_t *cl_char, GameState *game) {
    PlayerState *p = &cl_char->player.state;
    // 1. Handle aiming (doesn't affect movement)
    if (cl_char->input.up) {
        cl_char->angle += (3/360.0) * 2 * CONST_PI;
        if (cl_char->angle > CONST_PI/2) cl_char->angle = CONST_PI/2;
    }
    if (cl_char->input.down) {
        cl_char->angle -= (3/360.0) * 2 * CONST_PI;
        if (cl_char->angle < 0) cl_char->angle = 0;
    }
    cl_char->angle_render =
        cl_char->player.state.facing_right ? cl_char->angle : CONST_PI - cl_char->angle;

    // 2. Handle throwing
    cl_char->projectile_timer--;
    if (cl_char->projectile_timer <= 0)
        cl_char->projectile_timer = 0;

    cl_char->change_arm_timer--;
    if (cl_char->change_arm_timer <= 0)
        cl_char->change_arm_timer = 0;

    if (!cl_char->change_arm_timer) {
        if (cl_char->input.change_arm) {
            cl_char->change_arm_timer = 15;
            cl_char->curr_arm = (cl_char->curr_arm + 1) % 2;
            Debug_Info("Current arm %d", cl_char->curr_arm);
        }
    }

    int holding_throw = cl_char->input.shoot;

    if (!cl_char->projectile_timer) { /* timer has to be zero, in order for the throwing logic to happen */
        if (!cl_char->throwing) { /* NOT THROWING, CAN BEGIN THROWING */
            if (holding_throw) {
                cl_char->throwing = 1;
            }
        }
        else {  /* ALREADY THROWING */
            if (holding_throw) {    /* HOLD MORE POWER */
                cl_char->power += 5;
                cl_char->power = MIN(cl_char->power, MAX_POWER);
            }
            else {  /* BUTTON RELEASED, THROW! */
                int center_x = p->x + p->w/2;
                int center_y = p->y + p->h/2;
                float dx = p->x - center_x;
                float dy = p->y - center_y;
                float radius = sqrt(dx*dx + dy*dy);
                float start_x = center_x + radius * cos(cl_char->angle_render);
                float start_y = center_y + radius * (-sin(cl_char->angle_render));

                Projectile_Throw(&game->projectile_sys, cl_char->curr_arm,
                                 start_x, start_y, cl_char->angle_render, cl_char->power, cl_char->id);
                cl_char->projectile_timer = 30;

                Debug_Info("Player throwed bomb with power %d", cl_char->power);

                /* not throwing anymore, and power goes to zero */
                cl_char->throwing = 0;
                cl_char->power = 0;
            }
        }
    }
}

void ClientPlayer_Clean(cl_player_t *p) {
    for (int i = 0; i < PLAYER_NUM_SPRITES; i++) {
        if (p->sprites[i])
            SDL_DestroyTexture(p->sprites[i]);
    }
}
