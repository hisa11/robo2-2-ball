// includeファイル
#include "mbed.h"
#include "PID.hpp"
#include "firstpenguin.hpp"
#include "canSend.hpp"
#include "controller.hpp"

// firstpenguin_ID
constexpr uint32_t penguinID = 35;
int8_t pic = 0;
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
int outputpic = 0;

// シリアル通信
BufferedSerial pc(USBTX, USBRX, 250000);
Timer timer;

// CAN通信
CAN can1(PA_11, PA_12, (int)1e6);
CAN can2(PB_12, PB_13, (int)1e6);
FirstPenguin penguin(penguinID, can2);
uint8_t DATA[8] = {};

// PID制御
const float kp = 1.0;
const float ki = 0;
const float kd = 0;
const float rateSuppressionGain = 0.01;
const float sampleacceleration = 500;
const float sampleTime = 0.01; // 20ms sample time
const float maximumClampChangeRate = 16000;
const float maxChangeRate = 1000;

// PID制御器のインスタンスを作成
PID pidControllerRight(kp, ki, kd, rateSuppressionGain, sampleacceleration, sampleTime,maximumClampChangeRate,maxChangeRate); //P, I, D, rate_suppression_gain, sample_acceleration, sample_time, maximum_clamp_change_rate, max_change_rate
PID pidControllerLeft(kp, ki, kd, rateSuppressionGain, sampleacceleration, sampleTime,maximumClampChangeRate,maxChangeRate);
PID picSpeed(0.0001, 0.0, 0., 0.01, 500, sampleTime,1000,100);

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
        char output_buf[20];                           // 出力用のバッファを作成します
        readUntilPipe(output_buf, sizeof(output_buf)); // '|'が受け取られるまでデータを読み込みます
        processInput(output_buf);
        if (picbutton == 0)
        {
            picAngle = 0;
            printf("button is pressed\n");
        }
        if (picAngle >= 15000)
        {
            pic = 1;
        }

        if (pic == 1)
        {
            outputpic = 0;
        }
        else if (pic == 0)
        {
            outputpic = 500;
        }
        else if (pic == 2)
        {
            outputpic = -500;
            // printf("pic = 2\n");
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
    Thread thread1;
    thread1.start(picthred);
    while (1)
    {
        targetSpeedLeft = (leftJoystickY - rightJoystickX) * 7 / 12 + targetSpeedLeft_M;
        targetSpeedRight = (-leftJoystickY - rightJoystickX) * 7 / 12 + targetSpeedRight_M;
        // ThisThread::sleep_for(10ms);
    }
}