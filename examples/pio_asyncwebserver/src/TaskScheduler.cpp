#include "TaskScheduler.h"

uint8_t Task_unique_id = 0;

#ifdef _TASK_MICRO_RES

  #undef _TASK_SLEEP_ON_IDLE_RUN     // SLEEP_ON_IDLE has only millisecond resolution
  #define _TASK_TIME_FUNCTION() _task_micros()

#else
  #define _TASK_TIME_FUNCTION() _task_millis()

#endif  // _TASK_MICRO_RES

#ifndef _TASK_EXTERNAL_TIME
  static uint32_t _task_millis() {return millis();}
  static uint32_t _task_micros() {return micros();}
#endif  //  _TASK_EXTERNAL_TIME

#define TASK_DEBUG
#if defined(TASK_DEBUG)
  #define LOG(func, ...) Serial.func(__VA_ARGS__)
#else
  #define LOG(func, ...) ;
#endif

void _timeStamp(uint32_t timestamp, char * time){
uint32_t milliseconds   =       (timestamp / 1000) % 1000;
uint32_t seconds        = ((    (timestamp / 1000) - milliseconds)/1000)%60;
uint32_t minutes        = ((((  (timestamp / 1000) - milliseconds)/1000) - seconds)/60) %60;
uint32_t hours          = ((((( (timestamp / 1000) - milliseconds)/1000) - seconds)/60) - minutes)/60;  
  sprintf(time,"%lu:%02lu:%02lu:%02lu", (unsigned long)hours, (unsigned long)minutes , (unsigned long)seconds, (unsigned long)milliseconds);

}

TaskDelay::TaskDelay(){
  _delay      = 0;
  _last       = 0;  
  _enabled    = true; 
  // _terminate   = false;    
}
TaskDelay::TaskDelay(const uint32_t & max){
  _delay      = max;
  _last       = 0;  
  _enabled    = true; 
  // _terminate   = false;    
}
TaskDelay::~TaskDelay(){}
boolean TaskDelay::execute () {
  if (!_enabled)    return false;
  // if (_terminate)  return false;

  unsigned long now     = _TASK_TIME_FUNCTION();
  unsigned long delta = (now >= _last) ? now - _last : (0xffffffff - _last) + now ;
  boolean done  = (delta >= _delay);

  if (done) {
    // char t[12];
    // _timeStamp(now, t);
    // LOG(printf_P, PSTR("[%-12s][millis: %8d][_delay: %8d][elapsed:%8d]\n"), t, (now/1000), _delay, delta);   
    _last = now;
    return true;
  }
  return false;
}
// void TaskDelay::set_terminate(boolean v1)      {_terminate = v1;}
void TaskDelay::set_enabled(boolean v1)         {_enabled = v1;}
void TaskDelay::set_delay(const uint32_t & v1)  {_delay = v1;}
void TaskDelay::get_delay(uint32_t & v1)        {v1 = _delay;}

boolean TaskDelay::isEnabled()                  {return _enabled;}

void TaskDelay::reset() {_last = _TASK_TIME_FUNCTION();}


Task::Task() {
  if (_ID==0) {
    Task_unique_id++;
    _ID = Task_unique_id;
  } 
}
Task::~Task(){}
boolean Task::execute(TaskDelay * ptr){
  return (ptr && ptr->execute());
}
boolean Task::iteration_next(){
  if (_iteration_next_greater)
    return (_iteration_cnt > _iteration_max);
  else
    return (_iteration_cnt >= _iteration_max);
}
void Task::execute(){
  if (execute(_delay)){
      delete _delay;
      _delay=nullptr;
      setup_timer("[DELAY]", true);
  }
  if (!_timer) {return;}
  if (!_timer->isEnabled()) {return;}

  if (execute(_timerEnd)) {
    LOG(printf_P, PSTR("[ID: %d][Task::execute] [_timerEnd]\n"), _ID);
    if (_timer) _timer->set_enabled(false);
    _timerEnd->set_enabled(false);
    if (_callbackOend) _callbackOend();
  }

  if (execute(_timer)) {
    // LOG(printf_P, PSTR("[ID: %d][Task::execute] [RUNNIG][_iteration_max: %d][_iteration_cnt: %d]\n"), _ID, _iteration_max, _iteration_cnt);

    if (_iteration_max>0) {

      _iteration_cnt++;
      
      if (iteration_next()) {
        _iteration_cnt = 0;
        _timer->set_enabled(false);
        if (_callback && !_iteration_next_greater) {_callback();yield();}
        if (_callbackOend) _callbackOend();
      } else {
        if (_callback) {_callback();yield();}
      }
    } else {
      if (_callback) {_callback();yield();}
    }
  }
  // LOG(printf, "\n---------------\n[loop]\n---------------\n"); 
  // delay(1000); 
}
void Task::setup(boolean v1){
  if (!_delay) {
    LOG(printf_P, PSTR("[ID: %d][Task::setup]\n"), _ID);
    setup_timer("[NODELAY]", v1);
  }
}
void Task::setup_timer(const String & v1, boolean v2){
      LOG(printf_P, PSTR("[ID: %d][Task::setup_timer] %s\n\t[_iteration_max: %d]\n"), _ID, v1.c_str(), _iteration_max);

      if (_iteration_max == 0 ) {
        if (v2){
          LOG(printf, "\t[oneshot]\n");
          switch (_osMode) {
            case ETO_S:   if (_callbackOstart)  _callbackOstart();  break;
            case ETO_C:   if (_callback)        _callback();        break;
            case ETO_SC:  if (_callbackOstart)  _callbackOstart();  if (_callback)      _callback();      break;
            case ETO_SCE: if (_callbackOstart)  _callbackOstart();  if (_callback)      _callback(); if (_callbackOend) _callbackOend(); break;
            case ETO_SE:  if (_callbackOstart)  _callbackOstart();  if (_callbackOend)  _callbackOend();  break;
            case ETO_CE:  if (_callbackOstart)  _callback();        if (_callbackOend)  _callbackOend();  break;
            default: break;
          }         
          if (_timer && _timer->isEnabled()) {
            LOG(printf, "\t[_timer: stop]\n");
            _timer->set_enabled(false);
          }
        }
      } 
      else  {
        if (_callbackOstart)_callbackOstart();
        if (_delay) {
          LOG(printf_P, PSTR("\t[_delay: instanced][starting: %d]\n"), v2);
          _delay->set_enabled(v2);
          _delay->reset();
        } else {
          LOG(printf, "\t[_delay: not instanced]\n");
        }       
        if (_timer) {
          LOG(printf_P, PSTR("\t[_timer: instanced][starting: %d]\n"), v2);
          _timer->set_enabled(v2);
          _timer->reset();
        } else {
          LOG(printf, "\t[_timer: not instanced]\n");
        }
        if (_timerEnd) {
          char t[12];
          char t2[12];
          uint32_t duration;
          _timerEnd->get_delay(duration);
          _timeStamp(_TASK_TIME_FUNCTION()+(duration), t);
          _timeStamp(_TASK_TIME_FUNCTION(), t2);
          LOG(printf_P, PSTR("\t[_timerEnd: instanced][starting: %d][%s -> %s]\n"), v2, t2, t);
          _timerEnd->set_enabled(v2);
          _timerEnd->reset();
        } else {
          LOG(printf, "\t[_timerEnd: not instanced]\n");
        }       
      }
      LOG(println, "---");
}

void Task::set_callbackOstart(TaskCallback v1){_callbackOstart = std::move(v1);}
void Task::set_callbackOend(TaskCallback v1)  {_callbackOend = std::move(v1);}
void Task::set_callback(TaskCallback v1)      {_callback = std::move(v1);}
void Task::set_iteration_max(const int &v1)   {_iteration_max = v1;}
void Task::set_oneShot(ETO v1)                {_osMode = v1;}
void Task::set_iteration_next_greater(boolean v1) {_iteration_next_greater = v1;}

void Task::set_taskDelay(TaskDelay *& ptr, boolean v1, const uint32_t &v2,  uint8_t v3) {
  if (v1){
    if (!ptr) ptr = new TaskDelay(3000);
    uint32_t sDelay = 0;
    if (v3==0) sDelay = v2;
    if (v3==1) sDelay = (v2*1000);
    if (v3==2) sDelay = (v2*1000000);
    (ptr)->set_delay(sDelay);
  } else {
    if (ptr) {
      delete ptr;
      ptr = nullptr;      
    }
  }
}
void Task::set_taskDelay(ETD mod, boolean v1, const uint32_t &v2,  uint8_t v3) {
  switch (mod) {
    case ETD_TIMER:     set_taskDelay(_timer, v1, v2, v3);    break;
    case ETD_DELAY:     set_taskDelay(_delay, v1, v2, v3);    break;
    case ETD_TIMEREND:  set_taskDelay(_timerEnd, v1, v2, v3); break;
    default: break;
  }
}
void Task::set_taskDelayEnabled(TaskDelay *& ptr, boolean v1) {
  if (ptr) {
    (ptr)->set_enabled(v1);
    if (v1) (ptr)->reset();
  }
}
void Task::set_taskDelayEnabled(ETD mod, boolean v1) {
  switch (mod) {
    case ETD_TIMER:     set_taskDelayEnabled(_timer, v1);     break;
    case ETD_DELAY:     set_taskDelayEnabled(_delay, v1);     break;
    case ETD_TIMEREND:  set_taskDelayEnabled(_timerEnd, v1);  break;
    default: break;
  }
}
void Task::set_enabled(boolean v1) {
  _enabled = v1;
}
 
boolean Task::isEnabled() {return _enabled;}

TaskScheduler::TaskScheduler(uint8_t v1) {
  _TaskArrayMax = v1;
  _TaskArray = new Task[v1];
}

TaskScheduler::~TaskScheduler() {}

void TaskScheduler::loop(){
  for(int i=0; i<_TaskArrayMax; i++){if (_TaskArray[i].isEnabled()) _TaskArray[i].execute(); }
}
void TaskScheduler::setup(boolean v1){
  for(int i=0; i<_TaskArrayMax; i++){_TaskArray[i].setup(v1); } 
}

Task * TaskScheduler::get_task(uint8_t p){return &_TaskArray[p];}