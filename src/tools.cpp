#include "../include/tools.h"
#include "../include/constants.h"

#include <Arduino.h>

#ifdef FSOK
  #if defined USE_SPIFFS
    #include <FS.h>
  #elif defined USE_LITTLEFS
    #include <LittleFS.h> 
  #endif
#endif

#ifdef DEBUG_AP
  DebugPrintList _DebugPrintList;  
#endif

void define_print(){

  #ifdef AP_DEFAULT
    Serial.println(F("[AP_DEFAULT][OK]"));  
  #else
    Serial.println(F("[AP_DEFAULT][NO]"));  
  #endif
  #ifdef DEBUG_KEYBOARD
    Serial.println(F("[DEBUG_KEYBOARD][OK]"));  
  #else
    Serial.println(F("[DEBUG_KEYBOARD][NO]"));  
  #endif
  #ifdef USE_SPIFFS
    Serial.println(F("[USE_SPIFFS][OK]"));  
  #else
    Serial.println(F("[USE_SPIFFS][NO]"));  
  #endif
  #ifdef USE_LITTLEFS
    Serial.println(F("[USE_LITTLEFS][OK]"));  
  #else
    Serial.println(F("[USE_LITTLEFS][NO]"));  
  #endif  
  #ifdef FSOK
    Serial.println(F("[FSOK][OK]"));  
  #else
    Serial.println(F("[FSOK][NO]"));  
  #endif
  #ifdef DEBUG_AP
    Serial.println(F("[DEBUG_AP][OK]"));  
  #else
    Serial.println(F("[DEBUG_AP][NO]"));  
  #endif 
  #ifdef DEBUG_BASICLIST
    Serial.println(F("[DEBUG_BASICLIST][OK]"));  
  #else
    Serial.println(F("[DEBUG_BASICLIST][NO]"));  
  #endif 
  #ifdef DEBUG_PROGRAM
    Serial.println(F("[DEBUG_PROGRAM][OK]"));  
  #else
    Serial.println(F("[DEBUG_PROGRAM][NO]"));  
  #endif 
  #ifdef DEBUG_PLAYLIST
    Serial.println(F("[DEBUG_PLAYLIST][OK]"));  
  #else
    Serial.println(F("[DEBUG_PLAYLIST][NO]"));  
  #endif 
  #ifdef DEBUG_TASK
    Serial.println(F("[DEBUG_TASK][OK]"));  
  #else
    Serial.println(F("[DEBUG_TASK][NO]"));  
  #endif 
  #ifdef DEBUG_WEBSERVER
    Serial.println(F("[DEBUG_WEBSERVER][OK]"));  
  #else
    Serial.println(F("[DEBUG_WEBSERVER][NO]"));  
  #endif 

}

String ch_toString(const char * c){
  return String((const __FlashStringHelper*) c);
}



/**
 * @brief      split string with sep
 *
 * @param[in]  s      source String
 * @param[in]  sep    separator to split
 * @param      rSize  size of array by ref
 *
 * @return     array of split values
 */
void AP_explode(String s, char sep, int & rSize, String * list) {
  String  t           = s + sep;
  int     str_index   = 0;
  int     list_index  = 0;
  int     j;
  String  sub;
  int     size        = t.length();

  while (str_index < size ) {
    j=t.indexOf(sep,str_index);
    if (j!=-1) {
        list_index++;
        str_index=j+1;
    }
  }

  str_index   =0;
  list_index  =0;
  while (str_index < size ) {
    j=t.indexOf(sep,str_index);
    if (j!=-1) {
      sub=t.substring(str_index, j);
      if (list) list[list_index]=sub;
      list_index++;
      str_index=j+1;
    }
  }

  if (list) list[list_index] = "";
  rSize = list_index;
}

void millis2time_m(const uint32_t & s, char * time){
uint32_t milliseconds   =       (s / 1000) % 1000;
uint32_t seconds        = ((    (s / 1000) - milliseconds)/1000)%60;
uint32_t minutes        = ((((  (s / 1000) - milliseconds)/1000) - seconds)/60) %60;
uint32_t hours          = ((((( (s / 1000) - milliseconds)/1000) - seconds)/60) - minutes)/60;  
  sprintf(time,"%lu:%02lu:%02lu:%02lu", (unsigned long)hours, (unsigned long)minutes , (unsigned long)seconds, (unsigned long)milliseconds);

}
void millis2time_d(const uint32_t & s, char * time) {
  uint32_t seconds    = (s /   (1000)           ) % 60    ;
  uint32_t minutes    = (s /   (1000UL*60UL)    ) % 60    ;
  uint32_t hours      = (s /   (1000UL*3600UL)  ) % 24    ;
  uint32_t days       = (s /   (1000UL*3600UL*24UL)  )    ;
  sprintf(time,"%lu-%02lu:%02lu:%02lu", (unsigned long)days, (unsigned long)hours , (unsigned long)minutes, (unsigned long)seconds);
}
void millis2time_h(unsigned long s, char * time) {
  uint32_t seconds    = (s /   (1000)           ) % 60    ;
  uint32_t minutes    = (s /   (1000UL*60UL)    ) % 60    ;
  uint32_t hours      = (s /   (1000UL*3600UL)  ) % 24    ;
  sprintf(time,"%02lu:%02lu:%02lu", (unsigned long)hours , (unsigned long)minutes, (unsigned long)seconds);
}
void on_time_h(String & result) {
   char t[12];
   millis2time_h(millis(), t);
   result = String(t);
}
void on_time_m(String & result) {
   char t[12];
   millis2time_m(millis(), t);
   result = String(t);
}
void on_time_d(String & result) {
   char t[14];
   millis2time_d(millis(), t);
   result = String(t);
}



#ifdef FSOK
bool AP_deserializeFile(DynamicJsonDocument& doc, const char* filepath){
    if (!filepath || !*filepath)
        return false;

    File jfile = FILESYSTEM.open(filepath, "r");
    DeserializationError error;
    if (jfile){
        error = deserializeJson(doc, jfile);
        jfile.close();
    } else {
        return false;
    }

    if (error) {
        // ADRI_LOG(-1,2,2,"\nFile: failed to load json file: %s, deserializeJson error: %d", filepath, error.code());
        // fsprintf("\t");
        // Serial.println(error.code());
        return false;
    }
    // ADRI_LOG(-1,2,2,"\nFile: %s ", filepath);
    return true;
}  
#endif



uint32_t HeapStatu::pInitHeap(0);

HeapStatu::HeapStatu() {mod = true;};

void HeapStatu::update(){
  if (mod)  {if (pInitHeap  > ESP.getFreeHeap()) tused = (pInitHeap - ESP.getFreeHeap());}
  else      {if (initHeap   > ESP.getFreeHeap()) tused = (initHeap  - ESP.getFreeHeap());}

  tcnt++;
  if (tused > 0) ttot+=tused;  
}
void HeapStatu::print(String & ret){
  char buffer[255];
  uint32_t heap;
  if (mod)  heap = pInitHeap;
  else      heap = initHeap;
  sprintf(buffer, "heap-used:%7d |%d/%d|%d| init:%7d free:%7d mod: %d",
    tused,
    ttot,
    tcnt,
    ((ttot/tcnt)>0)?ttot/tcnt:0,
    heap,
    ESP.getFreeHeap(),
    mod
  );
  ret = String(buffer);   
}   
  void splitText(const String & inputString, const char* const & arg,  char sep, LList<SplitItem * > * ptr) {
    char inputChar[inputString.length() + 1] ;
    inputString.toCharArray(inputChar, inputString.length() + 1);

    char * command  = strtok(inputChar, arg);
    while (command != 0){          
      char* separator  = strchr(command, sep);
      if (separator != 0) {
        *separator  = 0;            
        ++separator ;
        // const char * cmd = command;
        // const char * val = separator;
        ptr->add( new SplitItem(String(command), String(separator)));
      }
      command = strtok(0, arg); 
    }
  }

#ifdef DEBUG_AP

char * ap_debugBuffer = nullptr;


uint32_t DebugPrintItem_maxlen_1 = 0;
uint32_t DebugPrintItem_maxlen_2 = 0;

void AP_debugPrint(const String & msg, const String & file, const String & line, const String & func) {
  Serial.printf_P(PSTR("[%s:%S] %s\n"), file.c_str() , line.c_str() , func.c_str());
  if (msg!="") Serial.printf_P(PSTR("%s"), msg.c_str() ); 
}
void AP_debugPrint(const String & msg, const String & file, const String & line, const String & func, const char * _id, AP_DEBUGLVL_T mod) {
  if (_id == (const char *)"") return;

  DebugPrintItem * ptr = _DebugPrintList.get_item(_id);
  if (!ptr) return;
  
  if (!ptr->is_macro()) {/*Serial.printf_P(PSTR("%s"), pMsg.c_str());*/ return;}

  String pMsg = msg;

  if (pMsg!="" && pMsg.substring(0, 1) == "&"){
    LList<SplitItem *>  _SplitItem;
    splitText(pMsg, "&",  ':', &_SplitItem);
    if (_SplitItem.size()>0) {
      if (_SplitItem[0]->_value == "1") {
        Serial.printf_P(PSTR("%s"), _SplitItem[1]->_value.c_str()); 
        while (_SplitItem.size()) {
          SplitItem *eff = _SplitItem.shift();
          delete eff;
        }
        _SplitItem.clear();            
        return; 
      }
    }
  }
  char * b_heap = nullptr;
  if (ptr->is_heap()) {
    String heap = String(ESP.getFreeHeap());
    b_heap = new char[heap.length()+1];
    strcpy(b_heap, heap.c_str());
  }

  char * b_timeStamp = nullptr;
  if (ptr->is_timeStamp()) {
    char t[14];
    millis2time_m(micros(), t);  
    b_timeStamp = new char[sizeof(t)+1];
    strcpy(b_timeStamp, t);
  }

  char * b_file = nullptr;
  if (ptr->is_file()) {
    b_file = new char[file.length()+1];
    strcpy(b_file, file.c_str());
  }

  char * b_line = nullptr;
  if (ptr->is_line()) {
    b_line = new char[line.length()+1];
    sprintf_P(b_line, PSTR("%s"), line.c_str());
  }  

  char * b_func = nullptr;
  if (ptr->is_func()) {
    int rSize = 0;
    AP_explode(func, '(', rSize) ;
    String split[rSize+1];
    AP_explode(func, '(', rSize, split) ;
    // Serial.println("A");
    // for(int i = 0; i < rSize; ++i) {Serial.printf("%d - %s\n", i, split[i].c_str());}
    String funcName = split[0];
    AP_explode(funcName, ' ', rSize) ;
    String split2[rSize+1];
    AP_explode(funcName, ' ', rSize, split2) ;
    // Serial.println("B");
    // for(int i = 0; i < rSize; ++i) {Serial.printf("%d - %s\n", i, split2[i].c_str());}
    if (rSize > 0) {
      funcName = split2[1];
    } else if (rSize == 0) {
      funcName = split2[0];
    }
    b_func = new char[funcName.length()+1];
    sprintf_P(b_func, PSTR("%s"), funcName.c_str());
  }

  char * full = new char[1024];

  strcpy(full, "");

  if (b_timeStamp) {strcat(full, "["); strcat(full, b_timeStamp); strcat(full, "]"); strcat(full, " ");}

  if (b_heap) {strcat(full, "["); strcat(full, b_heap); strcat(full, "]"); strcat(full, " ");}

  strcat(full, "[");
  if (b_line) strcat(full, b_line);
  if (b_file && b_line) strcat(full, ":");
  if (b_file) strcat(full, b_file);
  strcat(full, "]");
  if (b_file || b_line) strcat(full, " ");

  if (b_file)       delete b_file;
  if (b_line)       delete b_line;
  if (b_timeStamp)  delete b_timeStamp;
  if (b_heap)       delete b_heap;

  
  if (pMsg=="") {
    strcat(full, "\n");
    Serial.printf_P(PSTR("%s"), full);
  } else {
    if (ptr->is_crmsg()) {
      Serial.printf_P(PSTR("%s\n%s"), full, pMsg.c_str());
    } else {
        String    s     = String(full);
        uint32_t  size  = s.length();
        if (DebugPrintItem_maxlen_1 < s.length() ) DebugPrintItem_maxlen_1 = s.length() ;
        while (size < DebugPrintItem_maxlen_1) {s += " "; size = s.length();}  
        if (b_func) {
          String s2 = String(b_func) + " ";
          size  = s2.length();
          if (DebugPrintItem_maxlen_2 < s2.length() ) DebugPrintItem_maxlen_2 = s2.length() ;
          while (size < DebugPrintItem_maxlen_2) {s2 += " "; size = s2.length();}  
          Serial.printf_P(PSTR("%s%s%S"), s.c_str(), s2.c_str(), pMsg.c_str());

        } else Serial.printf_P(PSTR("%s%s"), s.c_str(), pMsg.c_str()); 
    }      
  }
  if (full)   delete full;
  if (b_func) delete b_func;
}

AP_DEBUGLVL_T AP_DEBUGLVLARR_T[] = {
  AP_DEBUGLVL_HIGHT,      
  AP_DEBUGLVL_MEDIUM,
  AP_DEBUGLVL_LOW,
  AP_DEBUGLVL_NORMAL     

};

DebugPrintItem::DebugPrintItem(const char * id){
  if (!_id) _id = new char[strlen_P(id)+1];
  strcpy(_id,  id);
};
DebugPrintItem::~DebugPrintItem(){};

void DebugPrintItem::print(){ 
    Serial.printf_P(PSTR("[%12s][%7d][%7d][%7d][%7d][%7d][%7d][%7d][%7s]\n"), 
      _id,
      is_macro(), 
      is_timeStamp(), 
      is_heap(), 
      is_line(), 
      is_file(), 
      is_func(), 
      is_crmsg(),
      FPSTR(APPT_DEBUGREGIONML_ALL[_lvl])
      );
}

void DebugPrintItem::set_macro(boolean v1)      {_p_macro       = v1;}
void DebugPrintItem::set_timeStamp(boolean v1)  {_p_timeStamp   = v1;}
void DebugPrintItem::set_heap(boolean v1)       {_p_heap        = v1;}
void DebugPrintItem::set_line(boolean v1)       {_p_line        = v1;}
void DebugPrintItem::set_file(boolean v1)       {_p_file        = v1;}
void DebugPrintItem::set_func(boolean v1)       {_p_func        = v1;}
// void DebugPrintItem::set_arg(boolean v1)        {_p_arg         = v1;}
void DebugPrintItem::set_crmsg(boolean v1)  {_p_crmsg     = v1;}
void DebugPrintItem::set_lvl(uint8_t v1)        {_lvl = AP_DEBUGLVLARR_T[v1];}


boolean DebugPrintItem::is_macro()              {return _p_macro;}
boolean DebugPrintItem::is_timeStamp()          {return _p_timeStamp;} 
boolean DebugPrintItem::is_heap()               {return _p_heap;}
boolean DebugPrintItem::is_line()               {return _p_line;}
boolean DebugPrintItem::is_file()               {return _p_file;}
boolean DebugPrintItem::is_func()               {return _p_func;}
// boolean DebugPrintItem::is_arg()                {return _p_arg;}
boolean DebugPrintItem::is_crmsg()              {return _p_crmsg;}

AP_DEBUGLVL_T DebugPrintItem::get_lvl()         {return _lvl;}


DebugPrintList::DebugPrintList(){};
DebugPrintList::~DebugPrintList(){};
DebugPrintItem * DebugPrintList::add(const char * const & id){ 
  _list.add(new DebugPrintItem(id));uint8_t pos = _list.size()-1;return _list[pos];
}
DebugPrintItem * DebugPrintList::get_item(const char *  v1){
  char buffId[strlen_P(v1)+1];
  strcpy(buffId,  v1);
  for(int i = 0; i < _list.size(); ++i) {
    DebugPrintItem * item = _list.get(i);
    if (strcmp(buffId, item->_id) == 0) {return item;}          
  }
  return nullptr;
} 

void DebugPrintList::ketboardPrintHeader(boolean pNbId){
  if (!pNbId)   Serial.printf_P(PSTR("[%12s]"), "_id");
  else          Serial.printf_P(PSTR("[%3s][%12s]"), "nb", "_id");
  uint8_t size = ARRAY_SIZE(APPT_DEBUGREGIONMS_ALL);
  for(uint8_t i = 0; i < size; ++i) {
    Serial.printf_P(PSTR("[%d %5s]"),i, FPSTR(APPT_DEBUGREGIONMS_ALL[i]) );
  }   
  Serial.println();          
}
void DebugPrintList::ketboardPrint(){
  Serial.printf_P(PSTR(";&region_id=&setter_id:value\n"));
  Serial.printf_P(PSTR(";&region_id=&range:start,end,value\n"));
  Serial.printf_P(PSTR(";&range:start,end=&setter:value\n"));
  Serial.printf_P(PSTR(";&range:start,end=&range:start,end,value\n"));
  ketboardPrintHeader(true);
  for(uint8_t i = 0; i < _list.size(); ++i) {Serial.printf_P(PSTR("[%3d]"), i);DebugPrintItem * item = _list.get(i);item->print();}
  Serial.println();
}


/*

pattern 
&cmd=&value
&cmd=&value:value_v1
&cmd:cmd_v1,cmd_v2=&value:value_v1,value_v2,value_v3

----------CMD----------  -------------VALUE-------------
;&main                  =&0:0  
& _id                   = & postion : valeur
;&main                  =&range:0,7,0  
& _id                   = & range : start , end , valeur  

;&range:0,7             =&0:0
& range : start , end   = & postion : valeur     
;&range:0,7             =&range:0,7,0 
& range : start , end   = & range : start , end , valeur 

            !!! PAS DE CONTRA SUE LES RANGE DONC FIAR GAFFE AU VALEUR !!!

*/
void  DebugPrintList::keyboardSet(const String & cmd, const String & value){
  Serial.printf_P(PSTR("[keyboardSet] cmd[%s] value[%s]\n"), cmd.c_str(), value.c_str());

  int rSize = 0;
  LList<SplitItem *> _SplitItem;

  splitText(cmd, "&",  ':', &_SplitItem);
  for(int i = 0; i < _SplitItem.size(); ++i) {
    if (_SplitItem[i]->_cmd == FPSTR(APPT_DEBUGREGIONMC_001)) {
      String arg[2];
      AP_explode(_SplitItem[i]->_value, ',', rSize, arg) ;
      ketboardPrintHeader();
      for(int j = arg[0].toInt(); j < arg[1].toInt(); ++j) {
        DebugPrintItem * item = _list.get(j);
        keyboardSet(item, value);
      }
    }
  }
  while (_SplitItem.size()) {
    SplitItem *eff = _SplitItem.shift();
    delete eff;
  }
  _SplitItem.clear();   


  DebugPrintItem * item = get_item(cmd.c_str());
  keyboardSet(item, value);

  DebugPrintItem_maxlen_1 = 0;
  DebugPrintItem_maxlen_2 = 0;
} 
void DebugPrintList::keyboardSet(DebugPrintItem * item, const String & value){
  if (!item) return;

  uint8_t aPos  = 0;
  uint8_t vPos  = 0;
  int     rSize = 0;

  LList<SplitItem *> _SplitItem;
  splitText(value, "&",  ':', &_SplitItem);

  for(int i = 0; i < _SplitItem.size(); ++i) {
    // if (_SplitItem[i]->_cmd == "set") {
      if (_SplitItem[i]->_cmd == FPSTR(APPT_DEBUGREGIONMC_001)) {
        AP_explode(_SplitItem[i]->_value, ',', rSize, nullptr) ;  
        if (rSize < 3) continue;
        String arg[rSize+1];
        AP_explode(_SplitItem[i]->_value, ',', rSize, arg) ;  
        for(int j = arg[0].toInt(); j < arg[1].toInt(); ++j) {
          keyboardSet(item, j, arg[2].toInt());
        }
      } else {
        aPos = _SplitItem[i]->_cmd.toInt();
        vPos = _SplitItem[i]->_value.toInt();
        keyboardSet(item, aPos, vPos);        
      }
    }
  // }
  while (_SplitItem.size()) {
    SplitItem *eff = _SplitItem.shift();
    delete eff;
  }
  _SplitItem.clear();

  item->print();       
}  
void DebugPrintList::keyboardSet(DebugPrintItem * item, uint8_t aPos, uint8_t vPos){ 
  switch (aPos) {
    case 0: item->set_macro(vPos);      break;
    case 1: item->set_timeStamp(vPos);  break;
    case 2: item->set_heap(vPos);       break;
    case 3: item->set_line(vPos);       break;
    case 4: item->set_file(vPos);       break;
    case 5: item->set_func(vPos);       break;
    case 6: item->set_crmsg(vPos);      break;
    case 7: item->set_lvl(vPos);        break;
    default:break;
  }
}


#endif





/*
// fast 8-bit random number generator shamelessly borrowed from FastLED
uint16_t _rand16seed;
uint8_t random8() {
  _rand16seed = (_rand16seed * 2053) + 13849;
  return (uint8_t)((_rand16seed + (_rand16seed >> 8)) & 0xFF);
}

// note random8(lim) generates numbers in the range 0 to (lim -1)
uint8_t random8(uint8_t lim) {
  uint8_t r = random8();
  r = ((uint16_t)r * lim) >> 8;
  return r;
}

uint16_t random16() {
  return (uint16_t)random8() * 256 + random8();
}

// note random16(lim) generates numbers in the range 0 to (lim - 1)
uint16_t random16(uint16_t lim) {
  uint16_t r = random16();
  r = ((uint32_t)r * lim) >> 16;
  return r;
}
uint8_t random8(uint8_t min, uint8_t max) {
  return (random(min, max));
}

uint8_t map8(uint8_t val, uint8_t min, uint8_t max) {
  return map(val, 0, 255, min, max);
}
*/


// char * debug_printBuffer;

// void debugPrint () {
//   Serial.print(debug_printBuffer);
// }




/*
_LLlist::_LLlist () {
  this->m_head = NULL;
  _size = 0;
}

_LLlist::~_LLlist () {
  Node* n = this->m_head;
  while (n) {
    Node *aux = n;
    n = n->next;
    delete aux;
  }
}

void _LLlist::insert (const char * data) {
  Node *n = new Node();
  n->data = data;
  n->next = this->m_head;
  this->m_head = n;
  _size++;
}

void _LLlist::remove (const char * data) {
  Node *curr = this->m_head;
  Node *next = NULL;

  // node to delete is on HEAD
  if (curr && curr->data == data) {
    next = curr->next;
    this->m_head = next;
    delete curr;
    _size--;
    return ;
  }

  // node to delete is after head
  while (curr) {
    next = curr->next;
    if (next && next->data == data) {
      curr->next = next->next;
      delete next;
      _size--;
      return ;
    }
    curr = next;
  }

}

void _LLlist::reverse () {
  Node *curr = this->m_head;
  Node *next = NULL;
  Node *prev = NULL;

  while (curr) {
    next = curr->next;
    curr->next = prev;
    prev = curr;
    curr = next;
  }
  this->m_head = prev;
}

void _LLlist::print () {
  Node *n = this->m_head;
  while (n) {
    Serial.printf("%s\n", n->data);
    n = n->next;
  }
}
void _LLlist::print (uint8_t p, const char * & res) {
  Node *n = this->m_head;
  uint8_t i = 0;
  while (n) {
    if (i == p) {
      res = n->data;
      break;
    }
    i++;
    n = n->next;
  }
}

Node* _LLlist::search (const char * data) {
  Node *n = this->m_head;
  while (n) {
    n = n->next;
    if (n->data == data)
    break;
  }
  return n;
}

*/