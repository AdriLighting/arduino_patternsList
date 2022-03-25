/*
build_flags =
  -DAP_DEFAULT
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

  _Program = new Program(1, false);
  _Program->set_fs_pl(false);  
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
  // PLAYLIST ADD ITEMS
  _Program->pl_item_toArray(0, 255, "lbl_1", "Pépita", "iref_cfg");
  _Program->pl_item_toArray(0, 255, "lbl_2", "Réglisse(Mon RéRé)", "iref_cfg");
  _Program->pl_item_toArray(0, 255, "lbl_3", "Crevette(Pot de cole)", "iref_cfg");

  _Program->pl_item_toArray(1, 255, "lbl_1", "Philippe", "iref_cfg");
  _Program->pl_item_toArray(1, 255, "lbl_2", "Catherine", "iref_cfg");
  _Program->pl_item_toArray(1, 255, "lbl_3", "Papie", "iref_cfg");
  _Program->pl_item_toArray(1, 255, "lbl_3", "Axel", "iref_cfg");
  _Program->pl_item_toArray(1, 255, "lbl_3", "Eliza", "iref_cfg");
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