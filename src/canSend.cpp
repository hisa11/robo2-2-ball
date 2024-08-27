// canSend.cpp
#include "mbed.h"
#include "PID.hpp"
#include "firstpenguin.hpp"
#include "canSend.hpp" // ヘッダーファイルをインクルード

extern CAN can1; // メインファイルで定義されたグローバル変数を参照
extern CAN can2;
extern FirstPenguin penguin;
extern int16_t currentSpeed;
extern int16_t currentSpeed1;
extern int16_t picAngle;
extern int targetSpeedRight;
extern int targetSpeedLeft;
extern uint8_t DATA[8];

// PID制御器のインスタンスを参照
extern PID pidControllerRight;
extern PID pidControllerLeft;

int pic = 0;

// extern int8_t pic;

void canSend()
{
    while (1)
    {
        static int previous_angle = 0;
        CANMessage msg1, msg2;
        if (can1.read(msg1) && msg1.id == 0x201)
        {
            currentSpeed = (msg1.data[2] << 8) | msg1.data[3];
        }
        if (can1.read(msg2) && msg2.id == 0x202)
        {
            currentSpeed1 = (msg2.data[2] << 8) | msg2.data[3];
        }
        if (can1.read(msg2) && msg2.id == 0x202)
        {
            picAngle = (msg2.data[0] << 8) | msg2.data[1];
            int diff = pic - previous_angle;
            if (diff > 4095) // ラップアラウンドの巻き戻し
            {
                picAngle -= (8192 - pic);
            }
            else if (diff < -4095) // ラップアラウンドの巻き進み
            {
                picAngle += (8192 + pic);
            }
            else
            {
                picAngle += diff;
            }
            previous_angle = pic;
        }
        printf("currentSpeed = %d , currentSpeed1 = %d\n", currentSpeed, currentSpeed1);

        float outputRight = pidControllerRight.calculate(targetSpeedRight, currentSpeed);
        float outputLeft = pidControllerLeft.calculate(targetSpeedLeft, currentSpeed1);

        if (outputRight > 9990)
        {
            outputRight = 9990;
        }
        if (outputRight < -9990)
        {
            outputRight = -9990;
        }
        if (outputLeft > 9990)
        {
            outputLeft = 9990;
        }
        if (outputLeft < -9990)
        {
            outputLeft = -9990;
        }

        int16_t outputRightInt16 = static_cast<int16_t>(outputRight);
        DATA[0] = outputRightInt16 >> 8;   // MSB
        DATA[1] = outputRightInt16 & 0xFF; // LSB

        int16_t outputLeftInt16 = static_cast<int16_t>(outputLeft);
        DATA[2] = outputLeftInt16 >> 8;   // MSB
        DATA[3] = outputLeftInt16 & 0xFF; // LSB

        CANMessage msg0(0x200, DATA, 8);
        can1.write(msg0);
        penguin.send();
    }
}
