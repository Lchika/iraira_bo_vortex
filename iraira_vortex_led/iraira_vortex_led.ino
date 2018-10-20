/**
 * @file iraira_vortex_led.ino
 * @brief イライラ棒渦巻コースLED制御プログラム
 * @date 2018.09.29
 * @details
 */

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>

#define PIN_LED_START               2       // 入口のLED
#define PIN_LED_GOAL                4       // 出口のLED
#define PIN_SPI_SS                  10
#define PIN_SPI_SCK                 11
#define PIN_SPI_MISO                12
#define PIN_SPI_MOSI                13

#define NUM_LEDS_START  5       // 入口のLEDの数
#define NUM_LEDS_GOAL   5       // 出口のLEDの数

enum MAIN_ARDUINO_COMM_E {
  MAIN_ARDUINO_COMM_NONE = 0,
  MAIN_ARDUINO_COMM_START,
  MAIN_ARDUINO_COMM_CENTER,
  MAIN_ARDUINO_COMM_GOAL,
  MAIN_ARDUINO_COMM_TOUCH,
  MAIN_ARDUINO_COMM_SCORE,
};

static bool lf_light_pixels_in_order(Adafruit_NeoPixel *pixels, int color[3], int delay_time);
static bool lf_change_pixels_color_in_order(Adafruit_NeoPixel *pixels, int color[3], int delay_time);

Adafruit_NeoPixel *leds_s;
Adafruit_NeoPixel *leds_g;
static byte state = MAIN_ARDUINO_COMM_NONE;

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
  pinMode(PIN_SPI_SS, INPUT);
  pinMode(PIN_SPI_SCK, INPUT);
  pinMode(PIN_SPI_MISO, INPUT);
  pinMode(PIN_SPI_MOSI, INPUT);

  //  シリアル通信開始
  Serial.begin(115200);
  delay(10);
  Serial.println("start.");

  //  SPI設定
  SPCR &= ~_BV(4);        //  スレーブ設定
  SPCR |= _BV(6);         //  SPI有効
  SPI.attachInterrupt();  //  SPI割り込み有効

  leds_s = new Adafruit_NeoPixel(NUM_LEDS_START, PIN_LED_START, NEO_RGB + NEO_KHZ400);
  leds_s->begin();
  leds_s->setBrightness(30);
  leds_s->clear();
  leds_s->show();
  leds_g = new Adafruit_NeoPixel(NUM_LEDS_GOAL, PIN_LED_GOAL, NEO_RGB + NEO_KHZ400);
  leds_g->begin();
  leds_g->setBrightness(30);
  leds_g->clear();
  leds_g->show();

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
  int red[3] = {150, 0, 0};
  int green[3] = {0, 150, 0};
  int blue[3] = {0, 0, 150};

  switch(state){
    case MAIN_ARDUINO_COMM_NONE:
      //  何もしない
      break;
    case MAIN_ARDUINO_COMM_START:
      //  START状態
      Serial.println("state = START");
      lf_light_pixels_in_order(leds_s, red, 100);
      break;
    case MAIN_ARDUINO_COMM_CENTER:
      //  CENTER状態
      Serial.println("state = CENTER");
      lf_light_pixels_in_order(leds_s, green, 100);
      break;
    case MAIN_ARDUINO_COMM_GOAL:
      //  GOAL状態
      Serial.println("state = GOAL");
      lf_light_pixels_in_order(leds_s, blue, 100);
      state = MAIN_ARDUINO_COMM_NONE;
      break;
    default:
      Serial.println("state is invalid value.[" + String(state) + "]");
      state = MAIN_ARDUINO_COMM_NONE;
      break;
  }

  delay(100);
  return;
}

/**
 * @fn NeoPixel順番点灯処理
 * @brief
 * @param[in] pixels          NeoPixelのポインタ
 *            color[3]        表示する色
 *            daley_time      表示間隔[ms]
 * @return None
 * @detail
 */
static bool lf_light_pixels_in_order(Adafruit_NeoPixel *pixels, int color[3], int delay_time){

  for(uint16_t i = 0; i < pixels->numPixels(); i++){
    //  すべてのLEDを消灯してから処理対象LEDのみ指定色で上書きする
    pixels->clear();
    pixels->setPixelColor(i, pixels->Color(color[0], color[1], color[2]));
    pixels->show();
    delay(delay_time);
  }
  return true;
}

/**
 * @fn NeoPixel順番色変わり処理
 * @brief
 * @param[in] pixels          NeoPixelのポインタ
 *            color[3]        表示する色
 *            daley_time      表示間隔[ms]
 * @return None
 * @detail lf_light_pixels_in_order()は同時に１つのLEDしか点灯していないが、
 *         本関数では消灯処理を行わないため前回の色がそのまま残る
 */
static bool lf_change_pixels_color_in_order(Adafruit_NeoPixel *pixels, int color[3], int delay_time){

  for(uint16_t i = 0; i < pixels->numPixels(); i++){
    pixels->setPixelColor(i, pixels->Color(color[0], color[1], color[2]));
    pixels->show();
    delay(delay_time);
  }
  return true;
}

/**
 * @fn 時間→虹色情報変換処理
 * @brief
 * @param[in] time           時間[-]
 * @param[out] color[3]      虹色情報
 * @return None
 * @detail
 */
static bool lf_convert_time_to_color_info(int time, int color[3]){
  return true;
}

/**
 * @fn SPI割り込み処理
 * @brief
 * @param[in] None
 * @param[out] None
 * @return None
 * @detail  割り込み時に呼ばれる関数
 */
ISR(SPI_STC_vect){

  //  受信データセット
  state = SPDR;
  return;
}