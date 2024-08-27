#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <mbed.h>
#include "firstpenguin.hpp"

// グローバル変数の宣言
extern int leftJoystickX;
extern int leftJoystickY;
extern int rightJoystickX;

extern const uint32_t penguinID;

extern CAN can1;
extern CAN can2;

extern FirstPenguin penguin;

void processInput(char *output_buf);

#endif // CONTROLLER_HPP