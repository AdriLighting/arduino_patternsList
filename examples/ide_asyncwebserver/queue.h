#ifndef _QUEUE_H
#define _QUEUE_H 
    #include <Arduino.h>
    #include <resizeBuff.h>
    #include <LinkedList.h>
    #include "TaskScheduler.h"



    class QueueItem {
        char* _mStr;
      public:
        QueueItem();
        ~QueueItem();
        void add(char* inStr);
        void get_string(String &);
    };
    class QueueItemList {
      typedef std::function<void(const String &)> _execute_callback_f;

      LList<QueueItem *>  _list;
      
      Task*               _task = nullptr;
      uint32_t            _taskDelay = 500;

      _execute_callback_f _execute_callback = nullptr;
    public:
      QueueItemList(){};
      ~QueueItemList(){
        while (_list.size()) {
          QueueItem *eff = _list.shift();
          delete eff;
        }
        _list.clear();
      };

      void addString(String*);
      void addString(char*); 

      uint8_t get_size();

      void set_callback(_execute_callback_f);
      void set_task(Task *);

      void execute_cbTask();  
    };
    
#endif