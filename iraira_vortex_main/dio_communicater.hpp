#ifndef DIO_COMMUNICATER_H
#define DIO_COMMUNICATER_H

//#include <StandardCplusplus.h>
#include <ArduinoSTL.h>
#include <vector>
#include <initializer_list>
#include <Arduino.h>

class DioCommunicater {
private:
  int interval_ms;
  std::vector<int> send_pins;
  std::vector<int> receive_pins;

public:
  DioCommunicater(const std::initializer_list<int>& s_pins = {},
    const std::initializer_list<int>& r_pins = {},
    const int& interval = 100)
   : send_pins(s_pins)
   , receive_pins(r_pins)
   , interval_ms(interval)
   {};
   bool send_signal(const int& num_signal, const int& pin_suffix = 0) const;
   int receive_signal(const int& pin_suffix = 0) const;
};

#endif