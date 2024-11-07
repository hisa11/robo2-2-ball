#include "controller.hpp"
bool kick_L = 0;
bool kick_R = 0;
bool tire_previous=0;
bool start = 0;

void processInput(char *output_buf)
{
    if (strncmp(output_buf, "L3_x:", 5) == 0) // "L3_x:"という文字列で始まるかどうかを確認します
    {
        char *dataPointer = output_buf + 5; // "L3_x:"の後の文字列の先頭ポインタを取得
        // 数字部分を読み取る
        leftJoystickX = atoi(dataPointer);
        // 数字を使って何かをする（ここでは単にPCに出力）
        // printf("Left Joystick X: %d\n", leftJoystickX);
        start = 1;
    }
    else if (strncmp(output_buf, "L3_y:", 5) == 0) // "L3_y:"という文字列で始まるかどうかを確認します
    {
        char *dataPointer = output_buf + 5; // "L3_y:"の後の文字列の先頭ポインタを取得
        // 数字部分を読み取る
        leftJoystickY = atoi(dataPointer);
        // 数字を使って何かをする（ここでは単にPCに出力）
        // printf("Left Joystick Y: %d\n", leftJoystickY);
        start = 1;
    }
    else if (strncmp(output_buf, "R3_x:", 5) == 0) // "R3_x:"という文字列で始まるかどうかを確認します
    {
        char *dataPointer = output_buf + 5; // "R3_x:"の後の文字列の先頭ポインタを取得
        // 数字部分を読み取る
        rightJoystickX = atoi(dataPointer);
        // 数字を使って何かをする（ここでは単にPCに出力）
        // printf("Right Joystick X: %d\n", rightJoystickX);
        rightJoystickX = rightJoystickX / 5;
        start = 1;
    }
    else if (strncmp(output_buf, "L1OFF", 5) == 0 || strncmp(output_buf, "R1OFF", 5) == 0 || strncmp(output_buf,"psOFF",5) == 0) // "L1OFF"または"R1OFF"という文字列で始まるかどうかを確認します
    {
        kick = 1;
        kick_L = 0;
        kick_R = 0;
    }
    else if (strncmp(output_buf, "L1ON", 4) == 0) // "L1ON"という文字列で始まるかどうかを確認します
    {
        kick = 0;
        kick_L = 1;
        // penguin.send();
    }
    else if (strncmp(output_buf, "R1ON", 4) == 0) // "R1ON"という文字列で始まるかどうかを確認します
    {
        kick = 2;
        kick_R = 1;
    }
    else if (strncmp(output_buf, "cross", 5) == 0)
    {
        pic = 0;
    }
    else if (strncmp(output_buf, "triangle", 5) == 0)
    {
        pic = 2;
    }
    else if(strncmp(output_buf,"crossOFF",8) == 0 || strncmp(output_buf,"triangleOFF",11) == 0){
        pic = 0;
    }
    else if (strncmp(output_buf, "go", 2) == 0)
    {
        targetSpeedLeft_M = 3000;
        targetSpeedRight_M = -3000;
        printf("go\n");
    }
    else if (strncmp(output_buf, "back", 4) == 0)
    {
        targetSpeedLeft_M = -3000;
        targetSpeedRight_M = 3000;
    }
    else if (strncmp(output_buf, "stop", 4) == 0)
    {
        targetSpeedLeft_M = 0;
        targetSpeedRight_M = 0;
    }
    else if(strncmp(output_buf,"psON",4)== 0){
        kick = 3;
    }
    else if(strncmp(output_buf,"L3ON",4)== 0){
        tire_previous = 1;
    }
    else if(strncmp(output_buf,"L3OFF",5)== 0){
        tire_previous = 0;
    }
    printf("kick_L = %d,kick_R = %d\n",kick_L,kick_R);
}