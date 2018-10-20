#include "position_detecter.hpp"

int PositionDetecter::get_position(void){
  //  すべてのピンについて電圧レベルを取得する
  int detect_count = 0;
  int detect_pin = 0;
  for(int i = 0; i < pins.size(); ++i){
    if(digitalRead(pins[i]) == LOW){
      if(pre_state[i] == false){
        detect_pin = pins[i];
        ++detect_count;
        pre_state[i] = true;
      }
    }else{
      pre_state[i] = false;
    }
  }
  if(detect_count == 1){
    return (detect_pin);
  }else if(detect_count == 0){
    return (-2);
  }else{
    return (-1);
  }
}

void PositionDetecter::reset_state(void){
  for(auto& state : pre_state){
    state = false;
  }
}