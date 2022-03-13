#ifndef _TASKSCHEDULER_H
#define _TASKSCHEDULER_H
#include <Arduino.h>
#include <functional>

#define _TASK_MICRO_RES

#ifndef _TASK_MICRO_RES

#define TASK_MILLISECOND       1UL
#define TASK_SECOND         1000UL
#define TASK_MINUTE        60000UL
#define TASK_HOUR        3600000UL

#else

#define TASK_MILLISECOND    1000UL
#define TASK_SECOND      1000000UL
#define TASK_MINUTE     60000000UL
#define TASK_HOUR     3600000000UL

#endif  // _TASK_MICRO_RES


void _timeStamp(uint32_t timestamp, char * time);


typedef enum : uint8_t {ETD_TIMER=0, ETD_DELAY, ETD_TIMEREND} ETD; 
typedef enum : uint8_t {ETO_S=0, ETO_C, ETO_SC, ETO_SCE, ETO_SE, ETO_CE} ETO; 


class TaskDelay
{
  unsigned long _last =      0;
  uint32_t      _delay      = 0;
  boolean       _enabled    = false; 

public:

  TaskDelay();
  TaskDelay(const uint32_t & max);

  ~TaskDelay();

  boolean execute();  
  // void set_terminate(boolean);

  void set_enabled(boolean);
  void set_delay(const uint32_t &);

  void get_delay(uint32_t &);
  boolean isEnabled();

  void reset();
};

class Task
{
    typedef std::function<void()> TaskCallback;

  ETO _osMode = ETO_S;

  uint8_t _ID = 0;

  uint8_t   _iteration_cnt      = 0;
  int       _iteration_max      = 0;
  boolean   _iteration_next_greater = true;

  boolean   _enabled  = false;

  TaskCallback _callbackOstart  = nullptr;
  TaskCallback _callbackOend    = nullptr;
  TaskCallback _callback        = nullptr;

  boolean iteration_next();

  void setup_timer(const String & v1, boolean v2 = false);

  boolean execute(TaskDelay * ptr);

public:
  TaskDelay * _delay    = nullptr;
  TaskDelay * _timer    = nullptr;
  TaskDelay * _timerEnd = nullptr;

  Task();
  ~Task();

  boolean isEnabled();

  void set_enabled(boolean);

  void setup(boolean v1 = false);

  void execute();
  
  void set_callbackOstart(TaskCallback v1);
  void set_callbackOend(TaskCallback);
  void set_callback(TaskCallback);
  void set_iteration_max(const int&);
  void set_oneShot(ETO);
  void set_iteration_next_greater(boolean);

  void set_taskDelay(TaskDelay *& ptr, boolean v1 = true, const uint32_t & v2 = 0, uint8_t v3 = 0);
  void set_taskDelay(ETD mod, boolean v1 = true, const uint32_t & v2 = 0, uint8_t v3 = 0);
  void set_taskDelayEnabled(TaskDelay *& ptr, boolean v1 = true);
  void set_taskDelayEnabled(ETD mod, boolean v1 = true);

};


class TaskScheduler {
public:
  Task * _TaskArray = nullptr;
  uint8_t _TaskArrayMax = 0;
  uint8_t _TaskArrayCnt = 0;
  TaskScheduler(uint8_t v1);
  ~TaskScheduler();

  void loop();
  void setup(boolean v1 = true);

  Task* get_task(uint8_t);
};

#endif

/*


  void _TaskScheduler_cStart_1(){Serial.printf(">>>[_TaskScheduler_cStart_1]<<<\n");}
  void _TaskScheduler_cStart_2(){Serial.printf(">>>[_TaskScheduler_cStart_2]<<<\n");}
  void _TaskScheduler_cend_1(){
    Serial.printf(">>>[_TaskScheduler_cend_1]<<<\n");
    _TaskScheduler->get_task(1)->set_enabled(true);
    _TaskScheduler->get_task(1)->set_taskDelayEnabled(ETD::ETD_DELAY, true);
  }
  void _TaskScheduler_cend_2(){Serial.printf(">>>[_TaskScheduler_cend_2]<<<\n");}
  void _TaskScheduler_c_1(){Serial.printf(">>>[_TaskScheduler_c_1]<<<\n");}
  void _TaskScheduler_c_2(){Serial.printf(">>>[_TaskScheduler_c_2]<<<\n");}
  _TaskScheduler->get_task(0)->set_callbackOstart(_TaskScheduler_cStart_1);
  _TaskScheduler->get_task(0)->set_callbackOend(_TaskScheduler_cend_1);
  _TaskScheduler->get_task(0)->set_callback(_TaskScheduler_c_1);
  _TaskScheduler->get_task(0)->set_taskDelay(ETD::ETD_TIMER, true, 3000, 1);
  _TaskScheduler->get_task(0)->set_iteration_max(1);
  _TaskScheduler->get_task(0)->setup(true);
  _TaskScheduler->get_task(0)->set_enabled(true);

  _TaskScheduler->get_task(1)->set_callbackOstart(_TaskScheduler_cStart_2);
  _TaskScheduler->get_task(1)->set_callbackOend(_TaskScheduler_cend_2);
  _TaskScheduler->get_task(1)->set_callback(_TaskScheduler_c_2);
  _TaskScheduler->get_task(1)->set_taskDelay(ETD::ETD_TIMER, true, 3000, 1);
  _TaskScheduler->get_task(1)->set_taskDelay(ETD::ETD_DELAY, true, 10000, 1);
  _TaskScheduler->get_task(1)->set_taskDelayEnabled(ETD::ETD_TIMER, false);
  _TaskScheduler->get_task(1)->set_taskDelayEnabled(ETD::ETD_DELAY, true);
  _TaskScheduler->get_task(1)->set_iteration_max(1);
  _TaskScheduler->get_task(1)->set_enabled(false);

*/