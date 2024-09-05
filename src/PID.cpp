#include "PID.hpp"
#include "mbed.h"

// PIDクラスのコンストラクタ
PID::PID(float kp, float ki, float kd, float rate_suppression_gain, float sample_acceleration, float sample_time, float maximum_clamp_change_rate, float max_change_rate)
    : _kp(kp), // 比例ゲイン
      _ki(ki), // 積分ゲイン
      _kd(kd), // 微分ゲイン
      _sample_time(sample_time), // サンプル時間
      _rate_suppression_gain(rate_suppression_gain), // 変化率抑制ゲイン
      _sample_acceleration(sample_acceleration), // サンプル加速度
      _maximum_clamp_change_rate(maximum_clamp_change_rate), // 最大クランプ変化率
      _max_change_rate(max_change_rate), // 最大変化率
      _last_input(0), // 前回の入力
      _integral(0), // 積分項の初期化
      _last_output(0), // 前回の出力
      _last_error(0), // 前回の誤差
      _last_rate(0) // 前回の変化率
{}

// PID制御の計算を行う関数
float PID::calculate(int set_speed, int now_speed)
{
    static float acceleration = 0; // 加速度の初期化
    static Timer timer; // タイマーの初期化
    static bool timer_started = false; // タイマーが開始されたかどうかのフラグ

    // タイマーが開始されていない場合、タイマーを開始
    if (!timer_started)
    {
        timer.start();
        timer_started = true;
    }

    // 現在の速度と目標速度の誤差を計算
    float error = set_speed - now_speed;
    // 比例項の計算
    float p_term = _kp * error;
    
    // 積分項の計算とアンチワインドアップ（積分項のクランプ）
    _integral += error * _sample_time;
    const float integral_max = 1000.0f; // 積分項の最大値
    const float integral_min = -1000.0f; // 積分項の最小値
    if (_integral > integral_max) _integral = integral_max;
    if (_integral < integral_min) _integral = integral_min;
    
    float i_term = _ki * _integral;
    // 微分項の計算
    float d_term = _kd * (error - _last_error) / _sample_time;

    // 一定時間ごとに加速度を更新
    if (chrono::duration<float>(timer.elapsed_time()).count() >= 0.1f)
    {
        float deltaTime = chrono::duration<float>(timer.elapsed_time()).count();
        acceleration = (now_speed - _last_input) / deltaTime;
        _last_input = now_speed;
        timer.reset();
    }

    // 変化率抑制項の計算
    float rate = p_term + i_term + d_term;
    float rate_suppression = 0.0f;
    if (rate - _last_rate + 100 != 0)
    {
        rate_suppression = _rate_suppression_gain * (acceleration * 10 / _sample_acceleration / (rate - _last_rate + 100));
    }

    // 目標出力の計算
    float desired_output = _last_output + rate - rate_suppression;

    // 出力のクランプ（最大値・最小値の制限）
    const float output_max = _maximum_clamp_change_rate; // 出力の最大値
    const float output_min = -_maximum_clamp_change_rate; // 出力の最小値
    if (desired_output > output_max) {
        desired_output = output_max;
        _integral = 0; // アンチワインドアップのため積分項をリセット
    }
    if (desired_output < output_min) {
        desired_output = output_min;
        _integral = 0; // アンチワインドアップのため積分項をリセット
    }

    // 出力の変化率を制限
    const float max_change_rate = _max_change_rate; // 出力の変化率の最大値
    float output = _last_output;
    if (desired_output > _last_output + max_change_rate) {
        output = _last_output + max_change_rate;
    } else if (desired_output < _last_output - max_change_rate) {
        output = _last_output - max_change_rate;
    } else {
        output = desired_output;
    }

    // デバッグ出力（コメントアウト）
    // printf("P: %f, I: %f, D: %f, Rate: %f, Rate Suppression: %f, Desired Output: %f, Output: %f\n", p_term, i_term, d_term, rate, rate_suppression, desired_output, output);

    // 前回の誤差、出力、変化率を更新
    _last_error = error;
    _last_output = output;
    _last_rate = rate;

    // デバッグ出力（コメントアウト）
    // printf("output = %f\n", output);
    return output;
}

// PIDゲインを設定する関数
void PID::setTunings(float kp, float ki, float kd)
{
    _kp = kp;
    _ki = ki;
    _kd = kd;
}

// サンプル時間を設定する関数
void PID::setSampleTime(float sample_time)
{
    _sample_time = sample_time;
}