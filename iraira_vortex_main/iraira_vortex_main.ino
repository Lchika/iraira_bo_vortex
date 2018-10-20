/**
 * @file iraira_vortex_main.ino
 * @brief イライラ棒渦巻コース制御プログラム
 * @date 2018.10.13
 * @details
 */

#include <StandardCplusplus.h>
#include <vector>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include "position_detecter.hpp"
#include <SPI.h>
#include <Arduino.h>

//  ピンアサイン
#define PIN_SOFT_RX1              2
#define PIN_SOFT_TX1              3
#define PIN_SOFT_RX2              4
#define PIN_SOFT_TX2              5
#define PIN_SPI_SS                10
#define PIN_PHOTO_INT_S           14
#define PIN_PHOTO_INT_C           15
#define PIN_PHOTO_INT_G           16
#define PIN_COURSE_LEVEL          A7

#define DEF_COURSE_TOUCHED_LEVEL  512

enum LED_ARDUINO_COMM_E {
  LED_ARDUINO_COMM_NONE = 0,
  LED_ARDUINO_COMM_START,
  LED_ARDUINO_COMM_CENTER,
  LED_ARDUINO_COMM_GOAL,
  LED_ARDUINO_COMM_TOUCH,
  LED_ARDUINO_COMM_SCORE,
};

//  関数定義
static bool lf_get_through_signal(int pos);
static byte lf_send_byte_by_spi(byte send_data);

//  変数定義
SoftwareSerial softwareSerial(PIN_SOFT_RX1, PIN_SOFT_TX1);    // RX, TX
SoftwareSerial softwareSerial2(PIN_SOFT_RX2, PIN_SOFT_TX2);   // RX, TX
DFRobotDFPlayerMini dFPlayer;
DFRobotDFPlayerMini dFPlayer2;
PositionDetecter positionDetecter({PIN_PHOTO_INT_S, PIN_PHOTO_INT_C, PIN_PHOTO_INT_G});


/**
 * @fn セットアップ処理
 * @brief arduino起動時のセットアップ処理を行う
 * @param None
 * @return None
 * @sa
 * @detail
 */
void setup(){

  //  ピン設定
  pinMode(PIN_COURSE_LEVEL, INPUT);
  pinMode(PIN_PHOTO_INT_S, INPUT);
  pinMode(PIN_PHOTO_INT_C, INPUT);
  pinMode(PIN_PHOTO_INT_G, INPUT);
  pinMode(PIN_SPI_SS, OUTPUT);

  //  シリアル通信開始
  softwareSerial.begin(9600);
  softwareSerial2.begin(9600);
  Serial.begin(115200);

  //  SPI設定
  SPI.setClockDivider(SPI_CLOCK_DIV128);
  SPI.begin();
  Serial.println("SPI MASTER start.");
  Serial.println("SPI clock DIV128");

  //  DFPlayer設定
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  //  DFPlayer1起動準備
  softwareSerial.listen();
  if (!dFPlayer.begin(softwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  
  //  DFPlayer2起動準備
  softwareSerial2.listen();
  if (!dFPlayer2.begin(softwareSerial2)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  //  DFPlayerの音量は10にしておく
  softwareSerial.listen();
  dFPlayer.volume(10);
  //  DFPlayer2の音量は10にしておく
  softwareSerial2.listen();
  dFPlayer2.volume(10);

  //  LED点灯パターン通知
  lf_send_byte_by_spi(LED_ARDUINO_COMM_NONE);
  
  return;
}

/**
 * @fn メインループ処理
 * @brief arduinoのメインループ処理を行う
 * @param None
 * @return None
 * @detail arduino起動後はsetup()実行後に本関数が繰り返し実行される
 */
void loop(){

  //  棒接触検知
  //  PIN_COURSE_LEVELの電圧レベルが閾値以下であれば接触判定を行う
  //  素直にデジタルピンを使えばよかったと反省
  if(analogRead(PIN_COURSE_LEVEL) <= DEF_COURSE_TOUCHED_LEVEL){
    Serial.println("course touched.");
  }
  //  棒位置検出
  int position = positionDetecter.get_position();
  switch(position){
    case -2:
      Serial.println("position NOT detected.");
      break;
    case -1:
      Serial.println("position multi detected.");
      break;
    case PIN_PHOTO_INT_S:
      Serial.println("position = start");
      //  メインBGM再生
      dFPlayer2.play(1);         //Play the first mp3
      //  実況1再生
      //dFPlayer.play(1);
      //  実況ランダム再生開始
      //  LED点灯パターン通知
      lf_send_byte_by_spi(LED_ARDUINO_COMM_START);
      break;
    case PIN_PHOTO_INT_C:
      Serial.println("position = center");
      //  実況2再生
      //dFPlayer.play(2);
      //  LED点灯パターン通知
      lf_send_byte_by_spi(LED_ARDUINO_COMM_CENTER);
      break;
    case PIN_PHOTO_INT_G:
      Serial.println("position = goal");
      //  実況3再生
      //dFPlayer.play(3);
      //  メインBGM停止
      dFPlayer2.stop();
      //  LED点灯パターン通知
      lf_send_byte_by_spi(LED_ARDUINO_COMM_GOAL);
      break;
    default:
      break;
  }

  return;
}

/**
 * @fn SPI送信処理
 * @brief
 * @param[in] send_data     送信データ
 * @return return_data      受信データ
 * @detail
 */
static byte lf_send_byte_by_spi(byte send_data){

  digitalWrite(PIN_SPI_SS, LOW);
  byte return_data = SPI.transfer(send_data);
  digitalWrite(PIN_SPI_SS, HIGH);
  return return_data;
}