#pragma once

//
// Created by ケダルマ・ファンタスティック(KEDARUMA FANTASTIC) on 2018/02/10.
// Licensed under MIT license
//

#include <functional>
#include <HardwareSerial.h>

class HackSerial: public HardwareSerial {
public:
  std::function<size_t(uint8_t)> on_write = nullptr;

  HackSerial(int uart_nr): HardwareSerial(uart_nr) {}
  int available(void);
  int peek(void);
  int read(void);
  void flush(void);
  size_t write(uint8_t ch);
#ifdef ESP32
  size_t write(const uint8_t *buffer, size_t size);
#endif
  void set_input_buf(const String& str);

private:
  String serial_input_hack_buf_ = "";
};

#if defined(USE_GLOBAL_HACK_SERIAL)
extern HackSerial HKSerial;
#define Serial HKSerial
#endif
#if defined(USE_GLOBAL_HACK_SERIAL1)
extern HackSerial HKSerial1;
#define Serial1 HKSerial1
#endif
