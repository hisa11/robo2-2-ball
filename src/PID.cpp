// PID.cpp
#include "PID.hpp"

PID::PID(float kp, float ki, float kd, float sample_time) :
    _kp(kp), _ki(ki), _kd(kd), _sample_time(sample_time),
    _last_input(0), _integral(0) {}

float PID::calculate(float setpoint, float input) {
    float error = setpoint - input;
    float p_term = _kp * error;
    _integral += error * _sample_time;
    float i_term = _ki * _integral;
    float d_term = 0;
    if (_sample_time > 0) {
        d_term = _kd * (input - _last_input) / _sample_time;
    }
    _last_input = input;
    return p_term + i_term + d_term;
}

void PID::setTunings(float kp, float ki, float kd) {
    _kp = kp;
    _ki = ki;
    _kd = kd;
}

void PID::setSampleTime(float sample_time) {
    _sample_time = sample_time;
}

void PID::reset() {
    _last_input = 0;
    _integral = 0;
}

float PID::getSampleTime() {
    return _sample_time;
}