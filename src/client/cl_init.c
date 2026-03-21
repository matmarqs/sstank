#include "cl_init.h"

#include "../shared/base.h"


int ClientInit_MoveLeft(Input *input) {
    return input->a && !input->d;
}

int ClientInit_MoveRight(Input *input) {
    return !input->a && input->d;
}

int ClientInit_IncreaseAngle(Input *input) {
    return input->w && !input->s;
}

int ClientInit_DecreaseAngle(Input *input) {
    return !input->w && input->s;
}

int ClientInit_ThrowBomb(Input *input) {
    return input->space;  // Space bar
}

int ClientInit_CycleArm(Input *input) {
    return input->l_shift;
}

void ClientInit_Player0(PlayerState *p) {
    p->id = 0;
    p->sprite_inverted = TRUE;
    /* idle */
    p->sprites_path[ 0] = "assets/img/kirby_idle_0.png";
    p->sprites_path[ 1] = "assets/img/kirby_idle_0.png";
    p->sprites_path[ 2] = "assets/img/kirby_idle_0.png";
    p->sprites_path[ 3] = "assets/img/kirby_idle_1.png";
    p->sprites_path[ 4] = "assets/img/kirby_idle_1.png";

    /* running */
    p->sprites_path[ 5] = "assets/img/kirby_running_0.png";
    p->sprites_path[ 6] = "assets/img/kirby_running_2.png";
    p->sprites_path[ 7] = "assets/img/kirby_running_3.png";
    p->sprites_path[ 8] = "assets/img/kirby_running_4.png";
    p->sprites_path[ 9] = "assets/img/kirby_running_0.png";
    p->sprites_path[10] = "assets/img/kirby_running_2.png";
    p->sprites_path[11] = "assets/img/kirby_running_3.png";

    for (int i = 0; i < NUM_SPRITES; i++) {
        Debug_Info("Loaded asset %s successfully", p->sprites_path[i]);
    }
    p->input_mapper.move_left = ClientInit_MoveLeft;
    p->input_mapper.move_right = ClientInit_MoveRight;
    p->input_mapper.increase_angle = ClientInit_IncreaseAngle;
    p->input_mapper.decrease_angle = ClientInit_DecreaseAngle;
    p->input_mapper.throw_projectile = ClientInit_ThrowBomb;
    p->input_mapper.cycle_arm = ClientInit_CycleArm;
}

void ClientInit_Player1(PlayerState *p) {
    p->id = 1;
    p->sprite_inverted = TRUE;

    /* idle */
    p->sprites_path[ 0] = "assets/img/sonic_idle_0.png";
    p->sprites_path[ 1] = "assets/img/sonic_idle_1.png";
    p->sprites_path[ 2] = "assets/img/sonic_idle_2.png";
    p->sprites_path[ 3] = "assets/img/sonic_idle_3.png";
    p->sprites_path[ 4] = "assets/img/sonic_idle_4.png";

    /* running */
    p->sprites_path[ 5] = "assets/img/sonic_running_0.png";
    p->sprites_path[ 6] = "assets/img/sonic_running_1.png";
    p->sprites_path[ 7] = "assets/img/sonic_running_2.png";
    p->sprites_path[ 8] = "assets/img/sonic_running_3.png";
    p->sprites_path[ 9] = "assets/img/sonic_running_4.png";
    p->sprites_path[10] = "assets/img/sonic_running_5.png";
    p->sprites_path[11] = "assets/img/sonic_running_6.png";

    for (int i = 0; i < NUM_SPRITES; i++) {
        Debug_Info("Loaded asset %s successfully", p->sprites_path[i]);
    }
    p->input_mapper.move_left = ClientInit_MoveLeft;
    p->input_mapper.move_right = ClientInit_MoveRight;
    p->input_mapper.increase_angle = ClientInit_IncreaseAngle;
    p->input_mapper.decrease_angle = ClientInit_DecreaseAngle;
    p->input_mapper.throw_projectile = ClientInit_ThrowBomb;
    p->input_mapper.cycle_arm = ClientInit_CycleArm;
}

void ClientInit_Players(PlayerState p[]) {
    ClientInit_Player0(&p[0]);
    ClientInit_Player1(&p[1]);
}
