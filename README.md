# iraira_bo_vortex
 - イライラ棒渦巻コース用プログラム

## 構成図
 - ブロック図
 <img src="https://github.com/Lchika/iraira_bo_vortex/blob/images/doc/block.png" width="400px">

 - メインArduinoピンアサイン
 <img src="https://github.com/Lchika/iraira_bo_vortex/blob/images/doc/mainArduino.png" width="400px">

 - LED制御用Arduinoピンアサイン
 <img src="https://github.com/Lchika/iraira_bo_vortex/blob/images/doc/ledArduino.png" width="400px">

## iraira_vortex_main
 - メインプログラム一式

### iraira_vortex_main.ino
### position_detecter.hpp | cpp
 - フォトインタラプタによる位置検出用クラス
 - インスタンス生成時にピン番号を指定する
 - ピン番号は複数指定できる

### dio_communicater.hpp | cpp

## iraira_vortex_led
 - LED制御用プログラム一式

### iraira_vortex_led.ino