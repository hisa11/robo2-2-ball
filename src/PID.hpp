#ifndef PID_HPP
#define PID_HPP

#include "mbed.h"

class PID {
public:
    PID(float kp, float ki, float kd, float sample_time);
    float calculate(float setpoint, float input);
    void setTunings(float kp, float ki, float kd);
    void setSampleTime(float sample_time);
    void reset();
    float getSampleTime();
private:
    float _kp;
    float _ki;
    float _kd;
    float _sample_time;
    float _last_input;
    float _integral;
    float _last_output;  // 前回の出力を保持するための変数
    float _last_error;   // 前回の誤差を保持するための変数
};

#endif // PID_HPP
