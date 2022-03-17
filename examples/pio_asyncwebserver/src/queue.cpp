#include "queue.h"

QueueItem::QueueItem() {
}

void QueueItem::add(char* inStr) {
   _mStr = NULL;                              // Defualt our dynamic memory pointer to NULL
   if (resizeBuff(strlen(inStr)+1,&_mStr)) {  // IF we were able to allocate enough RAM for holding our string..
      strcpy(_mStr,inStr);                    // Stuff a copy of what we got into our object.
   }
}

QueueItem::~QueueItem(void)  { resizeBuff(0,&_mStr); }

void QueueItem::get_string(String & v1){v1 = String(_mStr);}

void QueueItemList::addString(String* inStr) {
   char buffer[255]; 
   sprintf(buffer, "%s", inStr->c_str()); 
   addString(buffer); 
}

void QueueItemList::addString(char* inStr) {
  // Serial.printf_P(PSTR("size: %d\n"), _list.size());
  _list.add(new QueueItem());
  uint8_t pos = _list.size()-1;
  _list[pos]->add(inStr);
}

uint8_t QueueItemList::get_size(){return _list.size();}

void QueueItemList::execute(){
  if (_list.size() == 0) return;
  
  String sT = "";
  _list[0]->get_string(sT);
  // _Webserver.socket_send(sT);

  delete _list.remove(0);

  if (_list.size() == 0) return;

  // _TaskScheduler->get_task(3)->set_callbackOstart([=](){execute();});
  // _socket_task_start(500); 
}