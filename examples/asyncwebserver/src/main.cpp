#include "main.h"

#include <Arduino.h>
#include <Wificonnect.h>
#include <arduinoPatternList.h>

#include "Webserver.h"
#include "TaskScheduler.h"

WifiConnect * _DeviceWifi;
Program     * _Program = nullptr;
HeapStatu   _HeapStatu;
HeapStatu   _HeapStatu_2;
TaskScheduler * _TaskScheduler;

void _task_handle_wifi()      {_DeviceWifi->handleConnection(); }
void _task_handle_webserver() {if (_DeviceWifi->WIFIsetupIsReady())_Webserver.handle();}
void _task_handle_prog()      {ProgramPtrGet()->handle();}

#ifdef DEBUGSERIAL
  SerialRead * _serial;
  void serial_menu(const String & cmd, const String & value);
  void serial_ESPreset(const String & cmd, const String & value);
  void serial_freeHeap(const String & cmd, const String & value);
  void serial_menu_cmd(const String & cmd, const String & value);
  void serial_menu_p_3(const String & cmd, const String & value);
  void serial_menu_p_1(const String & cmd, const String & value);
  void serial_menu_p_2(const String & cmd, const String & value);
#endif
void _Program_handleCallback(const String itemBaseName, const uint16_t & itemBasePos, boolean updWebserver);
void webserver_parsingRequest(const String & s); 
void wehnSTAisReady();
void wehnAPisReady();
void _calltest(AsyncWebServerRequest * request, uint8_t pos);

void setup() {
  Serial.begin(115200);

  for(unsigned long const serialBeginTime = millis(); !Serial && (millis() - serialBeginTime > 5000); ) { }
  delay(300);

  HeapStatu::setupHeap_v1();

  Serial.println(F("\n##################################################\n"));

  Serial.printf_P(PSTR("\t[freeheap: %d]\n"), ESP.getFreeHeap());

  define_print();

  Serial.setDebugOutput(false);

  #ifdef DEBUGSERIAL
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
  #endif

  boolean fs = FILESYSTEM.begin();

  _DeviceWifi = new WifiConnect(
    "hostname",
    "ssid",
    "ssidpsk",
    "adsap1234",
    "adsota1234");
  _DeviceWifi->setFunc_STAinitServer( std::bind(wehnSTAisReady));
  _DeviceWifi->setFunc_APinitServer(  std::bind(wehnAPisReady));

  _Webserver.request_array(10);
  _Webserver.request_new("/test_1", HTTP_GET, WSTE_TXTPLAIN);
  uint8_t rP = _Webserver.request_new("/test_2", HTTP_GET, WSTE_APPJSON);
  Webserver_request * request = _Webserver.get_requestArray(rP);
  request->set_callback(_calltest);
  request->set_rType(WSRM_FROMCALLBACK);
  _Webserver.setup();
  _Webserver.set_socketCallback(webserver_parsingRequest);

  _Program = new Program(2, fs);
  _Program->set_fs_pl(false);  
  _Program->set_callback(_Program_handleCallback);

  // LISTEREF_ADD-------------------------------------------------------------------------------
    Serial.printf_P(PSTR(">>>>%d<<<\n"), ESP.getFreeHeap()); 
    _Program->initialize_lb(0, "full",  ARRAY_SIZE(LPALLNAMES)          , LPALLNAMES);
    _Program->initialize_lb(1, "cat",   ARRAY_SIZE(LPALLNAMES_CAT)      , LPALLNAMES_CAT);
    Serial.printf_P(PSTR(">>>>%d<<<\n"), ESP.getFreeHeap()); 

    // LISTEREF_CREATE
    _Program->initialize(ARRAY_SIZE(LPALLNAMES),      LPALLNAMES,     "full", SORT_TYPE::ST_BASE);
    _Program->print(PM_LB); 

    // _Program->initialize(ARRAY_SIZE(LPALLNAMES_CAT),  LPALLNAMES_CAT, "cat",  SORT_TYPE::ST_BASE);
    // _Program->print(PM_LB); 
    _Program->print(PM_LBNAME); 
  //--------------------------------------------------------------------------------------------

  // PLAYLIST_INITIALIZE -----------------------------------------------------------------------
    uint8_t plC     = 5;
    uint8_t iC[]      = {20,      20,        20,      0,        0       };  // nb items max
    const char * Ln[] = {"full",  "full",   "full",   "null",   "null"  };
    _Program->initialize_playlist(plC, iC, Ln);
    _Program->pl_fs_restore();  
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
   
  _TaskScheduler = new TaskScheduler(4);

  _TaskScheduler->get_task(0)->set_callback(_task_handle_prog);
  _TaskScheduler->get_task(0)->set_taskDelay(ETD::ETD_TIMER, true, 100000);
  _TaskScheduler->get_task(0)->set_iteration_max(-1);
  _TaskScheduler->get_task(0)->setup(true);
  _TaskScheduler->get_task(0)->set_enabled(true);

  _TaskScheduler->get_task(1)->set_callback(_task_handle_wifi);
  _TaskScheduler->get_task(1)->set_taskDelay(ETD::ETD_TIMER, true, 100000);
  _TaskScheduler->get_task(1)->set_iteration_max(-1);
  _TaskScheduler->get_task(1)->setup(true);
  _TaskScheduler->get_task(1)->set_enabled(true);

  _TaskScheduler->get_task(2)->set_callback(_task_handle_webserver);
  _TaskScheduler->get_task(2)->set_taskDelay(ETD::ETD_TIMER, true, 100000);
  _TaskScheduler->get_task(2)->set_iteration_max(-1);
  _TaskScheduler->get_task(2)->setup(true);
  _TaskScheduler->get_task(2)->set_enabled(true);

  String heap, time;
  on_timeD(time);_HeapStatu_2.setupHeap_v2();_HeapStatu_2.update();_HeapStatu_2.print(heap);
  Serial.printf_P(PSTR("[HEAP MONITOR]\n\t%-15s%s\n##########################\n"), time.c_str(), heap.c_str()); 

}

void loop() {
  #ifdef DEBUGSERIAL
    _serial->loop();  
  #endif  
  _TaskScheduler->loop();
}

void _calltest(AsyncWebServerRequest * request, uint8_t pos){
  const char        * requestName;
  WebRequestMethod  method;
  const char        * contentType;
  ENUM_WSRM         rType;

  Webserver_request * wRequest = _Webserver.get_requestArray(pos);

  wRequest->get_method(method);
  wRequest->get_name(requestName);
  wRequest->get_contentType(contentType);
  wRequest->get_rType(rType);

  Serial.printf_P(PSTR("[%d][_calltest]\n\t[method: %d][name: %s]\n\t[content-type: %s][reponse-type: %d]\n"),
  pos, method, requestName, contentType, rType);
  int params = request->params();
  for(uint8_t i=0;i<params;i++){
    AsyncWebParameter* p = request->getParam(i);
    if(p->isFile()){
      Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
    } else if(p->isPost()){
      Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
    } else {
      Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
    }
  }

  DynamicJsonDocument doc(5000);
  JsonArray           arr;
  JsonObject          var;
  String              reponse;

  doc[F("op")]    = 0;
  doc[F("type")]  = "ESP";

  arr = doc.createNestedArray(F("set"));  
  var = arr.createNestedObject();
  var[F("n")] = "";
  var[F("v")] = "";

  arr = doc.createNestedArray(F("get"));  
  arr.add("loop");
  // arr.add("list_pl");

  _WebserverRequest.parsingRequest(doc, reponse, "");

  if (rType == WSRM_FROMCALLBACK) request->send(200, contentType, reponse);      
}

void _Program_handleCallback(const String itemBaseName, const uint16_t & itemBasePos, boolean updWebserver){

  String heap, time;
  on_timeD(time);
  _HeapStatu.update();_HeapStatu.print(heap);
  Serial.printf_P(PSTR("[user_callback]\n\t[%d] %s\n\t%-15s%s\n"), itemBasePos, itemBaseName.c_str(), time.c_str(), heap.c_str());
  ProgramPtrGet()->print(PM_LLI);

  if (!updWebserver) return; 
   
  String                    rep;
  DynamicJsonDocument       reponse(2048);
  webserverRequest_reponse  * _webserverRequest_reponse = new webserverRequest_reponse[1];

  _webserverRequest_reponse[0].set_ra(RA::RA_ITEM_NEXT);
  _webserverRequest_reponse[0].make_reponse(reponse);
  serializeJson(reponse, rep); 

  delete[] _webserverRequest_reponse; 
  _Webserver.socket_send(rep);   
}
void webserver_parsingRequest(const String & s){
  DynamicJsonDocument doc(1024);  
  DeserializationError error = deserializeJson(doc, s);
  if (error) {      
      Serial.printf_P(PSTR("[webserver_parsingRequest][deserializeJson ERROR: %d]\nstring:\n\t%s"), error, s.c_str());  
  } else {
    String reponse;
    _WebserverRequest.parsingRequest(doc, reponse, "");
    _Webserver.socket_send(reponse);
  }
}  

void wehnSTAisReady(){_Webserver.begin();}
void wehnAPisReady(){_Webserver.begin();}  


#ifdef DEBUGSERIAL
  void serial_menu(const String & cmd, const String & value)    {_serial->menu();}
  void serial_ESPreset(const String & cmd, const String & value){ESP.restart();}   
  void serial_freeHeap(const String & cmd, const String & value){Serial.printf_P(PSTR("freeHeap: %d\n"), ESP.getFreeHeap());}  
  void serial_menu_cmd(const String & cmd, const String & value){
    Serial.printf_P(PSTR("[serial_menu_cmd] cmd[%s] value[%s]\n"), cmd.c_str(), value.c_str());
    uint8_t p       = value.toInt();
    String  v       = "";
    int     rSize   = 0;
    String  * arg   = AP_explode(value, ',', rSize) ;
    if (rSize>0) {v = arg[1];}

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

      arr = doc.createNestedArray(F("get"));  
      arr.add("loop");

      _WebserverRequest.parsingRequest(doc, reponse, "");
      _Webserver.socket_send(reponse);
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



