#ifndef _GAME_INIT_H
#define _GAME_INIT_H

#include "../shared/struct/st_input.h"
#include "../shared/struct/st_player.h"

int ClientInit_MoveLeft_Player0(Input *input);
int ClientInit_MoveRight_Player0(Input *input);
int ClientInit_IncreaseAngle_Player0(Input *input);
int ClientInit_DecreaseAngle_Player0(Input *input);
int ClientInit_ThrowBomb_Player0(Input *input);
int ClientInit_CycleArm_Player0(Input *input);
int ClientInit_MoveLeft_Player1(Input *input);
int ClientInit_MoveRight_Player1(Input *input);
int ClientInit_IncreaseAngle_Player1(Input *input);
int ClientInit_DecreaseAngle_Player1(Input *input);
int ClientInit_ThrowBomb_Player1(Input *input);
int ClientInit_CycleArm_Player1(Input *input);
void ClientInit_Player0(Player *p);
void ClientInit_Player1(Player *p);
void ClientInit_Players(Player p[]);

#endif
