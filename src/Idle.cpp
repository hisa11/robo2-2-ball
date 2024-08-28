#include "Idle.hpp"
#include "mbed.h"

// 前回の速度、目標速度、アンペアの値を保持するための変数
extern int16_t currentSpeed; //実際の速度
extern int16_t currentSpeed1;
extern int targetSpeedRight; //目標速度
extern int targetSpeedLeft;
extern int16_t currentAnpere; //実際のアンペア
extern int16_t currentAnpere1;

int currentSpeed_b = 0; //前回の速度
int currentSpeed1_b = 0;
int targetSpeedRight_b = 0; // 前回の目標速度
int targetSpeedLeft_b = 0;
int currentAnpere_b = 0; // 前回のアンペア
int currentAnpere1_b = 0;

Timer idleTimer;
int idleTimeThreshold = 2000; // 2秒間の空転で検知

void Idle() {
    // タイマをリセット
    idleTimer.reset();
    idleTimer.start();

    // 前進時の空転検知
    if(currentSpeed > 0 && targetSpeedRight > 0 && currentSpeed1 > 0 && targetSpeedLeft > 0) {
        if ((currentSpeed - currentSpeed_b > 10) && (currentAnpere - currentAnpere_b < 5) && 
            (targetSpeedRight - targetSpeedRight_b < 5) && (targetSpeedLeft - targetSpeedLeft_b < 5)) {
            if(idleTimer.read_ms() > idleTimeThreshold) {
                printf("空転検知\n");
                idleTimer.reset();
            }
        } else if (abs(currentAnpere - currentAnpere1) > 10) {
            printf("空転検知\n");
        }
    }
    // 逆転時の空転検知
    else if(currentSpeed < 0 && targetSpeedRight < 0 && currentSpeed1 < 0 && targetSpeedLeft < 0) {
        if ((currentSpeed - currentSpeed_b < 10) || (currentAnpere - currentAnpere_b > 5) || 
            (targetSpeedRight - targetSpeedRight_b > 5) || (targetSpeedLeft - targetSpeedLeft_b > 5)) {
            if(idleTimer.read_ms() > idleTimeThreshold) {
                printf("逆転版空転検知\n");
                idleTimer.reset();
            }
        } else if (abs(currentAnpere - currentAnpere1) <= 10) {
            printf("逆転版空転検知\n");
        }
    }
    
    // 前回の値を更新
    currentSpeed_b = currentSpeed;
    currentSpeed1_b = currentSpeed1;
    targetSpeedRight_b = targetSpeedRight;
    targetSpeedLeft_b = targetSpeedLeft;
    currentAnpere_b = currentAnpere;
    currentAnpere1_b = currentAnpere1;
}
