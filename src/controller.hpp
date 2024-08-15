#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "mbed.h"

// 変数宣言をexternで宣言
extern int leftJoystickX;
extern int leftJoystickY;
extern int rightJoystickX;

void processInput(char *output_buf);

#endif