#ifndef __KEY_H
#define __KEY_H

#include "common.h"

// 函数声明
void Key_Handler(KeyState_t key);
void Handle_Main_Menu(KeyState_t key);
void Handle_Temp_Setting(KeyState_t key);
void Handle_Humid_Setting(KeyState_t key);
void Handle_Smoke_Setting(KeyState_t key);
#endif
