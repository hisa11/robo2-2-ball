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
extern bool tire_previous;
extern bool start;

// PID制御器のインスタンスを参照
extern PID pidControllerRight;
extern PID pidControllerLeft;
extern PID picSpeed;
extern PID rotation;

// extern int8_t pic;
int outputRight = 0;
int outputLeft = 0;

int16_t picAngle_d = 0;
int16_t picSpeed_d = 0;
// int16_t picAngle = 0; // Declare and initialize picAngle
int tireAngle = 0;

void CANRead()
{
    CANMessage msg;
    CANMessage msg1;
    while (1)
    {
        if (can1.read(msg))
        {
            // printf("kitayo\n");、
            switch (msg.id)
            {
            case 0x201:
                currentSpeed = (msg.data[2] << 8) | msg.data[3];
                break;
            case 0x202:
                currentSpeed1 = (msg.data[2] << 8) | msg.data[3];
                break;
            case 0x203:
                picSpeed_d = (msg.data[2] << 8) | msg.data[3];
                break;
            case 0x204:
                picAngle_d = (msg.data[2] << 8) | msg.data[3];
                // printf("picAngle_d = %d\n", picAngle_d);
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
            //     }        int outputRight = pidControllerRight.calculate(targetSpeedLeft,currentSpeed1);

            //     previous_angle = picAngle_d;
            //     break;
            default:
                break;
            }
        }
        // printf("currentSpeed = %d\n", currentSpeed1);

        if (can2.read(msg1))
        {
            penguin.read(msg1);
            tireAngle = penguin.receive[0].enc;
            // printf("tireAngle = %d\n", tireAngle);
            // printf("tireAngle = %d\n", tireAngle);
        }
        printf("currentSpeed = %d, currentSpeed1 = %d, picSpeed_d = %d\n", currentSpeed, currentSpeed1, picSpeed_d);
    }
}

void PIDcalculation()
{
    while (1)
    {

        int outputRight_d = pidControllerRight.calculate(targetSpeedRight, currentSpeed);
        // printf("targetSpeedRight = %d, currentSpeed = %d\n", targetSpeedRight, currentSpeed);
        // printf("outputRight =%d,sokudo=%d\n",outputRight,currentSpeed);
        int outputLeft_d = pidControllerLeft.calculate(-targetSpeedLeft, currentSpeed1);
        // printf("outputleft = %d, targetSpeedLeft = %d,carentSpeed1= %d\n", outputLeft, targetSpeedLeft, currentSpeed1);
        if (outputRight_d > 11000)
        {
            outputRight = 11000;
        }
        else if (outputRight_d < -11000)
        {
            outputRight = -11000;
        }
        else
        {
            outputRight = outputRight_d;
        }
        if (outputLeft_d > 11000)
        {
            outputLeft = 11000;
        }
        else if (outputLeft_d < -11000)
        {
            outputLeft = -11000;
        }
        else
        {
            outputLeft = outputLeft_d;
        }
        ThisThread::sleep_for(10ms);
    }
}

void CANSend()
{
    while (1)
    {
        int outputLeft = pidControllerLeft.calculate(targetSpeedLeft, currentSpeed1);
        int outputRight = pidControllerRight.calculate(targetSpeedRight, currentSpeed);
        int outputpic = picSpeed.calculate(targetpicSpeed, picSpeed_d);
        if (tire_previous == 1)
        {
            joystickAngle = 0;
        }
        int outputTire = rotation.calculate(-joystickAngle * 2, tireAngle);
        // printf("outputTire = %f\n", outputTire);

        // 出力の制限

        // outputTireの制限
        if (outputTire > 16000)
        {
            outputTire = 16000;
        }
        if (outputTire < -16000)
        {
            outputTire = -16000;
        }
        int16_t outputRightInt16 = static_cast<int16_t>(outputRight);
        DATA[0] = outputRightInt16 >> 8;   // MSB
        DATA[1] = outputRightInt16 & 0xFF; // LSB
        // printf("outputRightInt16 = %d\n", outputRightInt16);

        int16_t outputLeftInt16 = static_cast<int16_t>(outputLeft);
        DATA[2] = outputLeftInt16 >> 8;   // MSB
        DATA[3] = outputLeftInt16 & 0xFF; // LSB

        int16_t outputpicInt16 = static_cast<int16_t>(outputpic);
        // printf("outputpicInt16 = %d\n", outputpicInt16);
        DATA[4] = outputpicInt16 >> 8;   // MSB
        DATA[5] = outputpicInt16 & 0xFF; // LSB
        // printf("outputpicInt16 = %d\n", outputpicInt16);
        // printf("outputTire = %d\n",outputTire);
        int16_t outputTireInt16 = static_cast<int16_t>(-outputTire);
        DATA[6] = outputTireInt16 >> 8;   // MSB
        DATA[7] = outputTireInt16 & 0xFF; // LSB
        // printf("outputTireInt16 = %d,joustic =%d\n", outputTireInt16,joystickAngle);

        CANMessage msg0(0x200, DATA, 8);
        if (can1.write(msg0))
        {
        }
        else
        {
            // printf("muri\n");
        }
        penguin.send();
        ThisThread::sleep_for(10ms);
    }
}