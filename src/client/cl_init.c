#include "cl_init.h"

void ClientInit_Player0(cl_player_t *p) {
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
}

void ClientInit_Player1(cl_player_t *p) {
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

}

void ClientInit_Players(cl_player_t p[]) {
    ClientInit_Player0(&p[0]);
    ClientInit_Player1(&p[1]);
}
