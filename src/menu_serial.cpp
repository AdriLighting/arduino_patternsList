#include "../include/menu_serial.h"

#ifdef DEBUGSERIAL
#include "../include/tools.h"
#include "../include/program.h"

namespace {
  void splitText(const String & A_readString, const char* sep, String & cmd, String & value) {

    String  s_command;
    String  s_valueCommand;
    String  s_readString = A_readString;
    char    inputChar[s_readString.length() + 1] ;
            s_readString.toCharArray(inputChar, s_readString.length() + 1);
    char    * Rcommand  = strtok(inputChar, sep);

    while (Rcommand != 0){          
        char* separator  = strchr(Rcommand, '=');
        if (separator != 0) {
            *separator  = 0;            
            ++separator ;
            s_command = String(Rcommand);
            s_valueCommand = String(separator); 
            
            if (s_command!=""){
                value = s_valueCommand;
                cmd = s_command;
                break;
            }
        }
        Rcommand = strtok(0, sep); 
    }

  }
}

    Sr_menu _Sr_menu;



Sr_item::Sr_item(){
}
Sr_item::~Sr_item(){}

void Sr_item::set(const char* v1, const char* v2, sr_cb_v_f v3, SR_MM v4) {
  _name   = v1;
  _key    = v2;
  _cb_v   = std::move(v3);   
 _mod     = v4;  
}
void Sr_item::set(const char* v1, const char* v2, sr_cb_ss_f v3, SR_MM v4) {
  _name   = v1;
  _key    = v2;
  _cb_ss  = std::move(v3);   
  _mod    = v4;    
}

void Sr_item::print() {
  Serial.printf_P(PSTR("[name: %s][key: %s]\n"), _name, _key);
}

void Sr_item::get_key(const char * & v1)  {v1 = _key;}
void Sr_item::get_mod(SR_MM & v1)         {v1 = _mod;}

void Sr_item::get_callback(const String & v1, const String & v2){
  switch (_mod) {
    case SR_MM::SRMM_SIMPLE: break;
    case SR_MM::SRMM_KEY: if (_cb_ss) _cb_ss(v1, v2); break;
    default:break;
  }
}
void Sr_item::get_callback(){
  switch (_mod) {
    case SR_MM::SRMM_SIMPLE: if (_cb_v) _cb_v(); break;
    case SR_MM::SRMM_KEY: break;
    default:break;
  }
}


Sr_menu::Sr_menu(){
    _Sr_menu.add("menu",               "a", []() { /*_serial->menu();*/  });
    _Sr_menu.add("ESPreset",           "z", []() { ESP.restart();    });
    _Sr_menu.add("freeHeap",           "e", []() { Serial.printf_P(PSTR("freeHeap: %d\n"), ESP.getFreeHeap()); });
    _Sr_menu.add("debug prog",         "r", []() { ProgramPtrGet()->print(PM_LOOP);  });
    _Sr_menu.add("remote action list", "t", []() { uint8_t cnt = ARRAY_SIZE(RAALLNAMES); for(int i=0; i<cnt; i++){ Serial.printf_P(PSTR("[%-3d][%-25S]\n"), i, RAALLNAMES[i]);}});
    _Sr_menu.add("lb+allpl",           "y", []() { ProgramPtrGet()->print(PM_LB); ProgramPtrGet()->print(PM_PL); });  
}
Sr_menu::~Sr_menu(){}

void Sr_menu::add(const char* v1, const char* v2, sr_cb_v_f v3, SR_MM v4){
  const char  * key = "";
  for(int i = 0; i < _list.size(); ++i) {
    _list[i]->get_key(key);
    if (strcmp(v2, key) == 0) {Serial.printf_P(PSTR("[Sr_menu::add | cb void] key %s already registered!\n"), v2);return;}}
  Serial.printf_P(PSTR("[Sr_menu::add | cb void] adding key: %s\n"), v2);
  _list.add(new Sr_item());
  uint8_t pos = _list.size()-1;
  _list[pos]->set(v1, v2, v3, v4);
}
void Sr_menu::add(const char* v1, const char* v2, sr_cb_ss_f v3, SR_MM v4){
  const char  * key = "";
  for(int i = 0; i < _list.size(); ++i) {
    _list[i]->get_key(key);
    if (strcmp(v2, key) == 0) {Serial.printf_P(PSTR("[Sr_menu::add | cb ss] key %s already registered!\n"), v2);return;}}
  Serial.printf_P(PSTR("[Sr_menu::add | cb ss] adding key: %s\n"), v2);
  _list.add(new Sr_item());
  uint8_t pos = _list.size()-1;
  _list[pos]->set(v1, v2, v3, v4);
}

void Sr_menu::serialRead(){
  if(!Serial.available()) return;
  String str = Serial.readStringUntil('\n');
  serialReadString(str);
}
void Sr_menu::serialReadString(const String & v1){
  if (v1 == "") return;
  char        * keyStr = new char[80];
  uint8_t     size  = _list.size();
  const char  * key = "";
  SR_MM       mod;
  strcpy(keyStr, String(v1[0]).c_str());
  for(int i = 0; i < size; ++i) {
    _list[i]->get_key(key);
    _list[i]->get_mod(mod);
    if (key == (const char*)"") continue; 
    switch (mod) {
      case SR_MM::SRMM_SIMPLE:
        if ( String((const __FlashStringHelper*) key) == v1 ) {_list[i]->get_callback(); } break;
      case SR_MM::SRMM_KEY: 
        if (strcmp(keyStr, key) == 0) {
          {
            String cmd    = "";
            String value  = "";
            splitText(v1, key, cmd, value);
            _list[i]->get_callback(cmd, value);
          }
        }
      break;
      default:break;
    }
  }
  delete keyStr;
  Serial.flush();
}
#endif
