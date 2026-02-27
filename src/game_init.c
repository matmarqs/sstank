#include "debug.h"
#include "player.h"

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

void Init_Player0(Player *p) {
    p->id = 0;
    p->sprites_path[0] = "assets/img/gato_maca0.png";
    p->sprites_path[1] = "assets/img/gato_maca1.png";
    p->sprites_path[2] = "assets/img/gato_maca2.png";
    for (int i = 0; i < NUM_SPRITES; i++) {
        Debug_Info("Loaded asset %s successfully", p->sprites_path[i]);
    }

    p->w_over_h = 100.0 / 100.0;
    p->input_mapper.move_left = MoveLeft_Player0;
    p->input_mapper.move_right = MoveRight_Player0;
    p->input_mapper.increase_angle = IncreaseAngle_Player0;
    p->input_mapper.decrease_angle = DecreaseAngle_Player0;
    p->input_mapper.throw_projectile = ThrowBomb_Player0;
}

void Init_Player1(Player *p) {
    p->id = 1;
    p->sprites_path[0] = "assets/img/gato_banana0.png";
    p->sprites_path[1] = "assets/img/gato_banana1.png";
    p->sprites_path[2] = "assets/img/gato_banana2.png";
    for (int i = 0; i < NUM_SPRITES; i++) {
        Debug_Info("Loaded asset %s successfully", p->sprites_path[i]);
    }

    p->w_over_h = 637.0 / 358.0;
    p->input_mapper.move_left = MoveLeft_Player1;
    p->input_mapper.move_right = MoveRight_Player1;
    p->input_mapper.increase_angle = IncreaseAngle_Player1;
    p->input_mapper.decrease_angle = DecreaseAngle_Player1;
    p->input_mapper.throw_projectile = ThrowBomb_Player1;
}

void Init_Players(Player p[]) {
    Init_Player0(&p[0]);
    Init_Player1(&p[1]);
}
