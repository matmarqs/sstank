#include <SDL2/SDL2_gfxPrimitives.h> // boxRGBA
#include <SDL2/SDL_net.h>

#include "cl_char.h"
#include "cl_net.h"
#include "cl_input.h"
#include "cl_types.h"

void cl_char_Init(cl_char_t *cl_char, cl_player_t *cl_player, TCPsocket server) {
    cl_char->id = cl_player->state->id;
    cl_char->player = cl_player;
    cl_input_InitKeys(&cl_char->input);
    cl_char->angle = 0;
    cl_char->angle_render = 0;
    cl_char->throwing = 0;
    cl_char->power = 0;
    cl_char->change_arm_timer = 0;
    cl_char->curr_arm = 0;
    cl_char->projectile_timer = 0;
    cl_char->server = server;
}


void cl_char_Update(cl_char_t *cl_char) {
    cl_char_MovingHandler(cl_char);
    cl_char_ShootingHandler(cl_char);
}

void cl_char_RenderPowerGauge(cl_char_t *cl_char, SDL_Renderer *renderer) {
    PlayerState *p = cl_char->player->state;
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

void cl_char_RenderAngle(cl_char_t *cl_char, SDL_Renderer *renderer) {
    cl_player_t *cl_p = cl_char->player;
    PlayerState *p = cl_char->player->state;
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

void cl_char_MovingHandler(cl_char_t *cl_char) {
    // Convert raw input to actions
    PlayerActions actions = {
        .move_left = cl_char->input.left,
        .move_right = cl_char->input.right,
    };
    // Send actions to server
    cl_net_SendMovement(cl_char->server, actions);
}

void cl_char_ShootingHandler(cl_char_t *cl_char) {
    // 1. Handle aiming (doesn't affect movement)
    if (cl_char->input.up) {
        cl_char->angle += (3/360.0) * 2 * CONST_PI;
        if (cl_char->angle > CONST_PI/2) cl_char->angle = CONST_PI/2;
    }
    if (cl_char->input.down) {
        cl_char->angle -= (3/360.0) * 2 * CONST_PI;
        if (cl_char->angle < 0) cl_char->angle = 0;
    }
    // flip angle if facing right or left
    cl_char->angle_render =
        cl_char->player->state->facing_right ? cl_char->angle : CONST_PI - cl_char->angle;
    // 2. Handle throwing
    cl_char->projectile_timer--;
    if (cl_char->projectile_timer <= 0)
        cl_char->projectile_timer = 0;
    cl_char->change_arm_timer--;
    if (cl_char->change_arm_timer <= 0)
        cl_char->change_arm_timer = 0;
    // change weapon timer
    if (!cl_char->change_arm_timer) {
        if (cl_char->input.change_arm) {
            cl_char->change_arm_timer = 15;
            cl_char->curr_arm = (cl_char->curr_arm + 1) % 2;
            Debug_Info("Changed curr_arm = %d", cl_char->curr_arm);
        }
    }
    // shoot when hold button is released
    int holding_throw = cl_char->input.shoot;
    if (!cl_char->projectile_timer) { /* timer has to be zero */
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
                cl_net_SendThrow(cl_char->server, cl_char->angle_render, cl_char->power, cl_char->curr_arm);
                cl_char->projectile_timer = 30;
                cl_char->throwing = 0;
                cl_char->power = 0;
            }
        }
    }
}
