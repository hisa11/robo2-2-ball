#ifndef PID_HPP
#define PID_HPP

class PID {
public:
    PID(float kp, float ki, float kd, float rate_suppression_gain, float sample_acceleration, float sample_time , float Maximum_clamp_change_rate , float max_change_rate);//kp, ki, kd, rate_suppression_gain, sample_acceleration, sample_time
    float calculate(int set_speed, int now_speed);
    void setTunings(float kp, float ki, float kd);
    void setSampleTime(float sample_time);
    void setRateSuppressionGain(float rate_suppression_gain);
    void reset();
    float getAcceleration() const; // 追加
    void resetIntegral(); // 追加

private:
    float _kp;
    float _ki;
    float _kd;
    float _sample_time;
    float _rate_suppression_gain;
    float _sample_acceleration;
    float _maximum_clamp_change_rate;
    float _max_change_rate;
    float _last_input;
    float _integral;
    float _last_output;
    float _last_error;
    float _last_rate;
    float _acceleration; // 追加
};

#endif // PID_HPP