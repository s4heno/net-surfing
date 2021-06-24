#include <Wire.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>
#include "Mouse.h"

/*
 * ネットサーフィン用バランスボード
 * 製作者：さへの（Twitter:＠S4heno）
 * LSM9DS1とArduino Leonardを使用することで、傾きでマウスを動かせます
 * 左クリックもできます
 * このスクリプトあるいはこのスクリプトを使用した工作で発生した、けがなどのあらゆる問題には、さへのは責任を持ちません。
 * Ver1.0.0　2021/06/06
 * このコードは、SparkFun Electronicsが作成した
 * SparkFun_LSM9DS1_Arduino_Library(ver1.0)を使用しています。
 */

LSM9DS1 imu;

//I2Cアドレスの設定
#define LSM9DS1_M	0x1C
#define LSM9DS1_AG 0x6A

const int mouseButton = 6;  //マウスボタンのピン番号（デジタル）
int mouseDelay = 30;     //マウスの移動のディレイ
int xDist = 0;              //マウスの横方向の移動距離
int yDist = 0;              //マウスの縦方向の移動距離
int xCoe = 1;               //マウス移動の係数
int yCoe = 1;               //
float pitch = 0;            //センサのピッチ角（deg）
float roll = 0;             //センサのロール角（deg）
float ax = 0;
float ay = 0;
float az = 0;
int clickState;              //マウスがクリックされているか判定

void setup()
{
  //I2C通信開始
  Wire.begin();

  //I2Cモードにして、I2Cアドレスを設定
  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;
  imu.begin();

  //ピンモード設定
  pinMode(mouseButton, INPUT_PULLUP);

  //マウス開始
  Mouse.begin();
}

void loop()
{
  //各値を取得
  imu.readGyro();
  imu.readAccel();
  imu.readMag();

  //ボタンの状態を代入
  clickState = digitalRead(mouseButton);
  
  //左クリック判定
  if (clickState == LOW) {
    // if the mouse is not pressed, press it:
    if (!Mouse.isPressed(MOUSE_LEFT)) {
      //マウスの左クリックをする
      Mouse.press(MOUSE_LEFT);
    }
  }
  else {
    // if the mouse is pressed, release it:
    if (Mouse.isPressed(MOUSE_LEFT)) {
      //マウスの左クリックを離す
      Mouse.release(MOUSE_LEFT);
    }
  }
  
  //計算
  ax = imu.ax;
  ay = imu.ay;
  az = imu.az;
  
  roll = atan2(ay, az);
  pitch = atan2(-ax, sqrt(ay * ay + az * az));

  roll  *= 180.0 / PI;
  pitch *= 180.0 / PI;
  
  //角の大きさによってマウスの移動量を決定
  yDist = roll * -1 * yCoe;
  xDist = pitch * xCoe;

  //マウスを移動
  if ((xDist != 0) || (yDist != 0)) {
    Mouse.move(xDist, yDist, 0);
  }
  delay(mouseDelay);
}
