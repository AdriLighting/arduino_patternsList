#include "../include/patterns.h"
#include "../include/tools.h"

#define DEBUG
#ifndef DEBUG
  #ifdef DEBUG_BASICLIST
    #define DEBUG
  #endif
#endif
#ifdef DEBUG
  #define LOG(func, ...) APTRACEC(func, __VA_ARGS__)
#else
  #define LOG(func, ...) ;
#endif

Listbase * ListbasePtr = nullptr;
Listbase * ListbasePtrGet(){return ListbasePtr;}
Listbase::Listbase(){   
  ListbasePtr = this;
}
Listbase::~Listbase(){
  LOG(APPT_DEBUGREGION_BASICLIST, "Listbase::destructor\n");   
  delete[] _list;
}
void Listbase::initialize(const uint16_t & maxCnt, const char * const & n){
  LOG(APPT_DEBUGREGION_BASICLIST, "INIT [freeheap: %d]\n", ESP.getFreeHeap());  

  _name       = n;

  if (_list) list_delete();
  _list     = new String*[maxCnt];
  for(int i = 0; i < maxCnt; ++i) { _list[i] = new String;}
  _cntMax   = maxCnt;
  _cnt      = 0;
}

void Listbase::list_delete(){
  LOG(APPT_DEBUGREGION_BASICLIST, "BEFOR [freeheap: %d]\n", ESP.getFreeHeap());  
  for(int i = 0; i < _cntMax; ++i) {delete _list[i];}
  delete[] _list;
  _list=nullptr; 
  LOG(APPT_DEBUGREGION_BASICLIST, "AFTER [freeheap: %d]\n", ESP.getFreeHeap());  
}

void Listbase::item_add(const String & value){
  #ifdef DEBUG
   LOG(APPT_DEBUGREGION_BASICLIST, "&c:1&s:[Listbase::item_add] pos: %d name: %s\n", _cnt, value.c_str());
  #endif
  *(String*)_list[_cnt] = value;
  _cnt++;
}

void Listbase::get_name(const char * & v1) {/*strcpy((char*)v1, _name);*/
  // sprintf(v1, "%s", _name);
  // v1 = ch_toString(_name);
  v1 = _name;
}
void Listbase::get_cnt(uint16_t & v1)           {v1 = _cnt;}
void Listbase::get_cntMax(uint16_t & v1)        {v1 = _cntMax;}

void Listbase::get_itemRnd(String & value, uint16_t & p) {
  p = random(0, _cntMax); value = *(String*)_list[p];
}
void Listbase::get_itemNameByPos(const uint16_t & pos, String & value) {
  value = *(String*)_list[pos]; 
}
boolean Listbase::get_itemPosByName(const String & search, uint16_t & result){
  boolean find = false;
  for (uint16_t i = 0; i < _cntMax; i++) {
    if ( *(String*)_list[i]== search) {
      result  = i;
      find    = true;
      break;
    }
  }
  return find;
}
boolean Listbase::get_item(const String & search){
  boolean find = false;
  for (uint16_t i = 0; i < _cntMax; i++) {
    if ( *(String*)_list[i]== search) {
      find    = true;
      break;
    }
  }
  return find;
}

void Listbase::jsonObject(JsonObject & root){
  root[F("cmax")]   = _cntMax;
  JsonArray arr = root.createNestedArray(F("items"));  
  for (int i = 0; i < _cntMax; i++) {arr.add(*(String*)_list[i]);}}
void Listbase::print() {
  Serial.printf_P(PSTR("[Listbase::print][_cnt: %d][_cntMax: %d][_name: %s]\n"), _cnt, _cntMax, _name);
  for (int i = 0; i < _cntMax; ++i) {String iName = *(String*)_list[i]; Serial.printf_P(PSTR("[%5d] %s\n"), i, iName.c_str());}
}






