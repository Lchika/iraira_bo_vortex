/**
 * @file iraira_vortex_led.ino
 * @brief イライラ棒渦巻コースLED制御プログラム
 * @date 2018.09.29
 * @details
 */

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>

#define PIN_LED_START               2       //  入口のLED
#define PIN_LED_GOAL                4       //  出口のLED
#define PIN_SPI_SS                  10
#define PIN_SPI_SCK                 11
#define PIN_SPI_MISO                12
#define PIN_SPI_MOSI                13

#define NUM_LEDS_START  5       //  入口のLEDの数
#define NUM_LEDS_GOAL   5       //  出口のLEDの数

#define DEF_COLOR_CHANGE_INTERVAL   1000    //  [ms]

enum MAIN_ARDUINO_COMM_E {
  MAIN_ARDUINO_COMM_NONE = 0,
  MAIN_ARDUINO_COMM_START,
  MAIN_ARDUINO_COMM_CENTER,
  MAIN_ARDUINO_COMM_GOAL,
  MAIN_ARDUINO_COMM_TOUCH,
  MAIN_ARDUINO_COMM_SCORE,
};

static bool lf_light_pixels_in_order(Adafruit_NeoPixel *pixels_s, Adafruit_NeoPixel *pixels_g, int color[3], int delay_time);
static bool lf_light_pixels_in_rev_order(Adafruit_NeoPixel *pixels, int color[3], int delay_time);
static bool lf_change_pixels_color_in_order(Adafruit_NeoPixel *pixels, int color[3], int delay_time);
static bool lf_light_pixels_color_blink(Adafruit_NeoPixel *pixels_s, Adafruit_NeoPixel *pixels_g, int color[3]);
static bool lf_light_pixels_color_alter(Adafruit_NeoPixel *pixels_s, Adafruit_NeoPixel *pixels_g,
                                        int delay_time, int num_side_change, int num_color_change);
static bool lf_convert_time_to_color_info(unsigned long now_time, int ret_range, int color[3]);

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
  int led_color[3] = {0, 0, 0};

  unsigned long now_time = millis();
  switch(state){
    case MAIN_ARDUINO_COMM_NONE:
      //  何もしない
      break;
    case MAIN_ARDUINO_COMM_START:
      //  START状態
      Serial.println("state = START");
      lf_convert_time_to_color_info(now_time, DEF_COLOR_CHANGE_INTERVAL, led_color);
      lf_light_pixels_in_order(leds_s, leds_g, led_color, 50);
      break;
    case MAIN_ARDUINO_COMM_CENTER:
      //  CENTER状態
      Serial.println("state = CENTER");
      lf_convert_time_to_color_info(now_time, DEF_COLOR_CHANGE_INTERVAL, led_color);
      lf_light_pixels_color_blink(leds_s, leds_g, led_color);
      delay(100);
      lf_convert_time_to_color_info(now_time + (unsigned long)(DEF_COLOR_CHANGE_INTERVAL * 0.75),
                                    DEF_COLOR_CHANGE_INTERVAL, led_color);
      lf_light_pixels_color_blink(leds_s, leds_g, led_color);
      delay(100);
      break;
    case MAIN_ARDUINO_COMM_GOAL:
      //  GOAL状態
      Serial.println("state = GOAL");
      lf_light_pixels_color_alter(leds_s, leds_g, 50, 2, 5);
      state = MAIN_ARDUINO_COMM_NONE;
      break;
    default:
      Serial.println("state is invalid value.[" + String(state) + "]");
      state = MAIN_ARDUINO_COMM_NONE;
      break;
  }

  return;
}

/**
 * @fn NeoPixel順番点灯処理
 * @brief
 * @param[in] pixels_s        入口NeoPixelのポインタ
 *            pixels_g        出口NeoPixelのポインタ
 *            color[3]        表示する色
 *            daley_time      表示間隔[ms]
 * @return None
 * @detail
 */
static bool lf_light_pixels_in_order(Adafruit_NeoPixel *pixels_s, Adafruit_NeoPixel *pixels_g,
                                      int color[3], int delay_time){

  for(uint16_t i = 0; i < pixels_s->numPixels(); i++){
    //  すべてのLEDを消灯してから処理対象LEDのみ指定色で上書きする
    pixels_s->clear();
    pixels_g->clear();
    pixels_s->setPixelColor(i, pixels_s->Color(color[0], color[1], color[2]));
    pixels_g->setPixelColor(i, pixels_g->Color(color[0], color[1], color[2]));
    pixels_s->show();
    pixels_g->show();
    delay(delay_time);
  }
  return true;
}

/**
 * @fn NeoPixel逆順番点灯処理
 * @brief
 * @param[in] pixels          NeoPixelのポインタ
 *            color[3]        表示する色
 *            daley_time      表示間隔[ms]
 * @return None
 * @detail
 */
static bool lf_light_pixels_in_rev_order(Adafruit_NeoPixel *pixels, int color[3], int delay_time){

  for(uint16_t i = pixels->numPixels() - 1; i >= 0; i--){
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
 * @fn NeoPixel色全点灯処理
 * @brief
 * @param[in] pixels_s        入口NeoPixelのポインタ
 *            pixels_g        出口NeoPicelのポインタ
 *            color[3]        表示する色
 * @return None
 * @detail
 */
static bool lf_light_pixels_color_blink(Adafruit_NeoPixel *pixels_s, Adafruit_NeoPixel *pixels_g, int color[3]){

  for(uint16_t i = 0; i < pixels_s->numPixels(); i++){
    pixels_s->setPixelColor(i, pixels_s->Color(color[0], color[1], color[2]));
    pixels_g->setPixelColor(i, pixels_g->Color(color[0], color[1], color[2]));
  }
  pixels_s->show();
  pixels_g->show();
  return true;
}

/**
 * @fn NeoPixel色交互点灯処理
 * @brief
 * @param[in] pixels_s          入口NeoPixelのポインタ
 *            pixels_g          出口NeoPicelのポインタ
 *            delay_time        色切り替わり時間[ms]
 *            num_side_change   横色切り替わり回数
 *            num_color_change  基準色切り替わり回数
 * @return None
 * @detail
 */
static bool lf_light_pixels_color_alter(Adafruit_NeoPixel *pixels_s, Adafruit_NeoPixel *pixels_g,
                                        int delay_time, int num_side_change, int num_color_change){

  int led_color1[3] = {0, 0, 0};
  int led_color2[3] = {0, 0, 0};
  
  for(int color_change_count = 0; color_change_count <= num_color_change; color_change_count++){
    unsigned long now_time = millis();
    lf_convert_time_to_color_info(now_time, DEF_COLOR_CHANGE_INTERVAL, led_color1);
    lf_convert_time_to_color_info(now_time + (unsigned long)(0.75 * DEF_COLOR_CHANGE_INTERVAL),
                                  DEF_COLOR_CHANGE_INTERVAL, led_color2);

    int comp_num = -1;
    for(int side_change_count = 0; side_change_count <= num_side_change; side_change_count++){ 
      for(uint16_t i = 0; i < pixels_s->numPixels(); i++){
        if(((i % 2) * 2) == (comp_num + 1)){
          pixels_s->setPixelColor(i, pixels_s->Color(led_color1[0], led_color1[1], led_color1[2]));
          pixels_g->setPixelColor(i, pixels_g->Color(led_color1[0], led_color1[1], led_color1[2]));
        }else{
          pixels_s->setPixelColor(i, pixels_s->Color(led_color2[0], led_color2[1], led_color2[2]));
          pixels_g->setPixelColor(i, pixels_g->Color(led_color2[0], led_color2[1], led_color2[2]));
        }
      }
      pixels_s->show();
      pixels_g->show();
      comp_num *= -1;
      delay(delay_time);
    }
  }
  return true;
}

/**
 * @fn 時間→虹色情報変換処理
 * @brief
 * @param[in] now_time           時間[ms]
 * @param[out] color[3]      虹色情報
 * @return None
 * @detail
 */
static bool lf_convert_time_to_color_info(unsigned long now_time, int ret_range, int color[3]){

  static const double pi = 3.141592653589793;
  int f_mills = now_time % (int)(ret_range * 1.5);
  //  red
  double red = 0.;
  if(f_mills > 0 && f_mills < ret_range){
    red = sin(((double)f_mills / (double)ret_range) * pi);
  }
  double green = 0.;
  if(f_mills > (0.5 * ret_range) && f_mills < (1.5 * ret_range)){
    green = sin((((double)f_mills / (double)ret_range) - 0.5) * pi);
  }
  double blue = 0.;
  if(f_mills < (0.5 * ret_range)){
    blue = sin((((double)f_mills / (double)ret_range) + 0.5) * pi);
  }else if(f_mills > ret_range){
    blue = sin((((double)f_mills / (double)ret_range) - 1.0) * pi);
  }
  color[0] = (int)(255 * red);
  color[1] = (int)(255 * green);
  color[2] = (int)(255 * blue);
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