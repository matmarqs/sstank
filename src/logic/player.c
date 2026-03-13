#include <SDL2/SDL_image.h> // IMG_LoadTexture
#include <SDL2/SDL2_gfxPrimitives.h> // boxRGBA

#include "../common/common.h"
#include "../common/debug.h"
#include "../common/config.h" // WORLD_WIDTH, WORLD_HEIGHT

#include "player.h" // already includes "struct Game"
#include "movement.h" // Movement_DeterminePlayerState
#include "projectile.h" // Projectile_Throw

int loop_integer(int i, int n) {
    int period = 2 * (n - 1);  // For n=6, period = 10
    int t = i % period;
    if (t < n) {
        return t;           // 0,1,2,3,4,5
    } else {
        return period - t;  // 4,3,2,1
    }
}

int Player_Load(Player *p, SDL_Renderer *renderer) {
    for (int i = 0; i < NUM_SPRITES; i++) {
        if (p->sprites_path[i]) {
            p->sprites[i] = IMG_LoadTexture(renderer, p->sprites_path[i]);
            if (!p->sprites[i]) {
                Debug_Error("Player_Load error: error loading image %s\n", p->sprites_path[i]);
                return FAILURE;
            }
        }
    }
    int w, h;
    SDL_QueryTexture(p->sprites[0], NULL, NULL, &w, &h);

    p->w_render = BASE_PLAYER_WIDTH;
    p->h_render = BASE_PLAYER_HEIGHT * ((float) h) / w;

    float hitbox_factor = 2.0 / 3;
    p->w = p->w_render * hitbox_factor;
    p->h = p->h_render;

    p->x = (WORLD_WIDTH - p->w) / 2.0 - 50;    /* start sprite center of screen */
    p->y = (WORLD_HEIGHT - p->h) / 2.0 - 100;
    p->vx = 0; p->vy = 0;
    p->angle = 0;
    p->angle_render = 0;
    p->curr_sprite = 0;
    p->facing_right = !p->sprite_inverted;
    p->projectile_timer = 0;
    p->change_arm_timer = 0;
    p->curr_arm = 0;
    p->on_ground = 0;
    p->alive = 1;
    p->throwing = 0;
    p->power = 0;
    p->health = 100;
    p->damage_timer = 0;

    return SUCCESS;
}

void Player_Teleport(Player *p, float x, float y) {
    p->x = x;
    p->y = y;
    p->vx = 0;
    p->vy = 0;
}

void Player_Update(Player *p, Game *game) {
    p->damage_timer--;
    p->damage_timer = MAX(p->damage_timer, 0);
    if (p->health <= 0) {
        p->alive = 0;
    }
    if (p->alive) {
        Player_ShootingHandler(p, game);
        Player_MovementHandler(p, game);
        Player_AnimationHandler(p, game);
    }
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

    p->change_arm_timer--;
    if (p->change_arm_timer <= 0)
        p->change_arm_timer = 0;

    int holding_change_arm = p->input_mapper.cycle_arm(&game->input);

    if (!p->change_arm_timer) {
        if (holding_change_arm) {
            p->change_arm_timer = 15;
            p->curr_arm = (p->curr_arm + 1) % 2;
            Debug_Info("Current arm %d", p->curr_arm);
        }
    }

    int holding_throw = p->input_mapper.throw_projectile(&game->input);

    if (!p->projectile_timer) { /* timer has to be zero, in order for the throwing logic to happen */
        if (!p->throwing) { /* NOT THROWING, CAN BEGIN THROWING */
            if (holding_throw) {
                p->throwing = 1;
            }
        }
        else {  /* ALREADY THROWING */
            if (holding_throw) {    /* HOLD MORE POWER */
                p->power += 5;
                p->power = MIN(p->power, MAX_POWER);
            }
            else {  /* BUTTON RELEASED, THROW! */
                int center_x = p->x + p->w/2;
                int center_y = p->y + p->h/2;
                float dx = p->x - center_x;
                float dy = p->y - center_y;
                float radius = sqrt(dx*dx + dy*dy);
                float start_x = center_x + radius * cos(p->angle_render);
                float start_y = center_y + radius * (-sin(p->angle_render));

                Projectile_Throw(&game->projectile_sys, p->curr_arm, start_x, start_y, p->angle_render, p->power, p->id);
                p->projectile_timer = 30;

                Debug_Info("Player %d throwed bomb with power %d", p->id, p->power);

                /* not throwing anymore, and power goes to zero */
                p->throwing = 0;
                p->power = 0;
            }
        }
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
    if (p->y > WORLD_HEIGHT) {
        p->y = WORLD_HEIGHT;
        p->alive = 0;   /* the player is dead */
        Debug_Info("Player %d is dead!", p->id);
    }
}

void Player_AnimationHandler(Player *p, Game *game) {
    // Animation
    int t = game->time;
    if (p->vx == 0) {
        p->curr_sprite = loop_integer(t / 8, 5);
    } else {
        if (p->id == 0) {
            p->curr_sprite = 5 + loop_integer(t / 10, 7);
        }
        else {
            p->curr_sprite = 5 + loop_integer(t / 5, 7);
        }
    }
}

void Player_RenderPowerGauge(Player *p, SDL_Renderer *renderer) {
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
    if (p->power > 0) {
        float normalized_power = (float)p->power / MAX_POWER;  // 0.0 to 1.0
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
    if (p->throwing) {
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

void Player_Render(Player *p, SDL_Renderer *renderer) {
    if (p->alive) {
        if (p->damage_timer > 0) {
            // Make sprite reddish
            SDL_SetTextureColorMod(p->sprites[p->curr_sprite], 255, 100, 100);
            p->damage_timer--;
        } else {
            // Normal color
            SDL_SetTextureColorMod(p->sprites[p->curr_sprite], 255, 255, 255);
        }

        float x = p->x, y = p->y, w = p->w_render, h = p->h_render, theta = p->angle_render;

        SDL_Rect rect = { x, y, w, h };
        SDL_RenderCopyEx(renderer, p->sprites[p->curr_sprite], NULL, &rect, 0, NULL, p->sprite_inverted ? !p->facing_right : p->facing_right);

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

        //        // Draw UI elements
        Player_RenderPowerGauge(p, renderer);

        //circleRGBA(renderer, center_x, center_y, radius, 0, 0, 0, 255);

        //Debug_Info("theta = %f\n", theta * 360 / (2*CONST_PI));

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        thickLineRGBA(renderer, start_x, start_y, end_x, end_y, 5, 0, 0, 0, 255);

        // Reset color mod for other players (important!)
        SDL_SetTextureColorMod(p->sprites[p->curr_sprite], 255, 255, 255);
    }
}

void Player_Clean(Player *p) {
    for (int i = 0; i < NUM_SPRITES; i++) {
        if (p->sprites[i])
            SDL_DestroyTexture(p->sprites[i]);
    }
}
