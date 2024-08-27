#include "controller.hpp"

// グローバル変数の定義
int leftJoystickX = 0;
int leftJoystickY = 0;
int rightJoystickX = 0;

void processInput(char *output_buf)
{
    if (strncmp(output_buf, "L3_x:", 5) == 0) // "L3_x:"という文字列で始まるかどうかを確認します
    {
        char *dataPointer = output_buf + 5; // "L3_x:"の後の文字列の先頭ポインタを取得
        // 数字部分を読み取る
        leftJoystickX = atoi(dataPointer);
        // 数字を使って何かをする（ここでは単にPCに出力）
        // printf("Left Joystick X: %d\n", leftJoystickX);
    }
    else if (strncmp(output_buf, "L3_y:", 5) == 0) // "L3_y:"という文字列で始まるかどうかを確認します
    {
        char *dataPointer = output_buf + 5; // "L3_y:"の後の文字列の先頭ポインタを取得
        // 数字部分を読み取る
        leftJoystickY = atoi(dataPointer);
        // 数字を使って何かをする（ここでは単にPCに出力）
        // printf("Left Joystick Y: %d\n", leftJoystickY);
    }
    else if (strncmp(output_buf, "R3_x:", 5) == 0) // "R3_x:"という文字列で始まるかどうかを確認します
    {
        char *dataPointer = output_buf + 5; // "R3_x:"の後の文字列の先頭ポインタを取得
        // 数字部分を読み取る
        rightJoystickX = atoi(dataPointer);
        // 数字を使って何かをする（ここでは単にPCに出力）
        // printf("Right Joystick X: %d\n", rightJoystickX);
        rightJoystickX = rightJoystickX / 5;
    }
    else if (strncmp(output_buf, "L1ON", 4) == 0) // "L1ON"という文字列で始まるかどうかを確認します
    {
        penguin.pwm[0] = -10000;
        penguin.pwm[1] = -10000;
        penguin.pwm[2] = -10000;
        penguin.pwm[3] = -10000;
        // penguin.send();
    }
    else if (strncmp(output_buf, "L1OFF", 5) == 0 || strncmp(output_buf, "R1OFF", 5) == 0) // "L1OFF"または"R1OFF"という文字列で始まるかどうかを確認します
    {
        penguin.pwm[0] = 0;
        penguin.pwm[1] = 0;
        penguin.pwm[2] = 0;
        penguin.pwm[3] = 0;
        // penguin.send();
    }
    else if (strncmp(output_buf, "R1ON", 4) == 0) // "R1ON"という文字列で始まるかどうかを確認します
    {
        penguin.pwm[0] = 1500;
        penguin.pwm[1] = 1500;
        penguin.pwm[2] = 1500;
        penguin.pwm[3] = 1500;
    }
}