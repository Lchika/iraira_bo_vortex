#include "dio_communicater.hpp"

bool DioCommunicater::send_signal(const int& num_signal, const int& pin_suffix) const{
  if(send_pins.size() <= pin_suffix){
    return false;
  }
  digitalWrite(send_pins[pin_suffix], HIGH);
  delay((num_signal + 0.5) * interval_ms);
  digitalWrite(send_pins[pin_suffix], LOW);
  return true;
}

int DioCommunicater::receive_signal(const int& pin_suffix) const{
  if(receive_pins.size() <= pin_suffix){
    return (-1);
  }
  while(digitalRead(receive_pins[pin_suffix]) == LOW){
    delay(interval_ms);
  }
  int receive_count = 0;
  delay(interval_ms);
  while(digitalRead(receive_pins[pin_suffix]) == HIGH){
    ++receive_count;
    delay(interval_ms);
  }
  return receive_count;
}