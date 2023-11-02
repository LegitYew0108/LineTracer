/* mbed Microcontroller Library
 * Copyright (c) 2023 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
//ピン宣言
//右モーター
PwmOut rightForward(PA_8);
PwmOut rightBack(PB_0);
//左モーター
PwmOut leftForward(PF_0);
PwmOut leftBack(PA_11);
//フォトダイオード
AnalogIn right_sensor(PA_0);
AnalogIn midRight_sensor(PA_1);
AnalogIn midLeft_sensor(PA_3);
AnalogIn left_sensor(PA_4);

//変数定義
const int sensor_Num = 4;
float sensors[sensor_Num];
//各フォトダイオードの閾値
const float zeros[sensor_Num] = {0.0021f,0.0019f,0.017f,0.0034f};
//PIDの定数
const float Kp1 = 0.0f;
const float Kp2 = 0.0f;
const float Ki1 = 0.0f;
const float Ki2 = 0.0f;
const float Kd1 = 0.0f;
const float Kd2 = 0.0f;
//センサー値の合計
float Totals[sensor_Num];
float Prevs[sensor_Num];
float Sensor_Calc(float,float,float,float,float,float);
void MotorMotion(float,float,float);
float RightSensor;
float LeftSensor;
const float motorSpeed = 0.3f;

int main()
{
    float Totals[sensor_Num] = {0.0f,0.0f,0.0f,0.0f};
    float Prevs[sensor_Num] = {0.0f,0.0f,0.0f,0.0f};
    //PWMの初期設定
    rightForward.period_us(50);
    rightBack.period_us(50);
    leftForward.period_us(50);
    leftBack.period_us(50);
    while(1){
        sensors[0] = right_sensor.read();
        sensors[1] = midRight_sensor.read();
        sensors[2] = midLeft_sensor.read();
        sensors[3] = left_sensor.read();
        for(int kaisuu = 0;kaisuu < sensor_Num;kaisuu++){
            printf("%dのセンサーの取得値:%f",kaisuu,sensors[kaisuu]);
            sensors[kaisuu] -= zeros[kaisuu];//閾値からのずれを計算
            printf("%dのセンサー値ズレ調整後%f",kaisuu,sensors[kaisuu]);
            Totals[kaisuu] += sensors[kaisuu];//ずれの合計値計算
        }
        RightSensor = Sensor_Calc(sensors[1],sensors[0],Totals[1],Totals[0],Prevs[1],Prevs[0]);
        printf("Right:%f",RightSensor);
        LeftSensor = Sensor_Calc(sensors[2],sensors[3],Totals[2],Totals[3],Prevs[2],Prevs[3]);
        printf("Left:%f",LeftSensor);
        MotorMotion(RightSensor,LeftSensor,motorSpeed);
        //Prevsの値を設定
        for(int i;i <= sensor_Num;i++){
            Prevs[i] = sensors[i];
        }
    }
}

float Sensor_Calc(float mid,float edge,float midTotal,float edgeTotal,float midPrev,float edgePrev){
    float p = mid*Kp1 + edge*Kp2;
    float i = midTotal*Ki1 + edgeTotal*Ki2;
    float d = (mid-midPrev)*Kd1 + (edge-edgePrev)*Kd2;
    return (p+i+d);
}

void MotorMotion(float Sensor_Right,float Sensor_Left,float motorSpeed){
    if(motorSpeed<Sensor_Right){
        printf("error:pwm_right is minus");
    }
    else if(motorSpeed<Sensor_Left){
        printf("error:pwm_left is minus");
    }
    else{
        rightForward = motorSpeed - Sensor_Right;
        leftForward = motorSpeed - Sensor_Left;
    }
}