#include "main.h"

#include <arduinoPatternList.h>
#include <Arduino.h>
#include <ArduinoJson.h>

Program* _Program = nullptr;

#ifdef DEBUG_KEYBOARD
    void serial_menu_cmd(const String & cmd, const String & value);
#endif
void _Program_handleCallback(const String itemBaseName, const uint16_t& itemBasePos, boolean updWebserver);

void setup() {
  Serial.begin(115200);

  for (unsigned long const serialBeginTime = millis(); !Serial && (millis() - serialBeginTime > 5000); ) {}
  delay(300);

  Serial.println(F("\n##################################################\n"));

  Serial.printf_P(PSTR("\t[freeheap: %d]\n"), ESP.getFreeHeap());

  define_print();

  Serial.setDebugOutput(false);

  #ifdef DEBUG_KEYBOARD
    _Sr_menu.add("name_1", "!", serial_menu_cmd, SR_MM::SRMM_KEY);
  #endif

  boolean fs = false;
#ifdef FSOK
  fs = FILESYSTEM.begin();
#endif

  _Program = new Program(2, fs);
  _Program->set_fs_pl(true);
  _Program->set_callback(_Program_handleCallback);

  // LISTEREF_ADD-------------------------------------------------------------------------------
  Serial.printf_P(PSTR(">>>>%d<<<\n"), ESP.getFreeHeap());
  _Program->initialize_lb(0, "full", ARRAY_SIZE(LPALLNAMES), LPALLNAMES);
  _Program->initialize_lb(1, "cat", ARRAY_SIZE(LPALLNAMES_CAT), LPALLNAMES_CAT);
  Serial.printf_P(PSTR(">>>>%d<<<\n"), ESP.getFreeHeap());

  // LISTEREF_CREATE
  _Program->initialize(ARRAY_SIZE(LPALLNAMES), LPALLNAMES, "full", SORT_TYPE::ST_BASE);
  _Program->print(PM_LB);

  // _Program->initialize(ARRAY_SIZE(LPALLNAMES_CAT),  LPALLNAMES_CAT, "cat",  SORT_TYPE::ST_BASE);
  // _Program->print(PM_LB); 
  _Program->print(PM_LBNAME);
  //--------------------------------------------------------------------------------------------

  // PLAYLIST_INITIALIZE -----------------------------------------------------------------------
  uint8_t plC       = 5;
  uint8_t iC[]      = { 10,      5,        4,        3,        2 };  // nb items max
  const char* Ln[]  = { "full",  "full",   "full",   "null",   "null" };
  _Program->initialize_playlist(plC, iC, Ln);
#ifdef FSOK
  _Program->pl_fs_restore();
#endif

  _Program->print(PM_PL);
  //
  //--------------------------------------------------------------------------------------------


  // SETUP PROGRAMM LOOP -----------------------------------------------------------------------
  _Program->remote_action(RA::RA_LSET_PL, "0", "");
  _Program->remote_action(RA::RA_PLAY_LB);
  _Program->remote_action(RA::RA_PLAY_DELAY, "10", "");
  _Program->remote_action(RA::RA_PLAY_STOP);
  _Program->print(PM_LOOP);
  //--------------------------------------------------------------------------------------------

}


void loop() {
  #ifdef DEBUG_KEYBOARD
    _Sr_menu.serialRead();
  #endif
  _Program->handle();
}


void _Program_handleCallback(const String itemBaseName, const uint16_t& itemBasePos, boolean updWebserver) {
  Serial.printf_P(PSTR("[user_callback]\n\t[%d] %s\n"), itemBasePos, itemBaseName.c_str());
  ProgramPtrGet()->print(PM_LLI);
}

#ifdef DEBUG_KEYBOARD
void serial_menu_cmd(const String& cmd, const String& value) {
  Serial.printf_P(PSTR("[serial_menu_cmd] cmd[%s] value[%s]\n"), cmd.c_str(), value.c_str());
  uint8_t p = value.toInt();
  String  v = "";
  String  v2 = "";
  int     rSize = 0;
  String* arg = AP_explode(value, ',', rSize);
  if (rSize > 0) { v = arg[1]; }
  if (rSize > 1) { v2 = arg[2]; }

  DynamicJsonDocument doc(1024);
  JsonArray           arr;
  JsonObject          var;
  String              reply;

  doc[F("op")] = 0;
  doc[F("type")] = "ESP";

  arr = doc.createNestedArray(F("set"));
  var = arr.createNestedObject();
  var[F("n")] = FPSTR(RAALLNAMES[p]);
  var[F("v")] = v;

  // arr = doc.createNestedArray(F("get"));  
  // arr.add("loop");

  _AP_Api.parsingRequest(doc, reply, v2);
  Serial.printf_P(PSTR("[serial_menu_cmd->reply]\n%S\n"), reply.c_str());

  // RA action = RAARR[p];
  // _Program->remote_action(action, v.c_str(), v2.c_str(), NULL);    
}
#endif