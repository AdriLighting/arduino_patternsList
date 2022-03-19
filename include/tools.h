#ifndef TOOLS_H
#define TOOLS_H
  #include <Arduino.h>
  #include <ArduinoJson.h>
  #include "def.h"

  // extern char * debug_printBuffer;
  // void debugPrint();
  // #define fsprintf(parm_a, ...) {sprintf_P(debug_printBuffer, (PGM_P)PSTR(parm_a), ##__VA_ARGS__); debugPrint();} 

  String * AP_explode(String s, char sep, int & rSize);

  #ifdef FSOK
    bool AP_deserializeFile(DynamicJsonDocument& doc, const char* filepath);  
  #endif

  #ifdef DEBUG_AP

    #ifndef ARDUINOTRACE_FUNCTION_NAME_IN_FLASH
      #if defined(ESP8266)
        #define ARDUINOTRACE_FUNCTION_NAME_IN_FLASH 1
      #else
        #define ARDUINOTRACE_FUNCTION_NAME_IN_FLASH 0
      #endif
    #endif

    #if ARDUINOTRACE_FUNCTION_NAME_IN_FLASH
      #define ARDUINOTRACE_FUNCTION_NAME \
        reinterpret_cast<const __FlashStringHelper *>(__PRETTY_FUNCTION__)
    #else
      #define ARDUINOTRACE_FUNCTION_NAME __PRETTY_FUNCTION__
    #endif

    extern char * ap_debugBuffer;


    void AP_debugPrint(const String & msg, const String & file, const String & line, const String & func);

    #define APTRACE_(text)  \
        {                   \
          AP_debugPrint(String(text), String(__FILE__), String(__LINE__), String(ARDUINOTRACE_FUNCTION_NAME)); \
        }
    #define APTRACE(parm_a, ...) {  \
      if (ap_debugBuffer)           \
        sprintf_P(ap_debugBuffer, (PGM_P)PSTR(parm_a), ##__VA_ARGS__); APTRACE_(String(ap_debugBuffer)); \
      }  
  #else
      #define APTRACE(parm_a, ...) 
  #endif

  class HeapStatu
  {
    uint32_t initHeap;
    int tcnt    = 0;
    int ttot    = 0;
    int tused   = 0;    
    boolean mod = true;
  public:

    static uint32_t pInitHeap;
    static void setupHeap_v1() {pInitHeap = ESP.getFreeHeap();};

    void setupHeap_v2() {mod = false; initHeap = ESP.getFreeHeap();};

    HeapStatu();
    ~HeapStatu(){};
    void print(String & ret);
    void update();

  };

  void on_timeD(String & result);
  void millis2time(const uint32_t & s, char * time);
  String ch_toString(const char * c);
  void define_print();
#endif // TOOLS_H