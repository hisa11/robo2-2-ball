#ifndef PID_HPP
#define PID_HPP

#include "mbed.h"

class PID {
public:
    PID(float kp, float ki, float kd, float rate_suppression_gain, float sample_acceleration, float sample_time, int maximum_clamp_change_rate, int max_change_rate);
    float calculate(int set_speed, int now_speed);
    void setTunings(float kp, float ki, float kd);
    void setSampleTime(float sample_time);

private:
    float _kp;
    float _ki;
    float _kd;
    float _sample_time;
    float _rate_suppression_gain;
    float _sample_acceleration;
    int _maximum_clamp_change_rate;
    int _max_change_rate;
    float _last_input;
    float _integral;
    float _last_output;
    float _last_error;
    float _last_rate;
    float _acceleration;
    Timer _timer;
};

#endif // PID_HPP
