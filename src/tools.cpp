#include "../include/tools.h"

#include <Arduino.h>

#ifdef FSOK
  #if defined USE_SPIFFS
    #include <FS.h>
  #elif defined USE_LITTLEFS
    #include <LittleFS.h> 
  #endif
#endif


#ifdef DEBUG_AP
  char * ap_debugBuffer = nullptr;
  void AP_debugPrint(const String & msg, const String & file, const String & line, const String & func) {
    Serial.printf_P(PSTR("[%s:%S] %s\n"), file.c_str() , line.c_str() , func.c_str());
    if (msg!="") Serial.printf_P(PSTR("%s"), msg.c_str() ); 
  }
#endif
void define_print(){
  #ifdef DEBUGSERIAL
    Serial.println(F("[DEBUGSERIAL][OK]"));  
  #else
    Serial.println(F("[DEBUGSERIAL][NO]"));  
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
}

String ch_toString(const char * c){
  return String((const __FlashStringHelper*) c);
}

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


/**
 * @brief      split string with sep
 *
 * @param[in]  s      source String
 * @param[in]  sep    separator to split 
 * @param      rSize  size of array by ref
 *
 * @return     array of split values
 */
String * AP_explode(String s, char sep, int & rSize) {
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

  String * list = new String[list_index+1];

  str_index   =0;
  list_index  =0;
  while (str_index < size ) {
    j=t.indexOf(sep,str_index);
    if (j!=-1) {
      sub=t.substring(str_index, j);
      list[list_index]=sub;
      list_index++;
      str_index=j+1;
    }
  }

  list[list_index]="";
  rSize = list_index;

  return list;
}
void millis2time(const uint32_t & s, char * time) {
  uint32_t seconds    = (s /   (1000)           ) % 60    ;
  uint32_t minutes    = (s /   (1000UL*60UL)    ) % 60    ;
  uint32_t hours      = (s /   (1000UL*3600UL)  ) % 24    ;
  uint32_t days       = (s /   (1000UL*3600UL*24UL)  )    ;
  sprintf(time,"%lu-%02lu:%02lu:%02lu", (unsigned long)days, (unsigned long)hours , (unsigned long)minutes, (unsigned long)seconds);
}
void seconds2timeD(unsigned long s, char * time) {
   int seconds    = (s /   (1000)         ) % 60   ;
   int minutes    = (s /   (1000*60)      ) % 60   ;
   int hours      = (s /   (1000*3600)    ) % 24   ;
   sprintf(time,"%02d:%02d:%02d", hours , minutes, seconds);
}
void on_timeD(String & result) {
   char t[100];
   seconds2timeD(millis(), t);
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

// HeapStatu * HeapStatuPtr;
// HeapStatu * HeapStatuPtrGet(){return HeapStatuPtr;};
HeapStatu::HeapStatu() {mod = true;};

void HeapStatu::update(){
  if (mod)  {if (pInitHeap > ESP.getFreeHeap())   tused = (pInitHeap - ESP.getFreeHeap());}
  else      {if (initHeap > ESP.getFreeHeap())    tused = (initHeap - ESP.getFreeHeap());}

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