#include "base_common.h"
#include "base_debug.h"
#include "base_config.h" // WORLD_WIDTH, WORLD_HEIGHT

#include "logic_player.h" // already includes "struct GameState"
#include "logic_physics.h" // Physics_DeterminePlayerState
#include "logic_projectile.h" // Projectile_Throw
#include "logic_input.h"

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
    Input_InitKeys(&p->input);

    return SUCCESS;
}

void Player_Teleport(Player *p, float x, float y) {
    p->x = x;
    p->y = y;
    p->vx = 0;
    p->vy = 0;
}

void Player_ShootingHandler(Player *p, GameState *game) {
    // 1. Handle aiming (doesn't affect movement)
    if (p->input_mapper.increase_angle(&p->input)) {
        p->angle += (3/360.0) * 2 * CONST_PI;
        if (p->angle > CONST_PI/2) p->angle = CONST_PI/2;
    }
    if (p->input_mapper.decrease_angle(&p->input)) {
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

    int holding_change_arm = p->input_mapper.cycle_arm(&p->input);

    if (!p->change_arm_timer) {
        if (holding_change_arm) {
            p->change_arm_timer = 15;
            p->curr_arm = (p->curr_arm + 1) % 2;
            Debug_Info("Current arm %d", p->curr_arm);
        }
    }

    int holding_throw = p->input_mapper.throw_projectile(&p->input);

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

void Player_MovementHandler(Player *p, GameState *game) {
    // 1. INPUT HANDLING (always runs, doesn't affect state)
    float input_vx = 0;
    if (p->input_mapper.move_left(&p->input)) {
        input_vx = -SPEED;
        p->facing_right = FALSE;
    }
    if (p->input_mapper.move_right(&p->input)) {
        input_vx = SPEED;
        p->facing_right = TRUE;
    }

    // Determine state and update
    PlayerMoveState state = Physics_DeterminePlayerState(&game->terrain, p);

    switch (state) {
        case GROUNDED:
            Physics_UpdateGrounded(&game->terrain, p, input_vx);
            break;
        case FALLING:
            Physics_UpdateFalling(&game->terrain, p, input_vx);
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

void Player_Clean(Player *p) {
    for (int i = 0; i < NUM_SPRITES; i++) {
        if (p->sprites[i])
            SDL_DestroyTexture(p->sprites[i]);
    }
}
