/**************************************************************
  EEPROM_variables.cpp

  Created by KEDARUMA FANTASTIC on 2018/01/13.
  Licensed under MIT license

example:
 **************************************************************/

#define EEPROM_variables_cpp
#include <Arduino.h>
#include <assert.h>
#include <EEPROM.h>
#include "EEPROM_variables.h"

namespace EEPROM_variables {

// default values
const PROGMEM uint32_t SIGNATURE = 0xa6b1d703UL;

Payload* get_var_ptr() {
  uint8_t* dataPtr = EEPROM.getDataPtr();
  if (!dataPtr) {
    EEPROM.begin(sizeof(Payload));
    dataPtr = EEPROM.getDataPtr();
  }
  assert(dataPtr);
  auto variablePtr = reinterpret_cast<Payload*>(dataPtr);
  if (variablePtr->signature != SIGNATURE) {
    init(variablePtr);
    EEPROM.commit();
  }
  return variablePtr;
}

boolean is_initialized() {
  return EEPROM_variables::get_var_ptr()->signature == SIGNATURE;
}

void init(Payload *variablePtr) {
  variablePtr->signature = SIGNATURE;
  variablePtr->ssid[0] = 0;
  variablePtr->password[0] = 0;
}

void commit() {
  EEPROM.commit();
}

}; // namespace EEPROM_variables
