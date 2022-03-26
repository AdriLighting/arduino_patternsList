#ifndef TOOLS_H
#define TOOLS_H
  #include <Arduino.h>
  #include <ArduinoJson.h>
  #include <LinkedList.h>
  #include "def.h"

  // extern char * debug_printBuffer;
  // void debugPrint();
  // #define fsprintf(parm_a, ...) {sprintf_P(debug_printBuffer, (PGM_P)PSTR(parm_a), ##__VA_ARGS__); debugPrint();} 

  

  #ifdef FSOK
    bool AP_deserializeFile(DynamicJsonDocument& doc, const char* filepath);  
  #endif

  #ifdef DEBUG_AP

    class DebugPrintItem;

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

    static const char APPT_DEBUGREGIONML_001   [] PROGMEM = "HIGHT";
    static const char APPT_DEBUGREGIONML_002   [] PROGMEM = "MEDIUM";
    static const char APPT_DEBUGREGIONML_003   [] PROGMEM = "LOW";
    static const char APPT_DEBUGREGIONML_004   [] PROGMEM = "NORMAL";
    static const char* const APPT_DEBUGREGIONML_ALL[] PROGMEM = {
      APPT_DEBUGREGIONML_001, APPT_DEBUGREGIONML_002, APPT_DEBUGREGIONML_003, APPT_DEBUGREGIONML_004
    };
    typedef enum : uint8_t  {
      AP_DEBUGLVL_HIGHT = 0, 
      AP_DEBUGLVL_MEDIUM, 
      AP_DEBUGLVL_LOW, 
      AP_DEBUGLVL_NORMAL
    } AP_DEBUGLVL_T; 
    // extern AP_DEBUGLVL_T AP_DEBUGLVLARR_T[];

    void AP_debugPrint(const String & msg, const String & file, const String & line, const String & func);
    void AP_debugPrint(const String & msg, const String & file, const String & line, const String & func, const char * ptr  = "", AP_DEBUGLVL_T mod = AP_DEBUGLVL_NORMAL);

    #define APTRACE(parm_a, ...) { \
      if (ap_debugBuffer) { \
        sprintf_P(ap_debugBuffer, (PGM_P)PSTR(parm_a), ##__VA_ARGS__); \
        AP_debugPrint(String(ap_debugBuffer), String(__FILE__), String(__LINE__), String(ARDUINOTRACE_FUNCTION_NAME));}  \
      } 

    #define APTRACEC(ptr, parm_a, ...) { \
      if (ap_debugBuffer) { \
        sprintf_P(ap_debugBuffer, (PGM_P)PSTR(parm_a), ##__VA_ARGS__); \
        AP_debugPrint(String(ap_debugBuffer), String(__FILE__), String(__LINE__), String(ARDUINOTRACE_FUNCTION_NAME), ptr);} \
    } 


    #define APTRACEM(ptr, mod, parm_a, ...) { \
      if (ap_debugBuffer){ \
        sprintf_P(ap_debugBuffer, (PGM_P)PSTR(parm_a), ##__VA_ARGS__); \
        AP_debugPrint(String(ap_debugBuffer), String(__FILE__), String(__LINE__), String(ARDUINOTRACE_FUNCTION_NAME), ptr, mod);} \
    } 

/*struct debugGlobal_t {
  const char* name = 0;             // Name
#ifdef DEBUG_USE_FLASH_F
  const __FlashStringHelper *nameF = 0;     // Name (in flash)
#endif
  uint8_t type = 0;               // Type of variable (see enum below)
  void *pointer = 0;                // Generic pointer
  uint8_t showLength = 0;             // To show only a part (strings)
#ifndef BOARD_LOW_MEMORY // Not for low memory boards
  const char* description = 0;          // Description
#ifdef DEBUG_USE_FLASH_F
  const __FlashStringHelper *descriptionF = 0;  // Description (in flash)
#endif
#endif
  uint8_t typeOld = 0;              // Type of old value variable (used to strings)
  void *pointerOld = 0;             // Generic pointer for old value
  boolean changed = false;            // Value change (between 2 debug handle call)
  boolean updateOldValue = false;         // Update old value ? (in debug handle call)
};*/
  static const char APPT_DEBUGREGIONMC_001   [] PROGMEM = "range";

  static const char APPT_DEBUGREGIONMS_001   [] PROGMEM = "macro";
  static const char APPT_DEBUGREGIONMS_002   [] PROGMEM = "time";
  static const char APPT_DEBUGREGIONMS_003   [] PROGMEM = "heap";
  static const char APPT_DEBUGREGIONMS_004   [] PROGMEM = "line";
  static const char APPT_DEBUGREGIONMS_005   [] PROGMEM = "file";
  static const char APPT_DEBUGREGIONMS_006   [] PROGMEM = "func";
  static const char APPT_DEBUGREGIONMS_007   [] PROGMEM = "crmsg";
  static const char APPT_DEBUGREGIONMS_008   [] PROGMEM = "lvl";
  static const char* const APPT_DEBUGREGIONMS_ALL[] PROGMEM = {
  APPT_DEBUGREGIONMS_001, APPT_DEBUGREGIONMS_002, APPT_DEBUGREGIONMS_003, APPT_DEBUGREGIONMS_004, APPT_DEBUGREGIONMS_005, APPT_DEBUGREGIONMS_006, APPT_DEBUGREGIONMS_007, APPT_DEBUGREGIONMS_008
  };


    class DebugPrintItem {
      boolean _p_macro     = true;
      boolean _p_timeStamp = true;
      boolean _p_heap      = true;
      boolean _p_line      = true;
      boolean _p_file      = true;
      boolean _p_func      = true;
      boolean _p_crmsg   = false;
      // boolean _p_arg       = true;

      AP_DEBUGLVL_T      _lvl = AP_DEBUGLVL_NORMAL;
    public:
      // const char  * _id = "";
      char * _id = nullptr;
      
      DebugPrintItem(const char * id);
      ~DebugPrintItem();

      void print();

      void set_macro(boolean);
      void set_timeStamp(boolean);
      void set_heap(boolean);
      void set_line(boolean);
      void set_file(boolean);
      void set_func(boolean);
      void set_crmsg(boolean);
      void set_lvl(uint8_t p);

      boolean is_macro();
      boolean is_timeStamp();
      boolean is_heap();
      boolean is_line();
      boolean is_file();
      boolean is_func();
      boolean is_crmsg();  

      AP_DEBUGLVL_T get_lvl();    
    };
    class DebugPrintList
    {
    public:
      LList<DebugPrintItem *>  _list;

      DebugPrintList();
      ~DebugPrintList();
      DebugPrintItem * add(const char * const & id);
      DebugPrintItem * get_item(const char * v1);
      void ketboardPrint();
      void ketboardPrintHeader(boolean pNbId = false);
      void keyboardSet(const String &, const String &);
      void keyboardSet(DebugPrintItem*, const String &);
      void keyboardSet(DebugPrintItem * item, uint8_t aPos, uint8_t vPos);
    };
    extern DebugPrintList _DebugPrintList;

  #else
      #define APTRACE(parm_a, ...) 
      #define APTRACEC(ptr, parm_a, ...) 
  #endif


  class HeapStatu {
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

  class SplitItem {
  public:
    SplitItem(const String  & v1, const String  & v2) {
      if (_cmd)   {delete _cmd;_cmd = nullptr;}
      if (_value) {delete _value;_value = nullptr;}
      _cmd    = new char[v1.length()+1];
      _value  = new char[v2.length()+1];
      strcpy(_cmd,    v1.c_str());
      strcpy(_value,  v2.c_str());
    };
    ~SplitItem(){
      if (_cmd)   {delete _cmd;}
      if (_value) {delete _value;}      
    }
     char* _cmd   = nullptr;
     char* _value = nullptr;

  };
  void splitText(const String & inputString, const char* const & arg,  char sep, LList<SplitItem * > * ptr);


  String ch_toString(const char * c);
  void define_print();
  void on_time_h(String & result);
/*
  int rSize = 0;
  AP_explode(func, '(', rSize, nullptr) ;
  String split[rSize+1];
  AP_explode(func, '(', rSize, split) ;
*/
  void AP_explode(const String & s, char sep, int & rSize, String * list);
/*
  https://stackoverflow.com/questions/16982015/getting-const-char-array-from-function
  String split = "s1,s2,s3";
  int splitSize;
  const char** list = AP_explode(split, ',', splitSize);
  Serial.printf("\n");
  for(int i = 0; i < splitSize; ++i) {Serial.printf("[%d] %s\n", i, list[i]);}
  delete[] list;
*/  
  const char** AP_explode(const String & s, char sep, int & rSize);
  // const char** _AP_explode(String s, char sep, int & rSize);
/*
  class Node {
  public:
    Node* next;
    const char * data;
  };

  class _LLlist {
  public:
    _LLlist();
    ~_LLlist();

    void insert(const char * data);
    void remove(const char * data);
    void print();
    void print(uint8_t p, const char * & res);
    Node* search(const char * data);
    void reverse();

    int _size;

  private:
      Node *m_head;

  };
*/


#endif // TOOLS_H