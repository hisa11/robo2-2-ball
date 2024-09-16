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
const float kp = 0.004;
const float ki = 0;
const float kd = 0.01;
const float rateSuppressionGain = 0.01;
const float sampleacceleration = 500;
const float sampleTime = 0.01; // 20ms sample time
const float maximumClampChangeRate = 12000.0;
const float maxChangeRate = 800;

// PID制御器のインスタンスを作成
PID pidControllerRight(kp + 0.005, ki, kd, rateSuppressionGain, sampleacceleration, sampleTime, maximumClampChangeRate, maxChangeRate, 10, 1); // P, I, D, rate_suppression_gain, sample_acceleration, sample_time, maximum_clamp_change_rate, max_change_rate
PID pidControllerLeft(kp, ki, kd, rateSuppressionGain, sampleacceleration, sampleTime, maximumClampChangeRate, maxChangeRate, 10, 1);          // P, I, D, rate_suppression_gain, sample_acceleration, sample_time, maximum_clamp_change_rate, max_change_rate
PID picSpeed(0.8903, 0.22, 21, 0.01, 500, sampleTime, 8000, 250, 10, 1);
PID rotation(30.0, 20.81, 2.5, 0, 0, sampleTime, 8000, 3000, 500, 0);

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
        else{
            if(picbutton == 1){
                pic_flag = 1;
            }
        }
    }
}

int main()
{

    picbutton.mode(PullUp);
    Thread thread2;
    thread2.start(CANRead); // CANReadスレッドを開始
    Thread thread;
    thread.start(CANSend); // CANSendスレッドを開始
    Thread thread3;
    thread3.start(picthred);
    while (1)
    {
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
        if (pic == 1)
        {
            targetpicSpeed = 0;
        }
        else if (pic == 2)
        {
            targetpicSpeed = 1500;
        }
        else if (pic == 0)
        {
            targetpicSpeed = -2500;
        }
        // printf("targetSpeed = %d\n", targetpicSpeed);
        printf("pic = %d\n", pic);

        targetSpeedRight = (targetSpeed - rightJoystickX) / 2 + targetSpeedRight_M;
        targetSpeedLeft = (targetSpeed + rightJoystickX) / 2 + targetSpeedLeft_M;
        // printf("x = %d, y = %d, joystickAngle = %d,targetSpeed = %d\n", leftJoystickX, leftJoystickY, joystickAngle, targetSpeedRight);
        // targetSpeedLeft = (leftJoystickY - rightJoystickX) * 7 / 12 + targetSpeedLeft_M;
        // targetSpeedRight = (-leftJoystickY - rightJoystickX) * 7 / 12 + targetSpeedRight_M;
        // ThisThread::sleep_for(10ms);
    }
}