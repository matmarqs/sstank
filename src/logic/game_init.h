#ifndef _GAME_INIT_H
#define _GAME_INIT_H

#include "../struct/st_input.h"
#include "../struct/st_player.h"

int MoveLeft_Player0(Input *input);
int MoveRight_Player0(Input *input);
int IncreaseAngle_Player0(Input *input);
int DecreaseAngle_Player0(Input *input);
int ThrowBomb_Player0(Input *input);
int CycleArm_Player0(Input *input);
int MoveLeft_Player1(Input *input);
int MoveRight_Player1(Input *input);
int IncreaseAngle_Player1(Input *input);
int DecreaseAngle_Player1(Input *input);
int ThrowBomb_Player1(Input *input);
int CycleArm_Player1(Input *input);
void Init_Player0(Player *p);
void Init_Player1(Player *p);
void Init_Players(Player p[]);

#endif
