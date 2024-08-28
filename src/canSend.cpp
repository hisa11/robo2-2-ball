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
extern int16_t currentAnpere;
extern int16_t currentAnpere1;
extern int picAngle;
extern int targetSpeedRight;
extern int targetSpeedLeft;
extern int targetpicSpeed;
extern uint8_t DATA[8];

// PID制御器のインスタンスを参照
extern PID pidControllerRight;
extern PID pidControllerLeft;
extern PID picSpeed;

// extern int8_t pic;

int16_t picAngle_d = 0;
// int16_t picAngle = 0; // Declare and initialize picAngle

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
        if (can1.read(msg1) && msg1.id == 0x201)
        {
            currentAnpere = (msg1.data[4] << 8) | msg1.data[5];
        }
        if (can1.read(msg2) && msg2.id == 0x202)
        {
            currentAnpere1 = (msg2.data[4] << 8) | msg2.data[5];
        }
        if (can1.read(msg2) && msg2.id == 0x203)
        {
            picAngle_d = (msg2.data[0] << 8) | msg2.data[1];
            int diff = picAngle_d - previous_angle;

            // 角度の範囲を正確に定義
            const int ANGLE_MAX = 8192;
            const int HALF_ANGLE = ANGLE_MAX / 2;

            if (diff > HALF_ANGLE) // ラップアラウンドの巻き戻し
            {
                picAngle -= (ANGLE_MAX - diff);
            }
            else if (diff < -HALF_ANGLE) // ラップアラウンドの巻き進み
            {
                picAngle += (ANGLE_MAX + diff);
            }
            else
            {
                picAngle += diff;
            }
            previous_angle = picAngle_d;
        }
        // printf("currentSpeed = %d , currentSpeed1 = %d\n", currentSpeed, currentSpeed1);
        printf("picAngle = %d\n", picAngle);

        float outputRight = pidControllerRight.calculate(targetSpeedRight, currentSpeed);
        float outputLeft = pidControllerLeft.calculate(targetSpeedLeft, currentSpeed1);
        float outputpic = picSpeed.calculate(targetpicSpeed, picAngle);

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

        // int16_t outputRightInt16 = static_cast<int16_t>(outputRight);
        // DATA[0] = outputRightInt16 >> 8;   // MSB
        // DATA[1] = outputRightInt16 & 0xFF; // LSB

        // int16_t outputLeftInt16 = static_cast<int16_t>(outputLeft);
        // DATA[2] = outputLeftInt16 >> 8;   // MSB
        // DATA[3] = outputLeftInt16 & 0xFF; // LSB

        CANMessage msg0(0x200, DATA, 8);
        can1.write(msg0);
        penguin.send();
    }
}
