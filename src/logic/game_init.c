#include "../common/common.h"
#include "../common/debug.h"

#include "game_init.h"

int MoveLeft_Player0(Input *input) {
    return input->left && !input->right;
}

int MoveRight_Player0(Input *input) {
    return !input->left && input->right;
}

int IncreaseAngle_Player0(Input *input) {
    return input->up && !input->down;
}

int DecreaseAngle_Player0(Input *input) {
    return !input->up && input->down;
}

int ThrowBomb_Player0(Input *input) {
    return input->enter;  // Enter/Return key
}

int CycleArm_Player0(Input *input) {
    return input->r_ctrl;
}

int MoveLeft_Player1(Input *input) {
    return input->a && !input->d;
}

int MoveRight_Player1(Input *input) {
    return !input->a && input->d;
}

int IncreaseAngle_Player1(Input *input) {
    return input->w && !input->s;
}

int DecreaseAngle_Player1(Input *input) {
    return !input->w && input->s;
}

int ThrowBomb_Player1(Input *input) {
    return input->space;  // Space bar
}

int CycleArm_Player1(Input *input) {
    return input->l_shift;
}

void Init_Player0(Player *p) {
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
    p->input_mapper.move_left = MoveLeft_Player0;
    p->input_mapper.move_right = MoveRight_Player0;
    p->input_mapper.increase_angle = IncreaseAngle_Player0;
    p->input_mapper.decrease_angle = DecreaseAngle_Player0;
    p->input_mapper.throw_projectile = ThrowBomb_Player0;
    p->input_mapper.cycle_arm = CycleArm_Player0;
}

void Init_Player1(Player *p) {
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
    p->input_mapper.move_left = MoveLeft_Player1;
    p->input_mapper.move_right = MoveRight_Player1;
    p->input_mapper.increase_angle = IncreaseAngle_Player1;
    p->input_mapper.decrease_angle = DecreaseAngle_Player1;
    p->input_mapper.throw_projectile = ThrowBomb_Player1;
    p->input_mapper.cycle_arm = CycleArm_Player1;
}

void Init_Players(Player p[]) {
    Init_Player0(&p[0]);
    Init_Player1(&p[1]);
}
