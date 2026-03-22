#include "base.h"

#include "core_player.h"
#include "core_physics.h" // Physics_DeterminePlayerState

void Player_Init(PlayerState *p) {
    p->w = BASE_PLAYER_WIDTH;
    p->h = BASE_PLAYER_HEIGHT;
    p->x = (WORLD_WIDTH - p->w) / 2.0;    /* start sprite center of screen */
    p->y = (WORLD_HEIGHT - p->h) / 2.0;
    p->vx = 0;
    p->vy = 0;
    p->alive = 1;
    p->health = 100;
    p->on_ground = 0;
    p->facing_right = 1;
    p->throw_cooldown = 0;
}

void Player_Teleport(PlayerState *p, float x, float y) {
    p->x = x;
    p->y = y;
    p->vx = 0;
    p->vy = 0;
}

void Player_MovementHandler(PlayerState *p, GameState *game, PlayerActions actions, float dt) {
    UNUSED(dt);

    float input_vx = 0;
    if (actions.move_left && !actions.move_right) {
        input_vx = -PLAYER_SPEED;
        p->facing_right = FALSE;
    }
    else if (!actions.move_left && actions.move_right) {
        input_vx = PLAYER_SPEED;
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
    }
}
