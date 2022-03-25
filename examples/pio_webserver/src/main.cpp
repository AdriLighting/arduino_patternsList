
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

#ifdef DEBUG_KEYBOARD
    void serial_menu_cmd(const String & cmd, const String & value);
#endif
void _Program_cb(const String itemBaseName, const uint16_t& itemBasePos, boolean updWebserver);
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

  #ifdef DEBUG_KEYBOARD
    _Sr_menu.add("name_1",             "!", serial_menu_cmd, SR_MM::SRMM_KEY);
  #endif
  boolean fs = FILESYSTEM.begin();

  _DeviceWifi = new WifiConnect(
    "arduinopattern",
    "SSID",
    "SSIDPASS",
    "adsap1234",
    "adsota1234");
  _DeviceWifi->setFunc_STAinitServer  ( [](){clientServer.begin();Socketserver.setup();} );
  _DeviceWifi->setFunc_APinitServer   ( [](){clientServer.begin();Socketserver.setup();} );

  clientServer.filesystem_ok(fs);
  clientServer.setup(true);
  Socketserver._parse = webserver_parsingRequest;

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


  String heap, time;
  on_time_h(time); _HeapStatu_2.setupHeap_v2(); _HeapStatu_2.update(); _HeapStatu_2.print(heap);
  Serial.printf_P(PSTR("[HEAP MONITOR]\n\t%-15s%s\n##########################\n"), time.c_str(), heap.c_str());
}


void loop() {

#ifdef DEBUG_KEYBOARD
  _Sr_menu.serialRead();
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

void _Program_cb(const String itemBaseName, const uint16_t& itemBasePos, boolean updWebserver) {

  String heap, time;
  on_time_h(time);
  _HeapStatu.update(); _HeapStatu.print(heap);
  Serial.printf_P(PSTR("[user_callback]\n\t[%d] %s\n\t%-15s%s\n"), itemBasePos, itemBaseName.c_str(), time.c_str(), heap.c_str());

  if (!updWebserver) return;

  String                    rep;
  DynamicJsonDocument       reply(2048);
  AP_ApiReply* _webserverRequest_reply = new AP_ApiReply[1];

  _webserverRequest_reply[0].set_ra(apSetter_t::APSET_ITEM_NEXT);
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



#ifdef DEBUG_KEYBOARD
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
  var[F("n")] = FPSTR(APPT_SETTER_ARRAY[p]);
  var[F("v")] = v;

  arr = doc.createNestedArray(F("get"));
  arr.add("loop");

  _AP_Api.parsingRequest(doc, reply, "");
  Socketserver.sendTXT(0, reply);
  // apSetter_t action = RAARR[p];
  // _Program->remote_action(action, v.c_str(), "", NULL);    
}
#endif

