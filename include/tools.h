#ifndef TOOLS_H
#define TOOLS_H
  #include <Arduino.h>
  #include <ArduinoJson.h>

  extern char * debug_printBuffer;
  void debugPrint();

  #define fsprintf(parm_a, ...) {sprintf_P(debug_printBuffer, (PGM_P)PSTR(parm_a), ##__VA_ARGS__); debugPrint();} 

  String * LH_explode(String s, char sep, int & rSize);

  #ifdef FSOK
  bool deserializeFile(DynamicJsonDocument& doc, const char* filepath);  
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
#endif // TOOLS_H