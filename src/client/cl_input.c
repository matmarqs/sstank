#include "cl_types.h"
#include "../shared/base.h"

void Input_InitKeys(Input *input) {
    input->w = input->a = input->s = input->d = input->space = input->l_shift = FALSE;
}

int Input_SetEvents(SDL_Event *ev, Input *input) {
    int done = FALSE;

    while (SDL_PollEvent(ev)) {
        switch(ev->type) {
            case SDL_QUIT:
                done = TRUE;
                break;

            case SDL_KEYDOWN:
                switch(ev->key.keysym.scancode) {
                    case SDL_SCANCODE_W:     input->w = TRUE; break;
                    case SDL_SCANCODE_A:     input->a = TRUE; break;
                    case SDL_SCANCODE_S:     input->s = TRUE; break;
                    case SDL_SCANCODE_D:     input->d = TRUE; break;
                    case SDL_SCANCODE_SPACE: input->space = TRUE; break;
                    case SDL_SCANCODE_LSHIFT: input->l_shift = TRUE; break;
                    default: break;
                }
                break;

            case SDL_KEYUP:
                switch(ev->key.keysym.scancode) {
                    case SDL_SCANCODE_W:     input->w = FALSE; break;
                    case SDL_SCANCODE_A:     input->a = FALSE; break;
                    case SDL_SCANCODE_S:     input->s = FALSE; break;
                    case SDL_SCANCODE_D:     input->d = FALSE; break;
                    case SDL_SCANCODE_SPACE: input->space = FALSE; break;
                    case SDL_SCANCODE_LSHIFT: input->l_shift = FALSE; break;
                    default: break;
                }
                break;

            default:
                break;
        }
    }

    return done;
}

PlayerActions Input_GetActions(Input *raw) {
    PlayerActions actions = {0};
    actions.move_left = raw->a;
    actions.move_right = raw->d;
    actions.jump = raw->space;
    actions.shoot = raw->l_shift;
    actions.aim_up = raw->w;
    actions.aim_down = raw->s;
    return actions;
}

void Client_SendActions(ClientState *client) {
    PlayerActions actions = Input_GetActions(&client->input);

    // Send ONLY actions, not raw keys
    ActionPacket pkt;
    pkt.sequence = client->next_seq++;
    pkt.timestamp = SDL_GetTicks();
    pkt.actions = actions;

    NetProtocol_SendPacketToServer(client->server_socket, PACKET_CL_ACTIONS, &pkt, sizeof(pkt));
}

void Player_ShootingHandler(PlayerState *p, GameState *game) {
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

