
#include "main.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wificonnect.h>

#include "apwebserver.h"
WifiConnect* _DeviceWifi;
espwebServer  clientServer(80);

#include "socketserver.h"
socketserver  Socketserver(81);


Program* _Program = nullptr;

HeapStatu _HeapStatu;
HeapStatu _HeapStatu_2;

#ifdef DEBUGSERIAL
SerialRead* _serial;
void serial_menu(const String& cmd, const String& value);
#endif
void _Program_handleCallback(const String itemBaseName, const uint16_t& itemBasePos, boolean updWebserver);
void webserver_parsingRequest(String s);


void setup() {
  Serial.begin(115200);

  for (unsigned long const serialBeginTime = millis(); !Serial && (millis() - serialBeginTime > 5000); ) {}
  delay(300);

  HeapStatu::setupHeap_v1();

  Serial.println(F("\n##################################################\n"));

  Serial.printf_P(PSTR("\t[freeheap: %d]\n"), ESP.getFreeHeap());

  define_print();

  Serial.setDebugOutput(false);

#ifdef DEBUGSERIAL
  _serial = new SerialRead();
  _serial->cmd_array(1, 6);
  _serial->cmd_item_add(1, "menu",                "a", [](const String& cmd, const String& value) { _serial->menu();  });
  _serial->cmd_item_add(1, "ESPreset",            "z", [](const String& cmd, const String& value) { ESP.restart();    });
  _serial->cmd_item_add(1, "freeHeap",            "e", [](const String& cmd, const String& value) { Serial.printf_P(PSTR("freeHeap: %d\n"), ESP.getFreeHeap()); });
  _serial->cmd_item_add(1, "debug prog",          "r", [](const String& cmd, const String& value) { _Program->print(PM_LOOP);  });
  _serial->cmd_item_add(1, "remote action list",  "t", [](const String& cmd, const String& value) { uint8_t cnt = ARRAY_SIZE(RAALLNAMES); for (int i = 0; i < cnt; i++) { Serial.printf_P(PSTR("[%-3d][%-25S]\n"), i, RAALLNAMES[i]); }});
  _serial->cmd_item_add(1, "lb+allpl",            "y", [](const String& cmd, const String& value) { _Program->print(PM_LB); _Program->print(PM_PL); });
  _serial->cmd_array(2, 1);
  _serial->cmd_item_add(2, "remote action", "e", serial_menu_cmd);
  Serial.printf_P(PSTR("\t[0 | freeheap: %d]\n"), ESP.getFreeHeap()); // debug Serial
#endif

  boolean fs = FILESYSTEM.begin();

  _DeviceWifi = new WifiConnect(
    "arduinopattern",
    "freebox_123_EXT",
    "phcaadax",
    "adsap1234",
    "adsota1234");
  _DeviceWifi->setFunc_STAinitServer  ( [](){clientServer.begin();Socketserver.setup();} );
  _DeviceWifi->setFunc_APinitServer   ( [](){clientServer.begin();Socketserver.setup();} );

  clientServer.filesystem_ok(fs);
  clientServer.setup(true);
  Socketserver._parse = webserver_parsingRequest;

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
  _Program->pl_fs_restore();
  _Program->print(PM_PL);

  //
  //--------------------------------------------------------------------------------------------


  // SETUP PROGRAMM LOOP -----------------------------------------------------------------------
  _Program->remote_action(RA::RA_LSET_PL, "0");
  _Program->remote_action(RA::RA_PLAY_LB);
  _Program->remote_action(RA::RA_PLAY_DELAY, "10");
  _Program->remote_action(RA::RA_PLAY_STOP);
  _Program->print(PM_LOOP);
  //--------------------------------------------------------------------------------------------


  String heap, time;
  on_timeD(time); _HeapStatu_2.setupHeap_v2(); _HeapStatu_2.update(); _HeapStatu_2.print(heap);
  Serial.printf_P(PSTR("[HEAP MONITOR]\n\t%-15s%s\n##########################\n"), time.c_str(), heap.c_str());
}


void loop() {

#ifdef DEBUGSERIAL
  _serial->loop();
#endif

  /*
  static uint32_t _timer_2;
    if ((millis()-_timer_2)>10000) {
    // uint32_t tR;
    // _Program->get_remainingTime(tR);
    // char t[100];
    // millis2time(tR, t);
     // Serial.printf("rt: %d\n", 10000 - tR );
     _timer_2 = millis();
      String heap;
      String time;
      on_timeD(time);
      _HeapStatu_2.update();
      _HeapStatu_2.print(heap);
      Serial.printf_P(PSTR("[HEAP MONITOR]\n\t%-15s%s\n"), time.c_str(), heap.c_str());
    }
  */

  _DeviceWifi->handleConnection();

  if (_DeviceWifi->WIFIsetupIsReady()) {
    ArduinoOTA.handle();
    clientServer.handleClient();
    Socketserver.loop();
  }

  ProgramPtrGet()->handle();

}

void _Program_handleCallback(const String itemBaseName, const uint16_t& itemBasePos, boolean updWebserver) {

  String heap, time;
  on_timeD(time);
  _HeapStatu.update(); _HeapStatu.print(heap);
  Serial.printf_P(PSTR("[user_callback]\n\t[%d] %s\n\t%-15s%s\n"), itemBasePos, itemBaseName.c_str(), time.c_str(), heap.c_str());
  ProgramPtrGet()->print(PM_LLI);

  if (!updWebserver) return;

  String                    rep;
  DynamicJsonDocument       reply(2048);
  AP_ApiReply* _webserverRequest_reply = new AP_ApiReply[1];

  _webserverRequest_reply[0].set_ra(RA::RA_ITEM_NEXT);
  _webserverRequest_reply[0].reply_generate(reply);
  serializeJson(reply, rep);

  delete[] _webserverRequest_reply;

  Socketserver.sendTXT(0, rep);
}
void webserver_parsingRequest(String s) {
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, s);
  if (error) {
    Serial.printf_P(PSTR("[webserver_parsingRequest][deserializeJson ERROR: %d]\nstring:\n\t%s"), error, s.c_str());
  }
  else {
    String reply;
    _AP_Api.parsingRequest(doc, reply, "");
    Socketserver.sendTXT(0, reply);
  }
}



#ifdef DEBUGSERIAL
void serial_menu(const String& cmd, const String& value) { _serial->menu(); }
void serial_ESPreset(const String& cmd, const String& value) { ESP.restart(); }
void serial_freeHeap(const String& cmd, const String& value) { Serial.printf_P(PSTR("freeHeap: %d\n"), ESP.getFreeHeap()); }
void serial_menu_cmd(const String& cmd, const String& value) {
  Serial.printf_P(PSTR("[serial_menu_cmd] cmd[%s] value[%s]\n"), cmd.c_str(), value.c_str());
  uint8_t p = value.toInt();
  String  v = "";
  int     rSize = 0;
  String* arg = AP_explode(value, ',', rSize);
  if (rSize > 0) { v = arg[1]; }

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

  arr = doc.createNestedArray(F("get"));
  arr.add("loop");

  _AP_Api.parsingRequest(doc, reply, "");
  Socketserver.sendTXT(0, reply);
  // RA action = RAARR[p];
  // _Program->remote_action(action, v.c_str(), "", NULL);    
}
void serial_menu_p_3(const String& cmd, const String& value) { _Program->print(PM_LB); _Program->print(PM_PL); }
void serial_menu_p_1(const String& cmd, const String& value) { _Program->print(PM_LOOP); }
void serial_menu_p_2(const String& cmd, const String& value) {
  uint8_t cnt = ARRAY_SIZE(RAALLNAMES);
  for (int i = 0; i < cnt; i++) { Serial.printf_P(PSTR("[%-3d][%-25S]\n"), i, RAALLNAMES[i]); } // debug Serial
}
#endif
