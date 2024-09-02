// PID.cpp
#include "PID.hpp"

PID::PID(float kp, float ki, float kd, float sample_time) :
    _kp(kp), _ki(ki), _kd(kd), _sample_time(sample_time),
    _last_input(0), _integral(0), _last_output(0) {}

float PID::calculate(float setpoint, float input) {
    float error = setpoint - input;
    float p_term = _kp * (error - _last_error);
    _integral += error * _sample_time;
    float i_term = _ki * _integral;
    float d_term = -_kd * (input - _last_input);
    
    float output = _last_output + p_term + i_term + d_term;
    
    // Update the state for the next iteration
    _last_input = input;
    _last_error = error;
    _last_output = output;
    
    return output;
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
    _last_output = 0;
    _last_error = 0;
}

float PID::getSampleTime() {
    return _sample_time;
}
