#ifndef _CLIENT_INIT_H
#define _CLIENT_INIT_H

#include "../shared/core_types.h"

int ClientInit_MoveLeft_Player1(Input *input);
int ClientInit_MoveRight_Player1(Input *input);
int ClientInit_IncreaseAngle_Player1(Input *input);
int ClientInit_DecreaseAngle_Player1(Input *input);
int ClientInit_ThrowBomb_Player1(Input *input);
int ClientInit_CycleArm_Player1(Input *input);
void ClientInit_Player0(PlayerState *p);
void ClientInit_Player1(PlayerState *p);
void ClientInit_Players(PlayerState p[]);

#endif
