#pragma once

/**************************************************************
  EEPROM_variables.h

  Created by KEDARUMA FANTASTIC on 2018/01/13.
  Licensed under MIT license
**************************************************************/
#include <Arduino.h>

namespace EEPROM_variables {

#ifndef EEPROM_variables_cpp
// default values
extern const PROGMEM uint32_t SIGNATURE;
#endif

struct Payload {
  uint32_t signature;
  char ssid[32];
  char password[64];
};

Payload* get_var_ptr();
boolean is_initialized();
void init(Payload* var_ptr);
void commit();

} // namespace EEPROM_variables
