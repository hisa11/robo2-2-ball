#include "PID.hpp"
#include "mbed.h"

// PIDクラスのコンストラクタ
PID::PID(float kp, float ki, float kd, float rate_suppression_gain, float sample_acceleration, float sample_time, int maximum_clamp_change_rate, int max_change_rate,int syosoku,bool PID_Mode)
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
      _last_rate(0),  // 前回の変化率
      _acceleration(0), // 加速度の初期化
      _syosoku_a(syosoku), // 加速度の初期化
      _PID_Mode(PID_Mode) // PIDモードの初期化
{
    // タイマーの開始
    _timer.start();
}

// PID制御の計算を行う関数
float PID::calculate(int set_speed, int now_speed)
{
    // 現在の速度と目標速度の誤差を計算
    int error = set_speed - now_speed;
    if(set_speed < 0){
        _syosoku = -_syosoku_a;
    }
    else if(set_speed > 0){
        _syosoku = _syosoku_a;
    }else{
        _syosoku = 0;
    }

    // 比例項の計算
    int p_term = _kp * error;
    
    // 積分項の計算とアンチワインドアップ（積分項のクランプ）
    _integral += error * _sample_time;
    const float integral_max = 1000.0f; // 積分項の最大値
    const float integral_min = -1000.0f; // 積分項の最小値
    if (_integral > integral_max) _integral = integral_max;
    if (_integral < integral_min) _integral = integral_min;
    
    int i_term = _ki * _integral;

    // 微分項の計算
    int d_term = _kd * (error - _last_error) / _sample_time;

    // タイマーを利用して加速度を計算
    float elapsed_time = _timer.read();
    if (elapsed_time >= _sample_time)
    {
        _acceleration = (now_speed - _last_input) / elapsed_time;
        _last_input = now_speed;
        _timer.reset();
    }

    // 変化率抑制項の計算
    int rate = p_term + i_term + d_term;
    float rate_suppression = 0.0f;
    if (rate - _last_rate != 0)
    {
        rate_suppression = _rate_suppression_gain * (_acceleration * 10 / _sample_acceleration / (rate - _last_rate));
    }

    // 目標出力の計算
    int desired_output = _PID_Mode*_last_output + rate - rate_suppression+ _syosoku;

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
    int max_change_rate = _max_change_rate; // 出力の変化率の最大値
    if(abs(now_speed) <= 4000 ){
        max_change_rate = max_change_rate / 2;
    }
    int output = _last_output;
    if (desired_output > _last_output + max_change_rate) {
        output = _last_output + max_change_rate;
    } else if (desired_output < _last_output - max_change_rate) {
        output = _last_output - max_change_rate;
    } else {
        output = desired_output;
    }

    // 速度が上がらない場合の出力調整
    if (output > _last_output && now_speed < _last_input) {
        output -= max_change_rate; // 出力を減少させる
    }

    // 前回の誤差、出力、変化率を更新
    _last_error = error;
    _last_output = output;
    _last_rate = rate;

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
