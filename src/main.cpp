// includeファイル
#include "mbed.h"
#include "PID.hpp"
#include "firstpenguin.hpp"
#include "canSend.hpp"
#include "controller.hpp"

// firstpenguin_ID
constexpr uint32_t penguinID = 35;
int8_t pic =1; 
DigitalIn picbutton(PC_1);

// 変数宣言
// int leftJoystickX = 0;
// int leftJoystickY = 0;
// int rightJoystickX = 0;
int16_t currentSpeed = 0;
int16_t currentSpeed1 = 0;
int16_t currentAnpere = 0;
int16_t currentAnpere1 = 0;
int picAngle = 0;
int targetSpeedRight = 0;
int targetSpeedLeft = 0;
int targetpicSpeed = 0;

// シリアル通信
BufferedSerial pc(USBTX, USBRX, 250000);
Timer timer;

// CAN通信
CAN can1(PA_11, PA_12, (int)1e6);
CAN can2(PB_12, PB_13, (int)1e6);
FirstPenguin penguin(penguinID, can2);
uint8_t DATA[8] = {};

// PID制御
const float kp = 0.4;
const float ki = 0.5;
const float kd = 0.07/8;
const float sampleTime = 0.01; // 20ms sample time

// PID制御器のインスタンスを作成
PID pidControllerRight(kp, ki, kd, sampleTime);
PID pidControllerLeft(kp, ki, kd, sampleTime);
PID picSpeed(kp, ki, kd, sampleTime);

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
void picthred(){
    while (1)
    {
        if(picbutton == 0)
        {
            picAngle = 0;
            printf("button is pressed\n");
        }
        if (picAngle <= 0 || picAngle >= 15000)
        {
            pic = 1;
        }
        
        if (pic == 1)
        {
            targetpicSpeed = 0;
        }
        else if (pic == 0)
        {
            targetpicSpeed = -2000;
        }
        else if(pic == 2)
        {
            targetpicSpeed = 2000;
        }
    }
    
}

int main()
{
    picbutton.mode(PullUp);
    char output_buf[20]; // 出力用のバッファを作成します
    Thread thread;
    thread.start(canSend); // canSendスレッドを開始
    Thread thread1;
    thread1.start(picthred);
    while (1)
    {
        readUntilPipe(output_buf, sizeof(output_buf)); // '|'が受け取られるまでデータを読み込みます
        processInput(output_buf);

        targetSpeedLeft = (leftJoystickY - rightJoystickX) * 7 / 12;
        targetSpeedRight = (-leftJoystickY - rightJoystickX) * 7 / 12;
        
        
    }
}