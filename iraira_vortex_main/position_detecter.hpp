#ifndef POSITION_DETECTER_H
#define POSITION_DETECTER_H

#include <StandardCplusplus.h>
#include <vector>
#include <initializer_list>
#include <Arduino.h>

class PositionDetecter {
private:
  std::vector<int> pins;
  std::vector<bool> pre_state;

public:
  PositionDetecter(std::initializer_list<int> input_pins)
   : pins(input_pins)
   , pre_state({false, false, false})
   {};
  int get_position(void);
  void reset_state(void);
};

#endif