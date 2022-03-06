
#include "main.h"

#include <arduinoPatternList.h>

#include <Arduino.h>
#include <ArduinoJson.h>

Program * _Program = nullptr;

#ifdef DEBUGSERIAL
  //debug files
  SerialRead * _serial;
  void serial_menu(const String & cmd, const String & value)    {_serial->menu();}
  void serial_ESPreset(const String & cmd, const String & value){ESP.restart();}   
  void serial_freeHeap(const String & cmd, const String & value){fsprintf("freeHeap: %d\n", ESP.getFreeHeap());}  
  void serial_menu_cmd(const String & cmd, const String & value){
    Serial.printf_P(PSTR("[serial_menu_cmd] cmd[%s] value[%s]\n"), cmd.c_str(), value.c_str());
    uint8_t p       = value.toInt();
    String  v       = "";
    String  v2      = "";
    int     rSize   = 0;
    String  * arg   = LH_explode(value, ',', rSize) ;
    if (rSize>0) {v = arg[1];}
    if (rSize>1) {v2  = arg[2];}

      DynamicJsonDocument doc(1024);
      JsonArray           arr;
      JsonObject          var;
      String              reponse;

      doc[F("op")]    = 0;
      doc[F("type")]  = "ESP";

      arr = doc.createNestedArray(F("set"));  
      var = arr.createNestedObject();
      var[F("n")] = FPSTR(RAALLNAMES[p]);
      var[F("v")] = v;

      // arr = doc.createNestedArray(F("get"));  
      // arr.add("loop");

      _WebserverRequest.parsingRequest(doc, reponse, v2);
      Serial.printf_P(PSTR("[serial_menu_cmd->reponse]\n%S\n"),reponse.c_str());

		// RA action = RAARR[p];
		// _Program->remote_action(action, v.c_str(), "", NULL);    
  }  
  void serial_menu_p_3(const String & cmd, const String & value){_Program->print(PM_LB); _Program->print(PM_PL); } 
  void serial_menu_p_1(const String & cmd, const String & value){_Program->print(PM_LOOP); } 
  void serial_menu_p_2(const String & cmd, const String & value){
    uint8_t cnt = ARRAY_SIZE(RAALLNAMES);
    for(int i=0; i<cnt; i++){ Serial.printf_P(PSTR("[%-3d][%-25S]\n"), i, RAALLNAMES[i]);} // debug Serial
  } 
#endif

void _Program_handleCallback(const String itemBaseName, const uint16_t & itemBasePos, boolean updWebserver){

  Serial.printf_P(PSTR("[user_callback]\n\t[%d] %s\n"), itemBasePos, itemBaseName.c_str());
  ProgramPtrGet()->print(PM_LLI);

}

void setup() {
  Serial.begin(115200);

  for(unsigned long const serialBeginTime = millis(); !Serial && (millis() - serialBeginTime > 5000); ) { }
  delay(300);

  Serial.println(F("\n##################################################\n"));

  Serial.printf_P(PSTR("\t[freeheap: %d]\n"), ESP.getFreeHeap());

  #ifdef DEBUGSERIAL
    Serial.println(F("[DEBUGSERIAL][OK]"));  
  #else
    Serial.println(F("[DEBUGSERIAL][NO]"));  
  #endif
  #ifdef USE_SPIFFS
    Serial.println(F("[USE_SPIFFS][OK]"));  
  #else
    Serial.println(F("[USE_SPIFFS][NO]"));  
  #endif
  #ifdef USE_LITTLEFS
    Serial.println(F("[USE_LITTLEFS][OK]"));  
  #else
    Serial.println(F("[USE_LITTLEFS][NO]"));  
  #endif  
  #ifdef FSOK
    Serial.println(F("[FSOK][OK]"));  
  #else
    Serial.println(F("[FSOK][NO]"));  
  #endif

  Serial.setDebugOutput(false);

  #ifdef DEBUGSERIAL
    //debug
    debug_printBuffer = new char[1024];
    _serial = new SerialRead();
    _serial->cmd_array(1, 6); 
    _serial->cmd_item_add(  1,  "menu",               "a", serial_menu);
    _serial->cmd_item_add(  1,  "ESPreset",           "z", serial_ESPreset);
    _serial->cmd_item_add(  1,  "freeHeap",           "e", serial_freeHeap);
    _serial->cmd_item_add(  1,  "debug prog",         "r", serial_menu_p_1);
    _serial->cmd_item_add(  1,  "remote action list", "t", serial_menu_p_2);
    _serial->cmd_item_add(  1,  "lb+allpl",           "y", serial_menu_p_3);
    _serial->cmd_array(2, 1); 
    _serial->cmd_item_add(  2, "remote action",       "e", serial_menu_cmd);   
    Serial.printf_P(PSTR("\t[0 | freeheap: %d]\n"), ESP.getFreeHeap()); // debug Serial
  #endif


  _Program = new Program(2, false);
  _Program->set_callback(_Program_handleCallback);

  // LISTEREF_ADD-------------------------------------------------------------------------------
    Serial.printf_P(PSTR(">>>>%d<<<\n"), ESP.getFreeHeap()); 
    _Program->initialize_lb(0, "full",  ARRAY_SIZE(LPALLNAMES)          , LPALLNAMES);
    _Program->initialize_lb(1, "cat",   ARRAY_SIZE(LPALLNAMES_CAT)      , LPALLNAMES_CAT);
    Serial.printf_P(PSTR(">>>>%d<<<\n"), ESP.getFreeHeap()); 

    // LISTEREF_CREATE
    _Program->initialize(ARRAY_SIZE(LPALLNAMES),      LPALLNAMES,     "full", SORT_TYPE::ST_AB);
    _Program->print(PM_LB); 

    // _Program->initialize(ARRAY_SIZE(LPALLNAMES_CAT),  LPALLNAMES_CAT, "cat",  SORT_TYPE::ST_BASE);
    // _Program->print(PM_LB); 
    _Program->print(PM_LBNAME); 
  //--------------------------------------------------------------------------------------------

  // PLAYLIST_INITIALIZE -----------------------------------------------------------------------
    uint8_t plC     = 5;
    uint8_t iC[]      = {10,      5,        4,        3,        2       };  // nb items max
    const char * Ln[] = {"full",  "full",   "full",   "null",   "null"  };
    _Program->initialize_playlist(plC, iC, Ln);


    // PLAYLIST ADD ITEMS
    _Program->pl_item_toArray(0, 255, "lbl_1", "Pépita",                "iref_cfg");
    _Program->pl_item_toArray(0, 255, "lbl_2", "Réglisse(Mon RéRé)",    "iref_cfg");
    _Program->pl_item_toArray(0, 255, "lbl_3", "Crevette(Pot de cole)", "iref_cfg");

    _Program->pl_item_toArray(1, 255, "lbl_1", "Philippe",  "iref_cfg");
    _Program->pl_item_toArray(1, 255, "lbl_2", "Catherine", "iref_cfg");
    _Program->pl_item_toArray(1, 255, "lbl_3", "Papie",     "iref_cfg");
    _Program->pl_item_toArray(1, 255, "lbl_3", "Axel",      "iref_cfg");
    _Program->pl_item_toArray(1, 255, "lbl_3", "Eliza",     "iref_cfg");
    _Program->print(PM_PL); 
  //
  //--------------------------------------------------------------------------------------------


  // SETUP PROGRAMM LOOP -----------------------------------------------------------------------
    _Program->remote_action(RA::RA_LSET_PL,     "0",  "", NULL);
    _Program->remote_action(RA::RA_PLAY_LB,     NULL);
    _Program->remote_action(RA::RA_PLAY_DELAY,  "10", "", NULL);
    _Program->remote_action(RA::RA_PLAY_STOP,   NULL);
    _Program->print(PM_LOOP);
  //--------------------------------------------------------------------------------------------

}


void loop() {

  #ifdef DEBUGSERIAL
    _serial->loop();  
  #endif

  ProgramPtrGet()->handle();
}


