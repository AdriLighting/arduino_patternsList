#include "system.h"


#include <arduinoPatternList.h>


#include <Wificonnect.h>
#include <ESP8266WiFi.h>  
extern WifiConnect * _DeviceWifi;	
#ifdef FSOK
  #if defined USE_SPIFFS
    #include <FS.h>
  #elif defined USE_LITTLEFS
    #include <LittleFS.h> 
  #endif
#endif


namespace {
  String ip2string(const IPAddress &a) {
      char buf[18];
      sprintf(buf,"%d.%d.%d.%d",a[0],a[1],a[2],a[3]);
      return String(buf);
  }
	IPAddress get_localIP() {
	  IPAddress localIP;
	  localIP = WiFi.localIP();
	  if (localIP[0] != 0) {
	    return localIP;
	  }

	  return INADDR_NONE;
	}	

}

void serverSystem::sys_firmWareJson(JsonObject & root){
  // String s_1 = String(ARDUINO_BOARD);
  // String s_2 = WiFi.macAddress();
  // s_2.replace(":", "");
  root[FPSTR(SYS_firmware_mac)]       = WiFi.macAddress();	
  root[FPSTR(SYS_firmware_eboard)]    = String(ARDUINO_BOARD);
  root[FPSTR(SYS_firmware_board)]     = FPSTR(ALS_BOARD_T     [LAMPBOARD]);
  root[FPSTR(SYS_firmware_framework)] = FPSTR(ALS_FRAMEWORK_T [LAMPFRAMEWORK]);
  root[FPSTR(SYS_firmware_platform)]  = FPSTR(ALS_PLATFORM_T  [LAMPPLATFORM]);
  root[FPSTR(SYS_firmware_cpu)]       = FPSTR(ALS_CPU_T       [LAMPCPU]);
  root[FPSTR(SYS_firmware_ram)]       = FPSTR(ALS_RAM_T       [LAMPRAM]);
  root[FPSTR(SYS_firmware_flash)]     = FPSTR(ALS_FLASH_T     [LAMPFLASH]);  

}
void serverSystem::sys_json(JsonObject & root){
#ifdef FSOK
  FSInfo fs_info;
  FILESYSTEM.info(fs_info);	
#endif
  String upTime;
  on_timeD(upTime);
  root[FPSTR(SYS_freeheap)]   = ESP.getFreeHeap();
  // root[FPSTR(SYS_initheap)]   = pInitHeap;
#ifdef FSOK
  root[FPSTR(SYS_fstotal)]    = fs_info.totalBytes;
  root[FPSTR(SYS_fsused)]     = fs_info.usedBytes;
  root[FPSTR(SYS_fsfree)]     = (fs_info.totalBytes-fs_info.usedBytes);	
#endif
  root[FPSTR(SYS_uptime)]     = upTime;
  root[FPSTR(SYS_heapfrag)]   = ESP.getHeapFragmentation();  
}
void serverSystem::sys_networkJson(JsonObject & root){
	String ip = ip2string(get_localIP());
	String getVal;
	root[FPSTR(SYS_network_currentIp)]    = ip;
  _DeviceWifi->get_hostName(getVal);
	root[FPSTR(SYS_network_hostName)]     = getVal;
	root[FPSTR(SYS_network_apIP)]         = "192.168.4.1";
	_DeviceWifi->get_apSSID(getVal);
	root[FPSTR(SYS_network_apHostName)]   = getVal;
	_DeviceWifi->get_apPsk(getVal);
	root[FPSTR(SYS_network_apPsk)]        = getVal;
	_DeviceWifi->get_staPsk(getVal);
	root[FPSTR(SYS_network_staPsk)]       = getVal;;
	_DeviceWifi->get_staSSID(getVal);
	root[FPSTR(SYS_network_SSID)]         = getVal;
	root[FPSTR(SYS_network_sta)]          = WiFi.status();
}	
void serverSystem::sys_toString( String & out){
  DynamicJsonDocument doc(1024);
  JsonObject root = doc.to<JsonObject>();
  JsonObject oOjbect = root.createNestedObject("device");
  JsonObject json = oOjbect.createNestedObject("system");
  sys_json(json);
  serializeJson(doc, out);
}
void serverSystem::sys_networkToString(String & out){
  DynamicJsonDocument doc(1024);
  JsonObject root = doc.to<JsonObject>();
  JsonObject oOjbect = root.createNestedObject("device");
  JsonObject json = oOjbect.createNestedObject("network"); 
  sys_networkJson(json);
  serializeJson(doc, out);
}	
void serverSystem::sys_fullToString(String & out){
  DynamicJsonDocument doc(1024);
  JsonObject root = doc.to<JsonObject>();
  JsonObject oOjbect = root.createNestedObject("device");
  sys_networkJson(oOjbect);	
  sys_json(oOjbect);
  serializeJson(doc, out);
}


