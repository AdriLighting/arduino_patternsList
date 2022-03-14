#include "main.h"

#include <Arduino.h>
#include <Wificonnect.h>
#include <arduinoPatternList.h>

#include "apwebserver.h"
#include "TaskScheduler.h"

WifiConnect* _DeviceWifi;
Program* _Program = nullptr;
HeapStatu     _HeapStatu;
HeapStatu     _HeapStatu_2;
TaskScheduler* _TaskScheduler;


#ifdef DEBUGSERIAL
SerialRead* _serial;
void serial_menu_cmd(const String& cmd, const String& value);
#endif
void _http_post_cb(AsyncWebServerRequest* request, uint8_t pos, const String& data);
void _http_get_cb(AsyncWebServerRequest* request, uint8_t pos, const String& data);
void _socket_cb(const String& s);
void _Program_cb(const String itemBaseName, const uint16_t& itemBasePos, boolean updWebserver);

void setup() {
	Serial.begin(115200);

	for (unsigned long const serialBeginTime = millis(); !Serial && (millis() - serialBeginTime > 5000); ) {}
	delay(300);

	HeapStatu::setupHeap_v1();

	Serial.println(F("\n##################################################\n"));

	Serial.printf_P(PSTR("\t[freeheap: %d]\n"), ESP.getFreeHeap());

	define_print();

#ifdef DEBUGSERIAL
	_serial = new SerialRead();
	_serial->cmd_array(1, 6);
	_serial->cmd_item_add(1, "menu", "a", [](const String& cmd, const String& value) { _serial->menu();  });
	_serial->cmd_item_add(1, "ESPreset", "z", [](const String& cmd, const String& value) { ESP.restart();    });
	_serial->cmd_item_add(1, "freeHeap", "e", [](const String& cmd, const String& value) { Serial.printf_P(PSTR("freeHeap: %d\n"), ESP.getFreeHeap()); });
	_serial->cmd_item_add(1, "debug prog", "r", [](const String& cmd, const String& value) { _Program->print(PM_LOOP);  });
	_serial->cmd_item_add(1, "remote action list", "t", [](const String& cmd, const String& value) { uint8_t cnt = ARRAY_SIZE(RAALLNAMES); for (int i = 0; i < cnt; i++) { Serial.printf_P(PSTR("[%-3d][%-25S]\n"), i, RAALLNAMES[i]); }});
	_serial->cmd_item_add(1, "lb+allpl", "y", [](const String& cmd, const String& value) { _Program->print(PM_LB); _Program->print(PM_PL); });
	_serial->cmd_array(2, 1);
	_serial->cmd_item_add(2, "remote action", "e", serial_menu_cmd);
#endif

	boolean fs = FILESYSTEM.begin();

	_DeviceWifi = new WifiConnect(
		"hostnameapas",
		"freebox_123_EXT",
		"phcaadax",
		"adsap1234",
		"adsota1234");
	_DeviceWifi->setFunc_STAinitServer([]() {_Webserver.begin(); });
	_DeviceWifi->setFunc_APinitServer([]() {_Webserver.begin(); });

	_Webserver.request_array(3);
	uint8_t rP = _Webserver.request_new("/api", HTTP_POST, WSTE_APPJSON);
	Webserver_request* request = _Webserver.get_requestArray(rP);
	request->set_callback(_http_post_cb);
	request->set_rType(WSRM_FROMCALLBACK);

	rP = _Webserver.request_new("/api", HTTP_GET, WSTE_APPJSON);
	request = _Webserver.get_requestArray(rP);
	request->set_callback(_http_get_cb);
	request->set_rType(WSRM_FROMCALLBACK);

	_Webserver.setup();
	_Webserver.set_socketCallback(_socket_cb);

	_Program = new Program(2, fs);
	_Program->set_fs_pl(false);
	_Program->set_callback(_Program_cb);

	// LISTEREF_ADD-------------------------------------------------------------------------------
	Serial.printf_P(PSTR(">>>>%d<<<\n"), ESP.getFreeHeap());
	_Program->initialize_lb(0, "full", ARRAY_SIZE(LPALLNAMES), LPALLNAMES);
	_Program->initialize_lb(1, "cat", ARRAY_SIZE(LPALLNAMES_CAT), LPALLNAMES_CAT);
	Serial.printf_P(PSTR(">>>>%d<<<\n"), ESP.getFreeHeap());

	// LISTEREF_CREATE
	_Program->initialize(ARRAY_SIZE(LPALLNAMES), LPALLNAMES, "full", SORT_TYPE::ST_BASE);
	_Program->print(PM_LB);
	delay(1000);

	// _Program->initialize(ARRAY_SIZE(LPALLNAMES_CAT),  LPALLNAMES_CAT, "cat",  SORT_TYPE::ST_BASE);
	// _Program->print(PM_LB); 
	_Program->print(PM_LBNAME);
	delay(1000);
	//--------------------------------------------------------------------------------------------

	// PLAYLIST_INITIALIZE -----------------------------------------------------------------------
	uint8_t plC = 5;
	uint8_t iC[] = { 20,      20,        20,      0,        0 };  // nb items max
	const char* Ln[] = { "full",  "full",   "full",   "null",   "null" };
	_Program->initialize_playlist(plC, iC, Ln);
	_Program->pl_fs_restore();
	_Program->print(PM_PL);
	delay(1000);
	//
	//--------------------------------------------------------------------------------------------


	// SETUP PROGRAMM LOOP -----------------------------------------------------------------------
	_Program->remote_action(RA::RA_LSET_PL, "0");
	_Program->remote_action(RA::RA_PLAY_LB);
	_Program->remote_action(RA::RA_PLAY_DELAY, "10");
	_Program->remote_action(RA::RA_PLAY_STOP);
	_Program->print(PM_LOOP);
	delay(1000);
	//--------------------------------------------------------------------------------------------


	_TaskScheduler = new TaskScheduler(5);
	_TaskScheduler->get_task(0)->set_callback([]() {ProgramPtrGet()->handle(); });
	_TaskScheduler->get_task(0)->set_taskDelay(ETD::ETD_TIMER, true, 100000);
	_TaskScheduler->get_task(0)->set_iteration_max(-1);
	_TaskScheduler->get_task(0)->setup(true);
	_TaskScheduler->get_task(0)->set_enabled(true);

	_TaskScheduler->get_task(1)->set_callback([]() {_DeviceWifi->handleConnection(); });
	_TaskScheduler->get_task(1)->set_taskDelay(ETD::ETD_TIMER, true, 100000);
	_TaskScheduler->get_task(1)->set_iteration_max(-1);
	_TaskScheduler->get_task(1)->setup(true);
	_TaskScheduler->get_task(1)->set_enabled(true);

	_TaskScheduler->get_task(2)->set_callback([]() {if (_DeviceWifi->WIFIsetupIsReady())_Webserver.handle(); });
	_TaskScheduler->get_task(2)->set_taskDelay(ETD::ETD_TIMER, true, 100000);
	_TaskScheduler->get_task(2)->set_iteration_max(-1);
	_TaskScheduler->get_task(2)->setup(true);
	_TaskScheduler->get_task(2)->set_enabled(true);

	String heap, time;
	on_timeD(time); _HeapStatu_2.setupHeap_v2(); _HeapStatu_2.update(); _HeapStatu_2.print(heap);
	Serial.printf_P(PSTR("[HEAP MONITOR]\n\t%-15s%s\n##########################\n"), time.c_str(), heap.c_str());

}


void loop() {
#ifdef DEBUGSERIAL
	_serial->loop();
#endif
	_TaskScheduler->loop();
}

void _http_get_cb(AsyncWebServerRequest* request, uint8_t pos, const String& data) {
	_Webserver.http_send(request, 200, WSTE_TXTPLAIN, "ok");
}
void _http_post_cb(AsyncWebServerRequest* request, uint8_t pos, const String& data) {
	String json = data;
	json.replace("\r\n", "");
	// Serial.printf_P(PSTR("[_callPost /json] %s\n"), json.c_str());  
	String reponse = "";
	DynamicJsonDocument doc(2048);
	DeserializationError error = deserializeJson(doc, json);
	if (error) {
		Serial.printf_P(PSTR("[_http_post_cb][PARSSING ERROR] %s\n"), json.c_str());
	}
	else { _WebserverRequest.parsingRequest(doc, reponse, ""); }
	_Webserver.http_send(request, 200, WSTE_APPJSON, reponse);
	_TaskScheduler->get_task(3)->set_callbackOstart([=]() {_Webserver.socket_send(reponse); });
	_TaskScheduler->get_task(3)->set_iteration_max(0);
	_TaskScheduler->get_task(3)->set_taskDelay(ETD::ETD_DELAY, true, 250, 1);
	_TaskScheduler->get_task(3)->set_taskDelayEnabled(ETD::ETD_DELAY, true);
	_TaskScheduler->get_task(3)->set_enabled(true);
}
void _socket_cb(const String& s) {
	DynamicJsonDocument doc(1024);
	DeserializationError error = deserializeJson(doc, s);
	if (error) {
		Serial.printf_P(PSTR("[webserver_parsingRequest][deserializeJson ERROR: %d]\nstring:\n\t%s"), error, s.c_str());
	}
	else {
		String reponse;
		_WebserverRequest.parsingRequest(doc, reponse, "");
		_Webserver.socket_send(reponse);
	}
}
void _Program_cb(const String itemBaseName, const uint16_t& itemBasePos, boolean updWebserver) {

	String heap, time;
	on_timeD(time);
	_HeapStatu.update(); _HeapStatu.print(heap);
	Serial.printf_P(PSTR("[user_callback]\n\t[%d] %s\n\t%-15s%s\n"),
		itemBasePos, itemBaseName.c_str(), time.c_str(), heap.c_str());
	ProgramPtrGet()->print(PM_LLI);

	if (!updWebserver) return;

	String                    rep;
	DynamicJsonDocument       reponse(2048);
	webserverRequest_reponse* _webserverRequest_reponse = new webserverRequest_reponse[1];

	_webserverRequest_reponse[0].set_ra(RA::RA_ITEM_NEXT);
	_webserverRequest_reponse[0].make_reponse(reponse);
	serializeJson(reponse, rep);

	delete[] _webserverRequest_reponse;
	_Webserver.socket_send(rep);
}

#ifdef DEBUGSERIAL
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
	String              reponse;

	doc[F("op")] = 0;
	doc[F("type")] = "ESP";

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

#endif



