#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <mbed.h>

// グローバル変数の宣言
extern int leftJoystickX;
extern int leftJoystickY;
extern int rightJoystickX;
extern int targetSpeedRight;
extern int targetSpeedLeft;
extern int8_t pic;
extern int targetSpeedRight_M;
extern int targetSpeedLeft_M;
extern int kick;

extern const uint32_t penguinID;

extern CAN can1;
extern CAN can2;

void processInput(char *output_buf);

#endif // CONTROLLER_HPP