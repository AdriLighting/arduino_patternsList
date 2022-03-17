#include "../include/patterns.h"

// #define DEBUG
#ifndef DEBUG
  #ifdef DEBUG_BASICLIST
    #define DEBUG
  #endif
#endif

Listbase * ListbasePtr = nullptr;
Listbase * ListbasePtrGet(){return ListbasePtr;}
Listbase::Listbase(){   
  ListbasePtr = this;
}
Listbase::~Listbase(){
#ifdef DEBUG
  Serial.printf("Listbase::destructor\n");  
#endif
  delete[] _list;
}
void Listbase::initialize(const uint16_t & maxCnt, const char * const & n){
#ifdef DEBUG
  Serial.printf("Listbase::initialize\n");  
#endif

  _name       = n;

  if (_list) list_delete();
  _list     = new String[maxCnt];

  _cntMax   = maxCnt;
  _cnt      = 0;
}

void Listbase::list_delete(){
  delete[] _list;_list=nullptr;}

void Listbase::item_add(const String & value){
  #ifdef DEBUG
    Serial.printf_P(PSTR("[Listbase::item_add]pos: %d name: %S\n"), _cnt, value.c_str());
  #endif
  _list[_cnt] = value;
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
  p = random(0, _cntMax); value = _list[p];}
void Listbase::get_itemNameByPos(const uint16_t & pos, String & value) {
  value = _list[pos]; }
boolean Listbase::get_itemPosByName(const String & search, uint16_t & result){
  boolean find = false;
  for (uint16_t i = 0; i < _cntMax; i++) {
    if ( _list[i]== search) {
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
    if ( _list[i]== search) {
      find    = true;
      break;
    }
  }
  return find;
}

void Listbase::jsonObject(JsonObject & root){
  root[F("cmax")]   = _cntMax;
  JsonArray arr = root.createNestedArray(F("items"));  
  for (int i = 0; i < _cntMax; i++) {arr.add(_list[i]);}}
void Listbase::print() {
  Serial.printf_P(PSTR("[Listbase::print][_cnt: %d][_cntMax: %d][_name: %s]\n"), _cnt, _cntMax, _name);
  for (int i = 0; i < _cntMax; ++i) {Serial.printf_P(PSTR("[%5d] %s\n"), i, _list[i].c_str());}
}






