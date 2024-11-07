// main.cpp
//  includeファイル
#include "mbed.h"
#include "PID.hpp"
#include "firstpenguin.hpp"
#include "canSend.hpp"
#include "controller.hpp"
#include <cmath>

// firstpenguin_ID
constexpr uint32_t penguinID = 40;
int8_t pic = 1;
DigitalIn picbutton(PC_1);
DigitalIn kickswich(PC_2);

// 変数宣言
int16_t currentSpeed = 0;
int16_t currentSpeed1 = 0;
int16_t currentAnpere = 0;
int16_t currentAnpere1 = 0;
int picAngle = 0;
int targetSpeedRight = 0;
int targetSpeedLeft = 0;
int targetpicSpeed = 0;
int targetSpeedRight_M = 0;
int targetSpeedLeft_M = 0;
int outputpic = 0; // グローバル変数の定義
int leftJoystickX = 0;
int leftJoystickY = 0;
int rightJoystickX = 0;
int joystickAngle = 0;
int targetSpeed = 0;
bool pic_flag = 1;
int kick = 1;
bool kick_flag = 0;
bool kick_flug_zyunbi = 0;

// シリアル通信
BufferedSerial pc(USBTX, USBRX, 250000);
Timer timer;

// CAN通信
CAN can1(PA_11, PA_12, (int)1e6);
CAN can2(PB_12, PB_13, (int)1e6);
// CAN can1(PA_11, PA_12, (int)1e6);
FirstPenguin penguin(penguinID, can2);
uint8_t DATA[8] = {};

// PID制御
const float kp = 0.008;
const float ki = 0.011;
const float kd = 0.006;
const float rateSuppressionGain = 0.01;
const float sampleacceleration = 500;
const float sampleTime = 0.01; // 20ms sample time
const float maximumClampChangeRate = 12000.0;
const float maxChangeRate = 800;

// PID制御器のインスタンスを作成
PID pidControllerRight(kp, ki, kd, rateSuppressionGain, sampleacceleration, sampleTime, maximumClampChangeRate, maxChangeRate, 10, 1); // P, I, D, rate_suppression_gain, sample_acceleration, sample_time, maximum_clamp_change_rate, max_change_rate
PID pidControllerLeft(kp, ki, kd, rateSuppressionGain, sampleacceleration, sampleTime, maximumClampChangeRate, maxChangeRate, 10, 1);  // P, I, D, rate_suppression_gain, sample_acceleration, sample_time, maximum_clamp_change_rate, max_change_rate
PID picSpeed(0.03, 1.04, 28, 0.01, 500, sampleTime, 8000, 600, 10, 1);
PID rotation(2.0, 11.81, 0, 0, 0, sampleTime, 8000, 2500, 1500, 0);

// シリアル通信読み取りのプログラム
void readUntilPipe(char *output_buf, int output_buf_size)
{
    char buf[20];
    int output_buf_index = 0;
    while (1)
    {
        if (pc.readable())
        {
            ssize_t num = pc.read(buf, sizeof(buf) - 1); // -1 to leave space for null terminator
            buf[num] = '\0';
            for (int i = 0; i < num; i++)
            {
                if (buf[i] == '|')
                {
                    output_buf[output_buf_index] = '\0';
                    return;
                }
                else if (buf[i] != '\n' && output_buf_index < output_buf_size - 1)
                {
                    output_buf[output_buf_index++] = buf[i];
                }
            }
        }
        if (output_buf_index >= output_buf_size - 1) // Prevent buffer overflow
        {
            output_buf[output_buf_index] = '\0';
            return;
        }
    }
}

void picthred()
{
    while (1)
    {
        picbutton.mode(PullUp);

        // printf("aaa\n");
        if (pic_flag == 1)
        {
            if (picbutton == 0)
            {
                // printf("button is pressed\n");
                pic = 1;
                pic_flag = 0;
            }
        }
        else
        {
            if (picbutton == 1)
            {
                pic_flag = 1;
            }
        }
        if (pic == 1)
        {
            targetpicSpeed = 0;
        }
        else if (pic == 2)
        {
            targetpicSpeed = 800;
        }
        else if (pic == 0)
        {
            targetpicSpeed = -1300;
        }
        // printf("pic=%d\n",pic);
        if (kickswich == 0)
        {
            if (kick_flug_zyunbi == 0)
            {
                kick = 1;
            }
            kick_flug_zyunbi = 1;
        }else{
            kick_flug_zyunbi = 0;
        }
        if (kick == 0)
        {
            penguin.pwm[0] = -2500;
        }
        else if (kick == 2)
        {
            penguin.pwm[0] = 2500;
        }
        else if (kick == 1)
        {
            penguin.pwm[0] = 0;
        }
        else if (kick == 3)
        {
            penguin.pwm[0] = -23000;
            if (kickswich == 0)
            {
                if (kick_flag == 0)
                {
                    kick_flag = 1;
                    ThisThread::sleep_for(100ms);
                    printf("kickflugpass\n");
                }
                else
                {
                    kick = 1;
                    kick_flag = 0;
                    printf("kickflugstop\n");
                }
            }
        }
        // printf("kick=%d\n", kick);
        // printf("pic = %d\n",pic);
    }
}

int main()
{
    picbutton.mode(PullUp);
    kickswich.mode(PullUp);
    Thread thread2;
    thread2.start(CANRead); // CANReadスレッドを開始
    Thread thread;
    thread.start(CANSend); // CANSendスレッドを開始
    Thread thread3;
    thread3.start(picthred);
    Thread thread4;
    thread4.start(PIDcalculation);

    while (1)
    {
        // printf("aaa\n");
        char output_buf[20];                           // 出力用のバッファを作成します
        readUntilPipe(output_buf, sizeof(output_buf)); // '|'が受け取られるまでデータを読み込みます
        processInput(output_buf);

        // Calculate the angle of the joystick
        // ジョイスティックの角度を計算

        if (leftJoystickX == 0 && leftJoystickY == 0)
        {
            joystickAngle = 0;
        }
        else
        {
            joystickAngle = atan2(leftJoystickY, leftJoystickX) * 180.0f / M_PI - 90;
        }
        if (joystickAngle > 90)
        {
            joystickAngle -= 180;
        }
        else if (joystickAngle < -90)
        {
            joystickAngle += 180;
        }

        // 結果を出力

        if (leftJoystickY < 0)
        {
            targetSpeed = -(sqrt(pow(leftJoystickX, 2) + pow(-leftJoystickY, 2)));
        }
        else
        {
            targetSpeed = (sqrt(pow(leftJoystickX, 2) + pow(-leftJoystickY, 2)));
        }

        // printf("targetSpeed = %d\n", targetpicSpeed);
        // printf("pic = %d\n", pic);

        targetSpeedRight = (targetSpeed - rightJoystickX) / 2 + targetSpeedRight_M;
        targetSpeedLeft = (targetSpeed + rightJoystickX) / 2 + targetSpeedLeft_M;
        // printf("x = %d, y = %d, joystickAngle = %d,targetSpeed = %d\n", leftJoystickX, leftJoystickY, joystickAngle, targetSpeedRight);
        // targetSpeedLeft = (leftJoystickY - rightJoystickX) * 7 / 12 + targetSpeedLeft_M;
        // targetSpeedRight = (-leftJoystickY - rightJoystickX) * 7 / 12 + targetSpeedRight_M;
        // ThisThread::sleep_for(10ms);
    }
}