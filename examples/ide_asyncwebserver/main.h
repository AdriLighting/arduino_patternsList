#ifndef _MAIN_H
#define _MAIN_H 
    #include <Arduino.h>
    #include <resizeBuff.h>
    #include <LList.h>
    #include "TaskScheduler.h"

    // static const char FOPATH_PLAYLISTEFF    [] PROGMEM = "/TEST/eff/";
    // static const char FSLSH                 [] PROGMEM = "/";

    static const char LPNAME_001[] PROGMEM = "Philippe";
    static const char LPNAME_002[] PROGMEM = "Catherine";
    static const char LPNAME_003[] PROGMEM = "Mamie Stella";
    static const char LPNAME_004[] PROGMEM = "Papi Roger";
    static const char LPNAME_005[] PROGMEM = "Mamie Christiane";
    static const char LPNAME_006[] PROGMEM = "Axel";
    static const char LPNAME_007[] PROGMEM = "Eliza";
    static const char LPNAME_008[] PROGMEM = "Zap";
    static const char LPNAME_009[] PROGMEM = "Mystic";
    static const char LPNAME_010[] PROGMEM = "Valentine";
    static const char LPNAME_011[] PROGMEM = "Pépita";
    static const char LPNAME_012[] PROGMEM = "Réglisse(Mon RéRé)";
    static const char LPNAME_013[] PROGMEM = "Crevette(Pot de cole)";
    static const char LPNAME_014[] PROGMEM = "Mimine(MouMoune)";
    static const char* const LPALLNAMES[] PROGMEM = {
    LPNAME_001, LPNAME_002, LPNAME_003, LPNAME_004, LPNAME_005, 
    LPNAME_006, LPNAME_007, LPNAME_008, LPNAME_009, LPNAME_010, 
    LPNAME_011, LPNAME_012, LPNAME_013, LPNAME_014
    };
    static const char* const LPALLNAMES_CAT[] PROGMEM = {
    LPNAME_008, LPNAME_009, LPNAME_010, 
    LPNAME_011, LPNAME_012, LPNAME_013
    };

    class socketQueeItem {
        char* _mStr;
      public:
        socketQueeItem();
        ~socketQueeItem();
        void add(char* inStr);
        void get_string(String & v1);
    };
    class socketQueeItemsList {
      LList<socketQueeItem *> _list;
    public:
      socketQueeItemsList(){};
      ~socketQueeItemsList(){
        while (_list.size()) {
          socketQueeItem *eff = _list.shift();
          delete eff;
        }
        _list.clear();
      };
      void addString(String* inStr);
      void addString(char* inStr);  
      uint8_t get_size();
      void execute();  
    };
    class socketQuee {
        uint32_t              _last_call = 0;
        uint32_t              _last_item = 0;
        boolean               _executeQuee = false;

        // Task                  * _task_quee = nullptr;
        socketQueeItemsList   * _list;

    public:

        socketQuee();
        ~socketQuee();
        void receive(const String &);
        void quee();
        void handle();
    };





/*
    class _socketQueeItemsList {
      uint8_t _cnt=0;
    public:
      _socketQueeItemsList(){_list = new socketQueeItem*[10];};
      ~_socketQueeItemsList(){
      for(int i=0;i<_cnt;i++)
          delete _list[i];
      delete[] _list;
      };
      void addString(String* inStr);
      void addString(char* inStr);  
      socketQueeItem ** _list;
    };

*/
#endif