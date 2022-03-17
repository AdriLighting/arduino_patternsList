#ifndef _QUEUE_H
#define _QUEUE_H 
    #include <Arduino.h>
    #include <resizeBuff.h>
    #include <LList.h>
    #include "TaskScheduler.h"


    class QueueItem {
        char* _mStr;
      public:
        QueueItem();
        ~QueueItem();
        void add(char* inStr);
        void get_string(String & v1);
    };
    class QueueItemList {
      LList<QueueItem *> _list;
    public:
      QueueItemList(){};
      ~QueueItemList(){
        while (_list.size()) {
          QueueItem *eff = _list.shift();
          delete eff;
        }
        _list.clear();
      };
      void addString(String* inStr);
      void addString(char* inStr);  
      uint8_t get_size();
      void execute();  
    };
    
#endif