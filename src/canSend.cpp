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
extern int outputpic;
extern int joystickAngle;
extern uint8_t DATA[8];

// PID制御器のインスタンスを参照
extern PID pidControllerRight;
extern PID pidControllerLeft;
extern PID picSpeed;

// extern int8_t pic;

int16_t picAngle_d = 0;
int16_t picSpeed_d = 0;
// int16_t picAngle = 0; // Declare and initialize picAngle
int tireAngle = 0;

void CANRead()
{
    CANMessage msg;
    while (1)
    {
        if (can1.read(msg))
        {
            switch (msg.id)
            {
            case 0x201:
                currentSpeed = (msg.data[2] << 8) | msg.data[3];
                // printf("currentSpeed = %d\n", currentSpeed);
                break;
            case 0x202:
                currentSpeed1 = (msg.data[2] << 8) | msg.data[3];
                break;
            case 0x203:
                picSpeed_d = (msg.data[2] << 8) | msg.data[3];
                break;
            case 0x204:
                picAngle_d = (msg.data[2] << 8) | msg.data[3];
                break;
            // case 0x204:
            //     picAngle_d = (msg.data[0] << 8) | msg.data[1];
            //     int diff = picAngle_d - previous_angle;

            //     // 角度の範囲を正確に定義
            //     const int ANGLE_MAX = 8192;
            //     const int HALF_ANGLE = ANGLE_MAX / 2;

            //     if (diff > HALF_ANGLE) // ラップアラウンドの巻き戻し
            //     {
            //         picAngle -= (ANGLE_MAX - diff);
            //     }
            //     else if (diff < -HALF_ANGLE) // ラップアラウンドの巻き進み
            //     {
            //         picAngle += (ANGLE_MAX + diff);
            //     }
            //     previous_angle = picAngle_d;
            //     break;
            default:
                break;
            }
        }
        if(can2.read(msg))
        {
            penguin.read(msg);
            tireAngle=penguin.receive[0].enc;
        }
        // printf("currentSpeed = %d, currentSpeed1 = %d, picSpeed_d = %d\n", currentSpeed, currentSpeed1, picSpeed_d);
    }
}

void CANSend()
{
    while (1)
    {
        float outputRight = pidControllerRight.calculate(targetSpeedRight, currentSpeed);
        // printf("outputRight = %f, nowSpeed = %f\n", outputRight, currentSpeed);
        float outputLeft = pidControllerLeft.calculate(targetSpeedLeft, currentSpeed1);
        float outputpic = picSpeed.calculate(0, picSpeed_d);
        float outputTire = pidControllerLeft.calculate(joystickAngle, tireAngle);
        // printf("outputpic = %f\n", outputpic);
        // printf("outputRight = %f, outputLeft = %f, outputpic = %f\n", outputRight, outputLeft, outputpic);

        if (outputRight > 11000)
        {
            outputRight = 11000;
        }
        if (outputRight < -11000)
        {
            outputRight = -11000;
        }
        if (outputLeft > 11000)
        {
            outputLeft = 11000;
        }
        if (outputLeft < -11000)
        {
            outputLeft = -11000;
        }
        // if(outputpic > 11000)
        // {
        //     outputpic = 11000;
        // }
        // printf("picSpeed_d = %d, outputpic = %f\n", picSpeed_d, static_cast<double>(outputpic));
        if (abs(targetSpeedRight) == abs(targetSpeedLeft))
        {
            if (abs(currentSpeed) - abs(currentSpeed1) > 1)
            {
                outputLeft = outputLeft + outputLeft * 0.04;
            }
            else if (abs(currentSpeed) - abs(currentSpeed1) < -1)
            {
                outputRight = outputRight + outputRight * 0.04;
            }
        }

        if (outputRight > 16300)
        {
            outputRight = 16300;
        }
        if (outputRight < -16300)
        {
            outputRight = -16300;
        }
        if (outputLeft > 16300)
        {
            outputLeft = 16300;
        }
        if (outputLeft < -16300)
        {
            outputLeft = -16300;
        }
        int16_t outputRightInt16 = static_cast<int16_t>(outputRight);
        DATA[0] = outputRightInt16 >> 8;   // MSB
        DATA[1] = outputRightInt16 & 0xFF; // LSB

        int16_t outputLeftInt16 = static_cast<int16_t>(outputLeft);
        DATA[2] = outputLeftInt16 >> 8;   // MSB
        DATA[3] = outputLeftInt16 & 0xFF; // LSB

        // int16_t outputpicInt16 = static_cast<int16_t>(outputpic);
        // // printf("outputpic = %d\n", outputpicInt16);
        // DATA[3] = outputpicInt16 >> 8;   // MSB
        // DATA[4] = outputpicInt16 & 0xFF; // LSB

        CANMessage msg0(0x200, DATA, 8);
        can1.write(msg0);
        penguin.send();
        // ThisThread::sleep_for(10ms);
    }
}
