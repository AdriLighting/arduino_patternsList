#include "main.h"

#include <Arduino.h>
#include <alwificonnect.h>
#include <arduinoPatternList.h>
#include <typeinfo>

#include "apwebserver.h"
#include "TaskScheduler.h"

// region ################################################ INSTANCE 
WifiConnect     * _DeviceWifi;
Program         * _Program = nullptr;
HeapStatu       _HeapStatu;
HeapStatu       _HeapStatu_2;
TaskScheduler   * _TaskScheduler;
socketQueue     * _socketQueueReply;
socketQueue     * _socketQueueSetter;  
// endregion >>>> 
   
void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

#ifdef DEBUG_KEYBOARD
  void keyboard_freeheap();
  void keyboard_apiSetter(const String & cmd, const String & value);
#endif

// region ################################################ PROTOTYPE 
void _http_post_cb(AsyncWebServerRequest * request, uint8_t pos, const String &data);
void _http_get_cb(AsyncWebServerRequest * request, uint8_t pos, const String &data);
void _socket_cb(const String & s); 
void _Program_cb(const String itemBaseName, const uint16_t & itemBasePos, boolean updWebserver);
// endregion >>>> 


// String _global_s = "_global_s";
// void global_s_onChange() {
//   Serial.printf("chnage");
// }
// DebugWatcher<String> _watcher(&_global_s, &global_s_onChange);

void setup() {
  Serial.begin(115200);

  for(unsigned long const serialBeginTime = millis(); !Serial && (millis() - serialBeginTime > 5000); ) { }
  delay(300);

  HeapStatu::setupHeap_v1();

  #ifdef ALT_DEBUG_TARCE
    _DebugPrintList.add("main");  
  #endif

  Serial.println(F("\n##################################################\n"));

  Serial.printf_P(PSTR("\t[freeheap: %d]\n"), ESP.getFreeHeap());


  #ifdef DEBUG_KEYBOARD
    _Sr_menu.add("heaploop", "q", keyboard_freeheap);
    _Sr_menu.add("api setter socket", "!", keyboard_apiSetter, SR_MM::SRMM_KEY);
  #endif

    #if defined(ESP8266)
      boolean fs = FILESYSTEM.begin();
    #elif defined(ESP32)
      boolean fs = FILESYSTEM.begin(false);
      createDir(FILESYSTEM, "/playlist");
    #endif
    

  // region ################################################ WIFI
  _DeviceWifi = new WifiConnect(
    "apdebug_1",
    "SSID",
    "SSIDPASS",
    "adsap1234",
    "adsota1234");
  _DeviceWifi->setFunc_STAinitServer  ( [](){_Webserver.begin();} );
  _DeviceWifi->setFunc_APinitServer   ( [](){_Webserver.begin();} );  
  // endregion >>>> WIFI


  // region ################################################ WEBSERVER
  _AP_userApi.initialize(2);
  _AP_userApi.set_request(0, "user", [](const String & v1, DynamicJsonDocument & doc){
    Serial.printf("[user getter][req: %s]\n", v1.c_str());
    JsonObject var = doc.createNestedObject(FPSTR(APPT_REQ_005));
    _Program->get_json_jsInit(var);});

  _Webserver.request_array(3);
  uint8_t rP = _Webserver.request_new("/api", HTTP_POST, WSTE_APPJSON);
  Webserver_request * request = _Webserver.get_requestArray(rP);
  request->set_callback(_http_post_cb);
  request->set_rType(WSRM_FROMCALLBACK);

  rP = _Webserver.request_new("/api", HTTP_GET, WSTE_APPJSON);
  request = _Webserver.get_requestArray(rP);
  request->set_callback(_http_get_cb);
  request->set_rType(WSRM_FROMCALLBACK); 

  _Webserver.setup();
  _Webserver.set_socketCallback(_socket_cb);  
  // endregion >>>> WEBSERVER


  // region ################################################ PROGRAM
  // 
  _Program = new Program(1, fs);
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
  uint8_t iC[]      = {20,      20,        20,      0,        0       };  // nb items max
  const char * Ln[] = {"full",  "full",   "full",   "null",   "null"  };
  _Program->initialize_playlist(plC, iC, Ln);
  _Program->pl_fs_restore();  
  //--------------------------------------------------------------------------------------------
  // SETUP PROGRAMM LOOP -----------------------------------------------------------------------
  _Program->remote_action(apSetter_t::APSET_LSET_PL, "0");
  _Program->remote_action(apSetter_t::APSET_PLAY_LB);
  _Program->remote_action(apSetter_t::APSET_PLAY_DELAY, "10");
  _Program->remote_action(apSetter_t::APSET_PLAY_STOP);
  //--------------------------------------------------------------------------------------------  
  // endregion >>>> PROGRAM


  // region ################################################ TASKS
  _TaskScheduler = new TaskScheduler(9);
  
  _TaskScheduler->get_task(0)->set_callback([](){if (_Program) _Program->handle();});
  _TaskScheduler->get_task(1)->set_callback([](){_DeviceWifi->handleConnection();});
  _TaskScheduler->get_task(2)->set_callback([](){if (_DeviceWifi->WIFIsetupIsReady())_Webserver.handle();});
  _TaskScheduler->get_task(3)->set_callback([](){if(_socketQueueReply)_socketQueueReply->handle();;});
  _TaskScheduler->get_task(4)->set_callback([](){if(_socketQueueSetter)_socketQueueSetter->handle();;});
  for (int i = 0; i < 5; ++i) {
    _TaskScheduler->get_task(i)->set_taskDelay(ETD::ETD_TIMER, true, 100000);
    _TaskScheduler->get_task(i)->set_iteration_max(-1);
    _TaskScheduler->get_task(i)->setup(true);
    _TaskScheduler->get_task(i)->set_enabled(true);
  }
  _socketQueueReply   = new socketQueueReply  (_TaskScheduler->get_task(5));
  _socketQueueSetter  = new socketQueueSetter (_TaskScheduler->get_task(6));  
  // endregion >>>> 

  delay(2);
  String heap, time;
  on_time_h(time);_HeapStatu_2.setupHeap_v2();_HeapStatu_2.update();_HeapStatu_2.print(heap);
  Serial.printf_P(PSTR("[HEAP MONITOR]\n\t%-15s%s\n##########################\n"), time.c_str(), heap.c_str());

  Serial.println(ESP.getFreeHeap());
  AP_SPIFFS_PRINT(false);
  Serial.println(ESP.getFreeHeap());

/*
  ALT_TRACEC("main", "%d\n", ESP.getFreeHeap());
  String **_sList = nullptr;
  _sList = new String*[10];
  for(int i = 0; i < 10; ++i) {
    _sList[i] = new String;
    String ptr = *(String*)_sList[i];
    ptr = "TEST_1" + String(i);
    ALT_TRACEC("main", "&c:1&s:%d - %s\n", i, ptr.c_str());
  }
  for(int i=0; i<10; i++) delete _sList[i];
  delete[] _sList;  
  ALT_TRACEC("main", "%d\n", ESP.getFreeHeap());
*/

}

// uint32_t _timer_watch = 0;
// boolean _testStr  =true;
void loop() {
  #ifdef DEBUG_KEYBOARD
    _Sr_menu.serialRead();
  #endif
  _TaskScheduler->loop();
  // if ( (millis() - _timer_watch) > 3000 ) {
  //   _timer_watch = millis();
  //   _global_s = (_testStr)? "value_1" : "value_2";
  //   _testStr = !_testStr;
  // }
  // _watcher.loop();
}

void _http_get_cb(AsyncWebServerRequest * request, uint8_t pos, const String & data){
  _Webserver.http_send(request, 200, WSTE_TXTPLAIN, "ok");
}
void _http_post_cb(AsyncWebServerRequest * request, uint8_t pos, const String & data){
  String json = data;
  json.replace("\r\n", "");
  // Serial.printf_P(PSTR("[_callPost /json] %s\n"), json.c_str(()));  
  String reply = "";
  DynamicJsonDocument doc(2048);  
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.printf_P(PSTR("[_http_post_cb][PARSSING ERROR] %s\n"), json.c_str());  
  } else {_AP_Api.parsingRequest(doc, reply, "");} 
  _Webserver.http_send(request, 200, WSTE_APPJSON, reply);
  _TaskScheduler->get_task(7)->set_callbackOstart([=](){_Webserver.socket_send(reply);});
  _TaskScheduler->get_task(7)->set_iteration_max(0);
  _TaskScheduler->get_task(7)->set_taskDelay(ETD::ETD_DELAY, true, 250, 1);
  _TaskScheduler->get_task(7)->set_taskDelayEnabled(ETD::ETD_DELAY, true);
  _TaskScheduler->get_task(7)->set_enabled(true); 
}

uint32_t _socket_cb_last = 0;
void _socket_cb(const String & s){

  uint32_t last = millis()-_socket_cb_last;
  ALT_TRACEC("main", "[last: %d]\n", last);
  
  _socket_cb_last = millis();

  DynamicJsonDocument doc(1024);  
  DeserializationError error = deserializeJson(doc, s);
  if (error) {      
    ALT_TRACEC("main", "[deserializeJson ERROR: %d]\nstring:\n\t%s", error, s.c_str());  
  } else {
    _socketQueueSetter->receive(doc);
  }
}  
void _Program_cb(const String itemBaseName, const uint16_t & itemBasePos, boolean updWebserver){
  String heap;
  _HeapStatu.update();_HeapStatu.print(heap);
  ALT_TRACEC("main", "\n");
  ALT_TRACEC("main", "&c:1&s:\t[%d] %s\n", itemBasePos, itemBaseName.c_str());
  ALT_TRACEC("main", "&c:1&s:\t%s\n", heap.c_str());
  if (!updWebserver) return; 
  if (!_Webserver.socketIsConnected()) {ALT_TRACEC("main", "[UPD WEBSERVER] no socket connected\n");return;} 
  ALT_TRACEC("main", "UPD WEBSERVER\n");
  String                    reply;
  DynamicJsonDocument       doc(2048);
  AP_ApiReply             * _ApiReply = new AP_ApiReply();
  _ApiReply->set_ra(apSetter_t::APSET_ITEM_NEXT);
  _ApiReply->reply_generate(doc);
  delete _ApiReply; 
  serializeJson(doc, reply); 
  doc.clear();
   _socketQueueReply->receive(reply);
}

#ifdef DEBUG_KEYBOARD
  void keyboard_freeheap(){
    static boolean statu = false;
    statu = !statu;
    _TaskScheduler->get_task(8)->set_callback([](){Serial.printf_P(PSTR("%d\n"), ESP.getFreeHeap());});
    _TaskScheduler->get_task(8)->set_taskDelay(ETD::ETD_TIMER, true, 3, 2);
    _TaskScheduler->get_task(8)->set_iteration_max(-1);
    _TaskScheduler->get_task(8)->setup(true);
    _TaskScheduler->get_task(8)->set_enabled(statu);
  }
  void keyboard_apiSetter(const String & cmd, const String & value){
    Serial.printf_P(PSTR("[keyboard_apiSetter] cmd[%s] value[%s]\n"), cmd.c_str(), value.c_str());
    uint8_t p = cmd.toInt();
    DynamicJsonDocument doc(1024);
    JsonArray           arr;
    JsonObject          var;
    String              reply;
    doc[F("op")]    = 0;
    doc[F("type")]  = "ESP";
    arr = doc.createNestedArray(F("set"));  
    var = arr.createNestedObject();
    var[F("n")] = FPSTR(APPT_SETTER_ARRAY[p]);
    var[F("v")] = value;
    arr = doc.createNestedArray(F("get"));  
    arr.add("loop");
    _AP_Api.parsingRequest(doc, reply, "");
    _Webserver.socket_send(reply);
  } 
#endif



