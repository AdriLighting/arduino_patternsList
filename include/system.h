
#ifndef SYSTEM_H
#define SYSTEM_H
  #include <Arduino.h>
  #include <ArduinoJson.h>
  #include "def.h"

  #ifndef LAMPFRAMEWORK
    #define LAMPFRAMEWORK 0
  #endif
  #ifndef LAMPPLATFORM
    #define LAMPPLATFORM 0
  #endif
  #ifndef LAMPBOARD
    #define LAMPBOARD 0
  #endif
  #ifndef LAMPCPU
    #define LAMPCPU 1
  #endif
  #ifndef LAMPRAM
    #define LAMPRAM 0
  #endif
  #ifndef LAMPFLASH
    #define LAMPFLASH 0
  #endif

static const char SYS_freeheap            [] PROGMEM = "Memory - Free Heap";
static const char SYS_initheap            [] PROGMEM = "Memory - Start Free Heap";
static const char SYS_fstotal             [] PROGMEM = "Filesystem - Total Bytes";
static const char SYS_fsused              [] PROGMEM = "Filesystem - Used Bytes";
static const char SYS_fsfree              [] PROGMEM = "Filesystem - Free Bytes";
static const char SYS_uptime              [] PROGMEM = "Uptime";
static const char SYS_heapfrag            [] PROGMEM = "Memory - Heap Fragmentation";
static const char SYS_network_currentIp   [] PROGMEM = "Current IP";
static const char SYS_network_hostName    [] PROGMEM = "Hostname";
static const char SYS_network_apHostName  [] PROGMEM = "AP SSID";
static const char SYS_network_apPsk       [] PROGMEM = "AP PSK";
static const char SYS_network_staPsk      [] PROGMEM = "STA PSK";
static const char SYS_network_apIP        [] PROGMEM = "AP IP";
static const char SYS_network_SSID        [] PROGMEM = "STA SSID";
static const char SYS_network_sta         [] PROGMEM = "STA";
static const char SYS_firmware_framework  [] PROGMEM = "Firmware - Framework";
static const char SYS_firmware_platform   [] PROGMEM = "Firmware - Platform";
static const char SYS_firmware_eboard     [] PROGMEM = "Pic - Compilation Board Name";
static const char SYS_firmware_mac        [] PROGMEM = "Pic - Mac Adress";
static const char SYS_firmware_board      [] PROGMEM = "Pic - Board Name";
static const char SYS_firmware_cpu        [] PROGMEM = "Pic - cpu Frequency";
static const char SYS_firmware_ram        [] PROGMEM = "Pic - Ram Size";
static const char SYS_firmware_flash      [] PROGMEM = "Pic - Flash Size";

static const char ALS_BOARD_0[] PROGMEM = "WeMos D1 R2 and mini";
static const char ALS_BOARD_1[] PROGMEM = "WeMos D1 mini Pro";
static const char ALS_BOARD_2[] PROGMEM = "Sonoff S20";
static const char ALS_BOARD_3[] PROGMEM = "Adafruit HUZZAH ESP8266";
static const char* const ALS_BOARD_T[] PROGMEM = {ALS_BOARD_0, ALS_BOARD_1, ALS_BOARD_2, ALS_BOARD_3};

static const char ALS_PLATFORM_0[] PROGMEM = "Espressif 8266 (2.6.3)";
static const char ALS_PLATFORM_1[] PROGMEM = "Espressif 8266 (3.1.0)";
static const char ALS_PLATFORM_2[] PROGMEM = "Espressif 8266 (3.2.0)";
static const char* const ALS_PLATFORM_T[] PROGMEM = {ALS_PLATFORM_0, ALS_PLATFORM_1, ALS_PLATFORM_2};

static const char ALS_FRAMEWORK_0[] PROGMEM = "arduinoespressif8266 3.20704.0 (2.7.4)";
static const char* const ALS_FRAMEWORK_T[] PROGMEM = {ALS_FRAMEWORK_0}; 

static const char ALS_FLASH_0[] PROGMEM = "4";
static const char ALS_FLASH_1[] PROGMEM = "16";
static const char ALS_FLASH_2[] PROGMEM = "1";
static const char* const ALS_FLASH_T[] PROGMEM = {ALS_FLASH_0, ALS_FLASH_1, ALS_FLASH_2};

static const char ALS_RAM_0[] PROGMEM = "80";
static const char* const ALS_RAM_T[] PROGMEM = {ALS_RAM_0};

static const char ALS_CPU_0[] PROGMEM = "80";
static const char ALS_CPU_1[] PROGMEM = "160";
static const char* const ALS_CPU_T[] PROGMEM = {ALS_CPU_0, ALS_CPU_1};  


static const char req_sys     [] PROGMEM = "sys";
static const char req_sys_1   [] PROGMEM = "system";
static const char req_sys_2   [] PROGMEM = "network";
static const char req_sys_3   [] PROGMEM = "firmware";
static const char req_output  [] PROGMEM = "output";

class serverSystem
{
public:
  serverSystem(){};
  ~serverSystem(){};
  void sys_firmWareJson(JsonObject & root);
  void sys_json(JsonObject & root);
  #ifdef WEBSERVEROK
  void sys_networkJson(JsonObject & root);  
  #endif
  void sys_toString( String & out);
  #ifdef WEBSERVEROK
  void sys_networkToString(String & out); 
  #endif
  void sys_fullToString(String & out);
  
};

#endif // SYSTEM_H  
