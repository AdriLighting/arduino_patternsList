/*

*/

#include "main.h"

#include <arduinoPatternList.h>
#include <Arduino.h>
#include <ArduinoJson.h>

Program* _Program = nullptr;

void _Program_cb(const String itemBaseName, const uint16_t& itemBasePos, boolean updWebserver);
void setup() {
  Serial.begin(115200);

  for (unsigned long const serialBeginTime = millis(); !Serial && (millis() - serialBeginTime > 5000); ) {}
  delay(300);

  Serial.println(F("\n##################################################\n"));

  Serial.printf_P(PSTR("\t[freeheap: %d]\n"), ESP.getFreeHeap());

  Serial.setDebugOutput(false);

  boolean fs = false;
  #ifdef FSOK
    fs = FILESYSTEM.begin();
  #endif

  _Program = new Program(1, fs);
  _Program->set_fs_pl(fs);  
  _Program->set_callback(_Program_cb);
  //
  // LISTEREF_ADD-------------------------------------------------------------------------------
  _Program->initialize_lb(0, "full", ARRAY_SIZE(LPALLNAMES) , LPALLNAMES);
  // LISTEREF_CREATE
  _Program->initialize(ARRAY_SIZE(LPALLNAMES), LPALLNAMES, "full", apListSortType_t::ST_BASE);
  //--------------------------------------------------------------------------------------------
  // PLAYLIST_INITIALIZE -----------------------------------------------------------------------
  uint8_t plC       = 5;
  uint8_t iC[]      = { 10,      5,        0,        0,        0      };  // nb items max
  const char* Ln[]  = { "full",  "full",   "null",   "null",   "null" };
  _Program->initialize_playlist(plC, iC, Ln);
  #ifdef FSOK
    _Program->pl_fs_restore();
  #endif
  //
  //--------------------------------------------------------------------------------------------
  // SETUP PROGRAMM LOOP -----------------------------------------------------------------------
  _Program->remote_action(apSetter_t::APSET_LSET_PL, "0");
  _Program->remote_action(apSetter_t::APSET_PLAY_LB);
  _Program->remote_action(apSetter_t::APSET_PLAY_DELAY, "10");
  _Program->remote_action(apSetter_t::APSET_PLAY_STOP);
  //--------------------------------------------------------------------------------------------
}


void loop() {
  #ifdef DEBUG_KEYBOARD
    _Sr_menu.serialRead();
  #endif
  _Program->handle();
}


void _Program_cb(const String itemBaseName, const uint16_t& itemBasePos, boolean updWebserver) {
  Serial.printf_P(PSTR("[user_callback]\n\t[%d] %s\n"), itemBasePos, itemBaseName.c_str());
}