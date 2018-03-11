//
// Created by ケダルマ・ファンタスティック(KEDARUMA FANTASTIC) on 2018/02/10.
// Licensed under MIT license
//

#include "HackSerial.h"

#ifndef UART0
#define UART0 0
#endif

#ifndef UART1
#define UART1 1
#endif

#if defined(USE_GLOBAL_HACK_SERIAL) && !defined(NO_GLOBAL_INSTANCES) && defined(NO_GLOBAL_SERIAL)
HackSerial HKSerial(UART0);
#endif
#if defined(USE_GLOBAL_HACK_SERIAL1) && !defined(NO_GLOBAL_INSTANCES) && defined(NO_GLOBAL_SERIAL1)
HackSerial HKSerial1(UART1);
#endif

int HackSerial::available(void) {
  if (serial_input_hack_buf_.length()) {
    return serial_input_hack_buf_.length();
  } else {
    return HardwareSerial::available();
  }
}

int HackSerial::peek(void) {
  if (serial_input_hack_buf_.length()) {
    return serial_input_hack_buf_[0];
  } else {
    return HardwareSerial::peek();
  }
}

int HackSerial::read(void) {
  int ch;
  if (serial_input_hack_buf_.length()) {
    ch = serial_input_hack_buf_[0];
    serial_input_hack_buf_.remove(0, 1);
  } else {
    ch = HardwareSerial::read();
  }
  return ch;
}

void HackSerial::flush(void) {
  serial_input_hack_buf_ = "";
  HardwareSerial::flush();
}

size_t HackSerial::write(uint8_t ch) {
  if (on_write) {
    on_write(ch);
  }
  return HardwareSerial::write(ch);
}

void HackSerial::set_input_buf(const String& str) {
  serial_input_hack_buf_ = str;
}
