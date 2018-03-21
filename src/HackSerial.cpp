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

#if defined(USE_GLOBAL_HACK_SERIAL)
HackSerial Serial(UART0);
#endif
#if defined(USE_GLOBAL_HACK_SERIAL1)
HackSerial Serial1(UART1);
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

#ifdef ESP32
size_t HackSerial::write(const uint8_t *buffer, size_t size) {
  if (on_write) {
    for (int i = 0; i < size; i++) {
      on_write(buffer[i]);
    }
  }
  return HardwareSerial::write(buffer, size);
}
#endif

void HackSerial::set_input_buf(const String& str) {
  serial_input_hack_buf_ = str;
}
