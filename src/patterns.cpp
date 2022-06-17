#include "../include/patterns.h"
#include <altoolslib.h>

#define DEBUG
#ifndef DEBUG
  #ifdef DEBUG_BASICLIST
    #define DEBUG
  #endif
#endif
#ifdef DEBUG
  #define LOG(func, ...) ALT_TRACEC(func, __VA_ARGS__)
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
  for(int i = 0; i < _cntMax; ++i) {delete _list[i];}
  delete[] _list;
  for(int i = 0; i < _cntMax; ++i) {delete _id[i];}
  delete[] _id;
}
void Listbase::initialize(const uint16_t & maxCnt, const char * const & n){
  LOG(APPT_DEBUGREGION_BASICLIST, "INIT [freeheap: %d]\n", ESP.getFreeHeap());  

  _name       = n;

  if (_list) list_delete();
  _list     = new String*[maxCnt];
  _id       = new uint8_t*[maxCnt];
  for(int i = 0; i < maxCnt; ++i) { _list[i]  = new String;}
  for(int i = 0; i < maxCnt; ++i) { _id[i]    = new uint8_t;}
  _cntMax   = maxCnt;
  _cnt      = 0;
}

void Listbase::list_delete(){
  LOG(APPT_DEBUGREGION_BASICLIST, "BEFOR [freeheap: %d]\n", ESP.getFreeHeap());  
  for(int i = 0; i < _cntMax; ++i) {delete _list[i];}
  delete[] _list;
  _list=nullptr; 
  for(int i = 0; i < _cntMax; ++i) {delete _id[i];}
  delete[] _id;
  _id=nullptr;   
  LOG(APPT_DEBUGREGION_BASICLIST, "AFTER [freeheap: %d]\n", ESP.getFreeHeap());  
}

void Listbase::item_add(const String & value, uint8_t id){
  #ifdef DEBUG
   // LOG(APPT_DEBUGREGION_BASICLIST, "&c:1&s:[Listbase::item_add] pos: %d name: %s\n", _cnt, value.c_str());
  #endif
  *(String*)_list[_cnt] = value;
  *(uint8_t*)_id[_cnt] = id;
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
void Listbase::get_itemIdByPos(uint8_t pos, uint8_t & value) {
  value = *(uint8_t*)_id[pos]; 
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
boolean Listbase::get_itemPosById(uint8_t search, uint16_t & result){
  boolean find = false;
  for (uint16_t i = 0; i < _cntMax; i++) {
    if ( *(uint8_t*)_id[i] == search) {
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
    if ( *(String*)_list[i] == search) {
      find    = true;
      break;
    }
  }
  return find;
}
boolean Listbase::get_item(uint8_t search){
  boolean find = false;
  for (uint16_t i = 0; i < _cntMax; i++) {
    if ( *(uint8_t*)_id[i]== search) {
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
void Listbase::jsonObjectId(JsonObject & root){
  root[F("cmax")]   = _cntMax;
  JsonArray arr = root.createNestedArray(F("items"));  
  for (int i = 0; i < _cntMax; i++) {
    JsonArray arr2 = arr.createNestedArray();  
    arr2.add(*(String*)_list[i]);
    arr2.add(String(*(uint8_t*)_id[i]));
  }
}    
void Listbase::print() {
  Serial.printf_P(PSTR("[Listbase::print][_cnt: %d][_cntMax: %d][_name: %s]\n"), _cnt, _cntMax, _name);
  for (int i = 0; i < _cntMax; ++i) {
    String iName = *(String*)_list[i]; 
    uint8_t iId = *(uint8_t*)_id[i]; 
    Serial.printf_P(PSTR("[%5d] %-40s %-3d\n"), i, iName.c_str(), iId);}
}






