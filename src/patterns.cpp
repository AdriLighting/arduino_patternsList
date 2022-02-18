#include "patterns.h"


uint32_t pInitHeap;


void heapStatsPatternLoop_print(String & ret){
    // fsprintfs("\n[heapStatsPatternLoop_print]\n");

    char buffer[255];

    static  int tcnt    = 0;
    static  int ttot    = 0;
    static  int tused   = 0;
            if (pInitHeap > ESP.getFreeHeap())  tused = (pInitHeap - ESP.getFreeHeap());
            tcnt++;
            ttot+=tused;
    sprintf(buffer, "heap-used:%7d |%d/%d|%d| init:%7d free:%7d",
        tused,
        ttot,
        tcnt,
        ttot/tcnt,
        pInitHeap,
        ESP.getFreeHeap()
    );

    ret = String(buffer);   
}

void heapStats_print(){
    Serial.print(F("\n[heapStatsPatternLoop_print]\n"));

    static  int tused   = 0;
            if (pInitHeap > ESP.getFreeHeap())  tused = (pInitHeap - ESP.getFreeHeap());


    Serial.printf_P(PSTR("\tused:%-10d pInitHeap: %-10d getFreeHeap: %-10d\n"),
        tused,
        pInitHeap,
        ESP.getFreeHeap()
    );
}





pattern_list * patternList_ptr;

pattern_list::pattern_list(uint8_t maxCnt, String n){
    patternList_ptr = this;
    _name           = n;
    _listName       = new String[maxCnt];
    _maxcnt         = maxCnt;
}
void pattern_list::get_name(String & value){
    value = _name;
}
uint8_t pattern_list::item_add(const String &value){
    #ifdef PATHEAPSTATS
        heapStatArray[_initialize_cnt] = new heap_stats(value);
    #endif
    #ifdef DEBUG
        Serial.printf_P(PSTR("\n[pattern_list::item_add]pos: %d name: %S\n"), _initialize_cnt, value.c_str());
    #endif
    _listName[_initialize_cnt] = value;
    _initialize_cnt++;
    return _initialize_cnt;
}
void pattern_list::item_new(const String &value, const uint16_t & id){
    _ListName.add(new listItem(value, id));
}
void pattern_list::item_ceck(){



}

void pattern_list::listReSort(int pos, const String & value){
    _listName[pos] = value;
}
void pattern_list::print(){
    Serial.printf_P(PSTR("\n[pattern_list::print][_initialize_cnt: %d][_maxcnt: %d][_name: %s]\n"),_initialize_cnt, _maxcnt, _name.c_str()  );
    for (int i = 0; i < _maxcnt; ++i)
    {
        Serial.printf_P(PSTR("[%5d] - %s\n"), i, _listName[i].c_str());
    }
}

void pattern_list::item_getName(String & value, uint8_t pos){
    value = _listName[pos];
}
void pattern_list::item_getRandomName(String & value){
    value = _listName[random(0, _maxcnt)];
}

int pattern_list::item_getPosByName(const String &search){
    int ret = -1;
    for (int i = 0; i < _maxcnt; i++) {
        if ( _listName[i] == search) {
            ret = i;
            break;
        }
    }
    return ret;
}
boolean pattern_list::item_getPosByName(const String &search, int & result){
    result = -1;
    for (int i = 0; i < _maxcnt; i++) {
        if ( _listName[i] == search) {
            result = i;
            break;
        }
    }
    if (result == -1) return false; else return true;
}

void pattern_list::jsonObject(JsonObject & root){
    for (int i = 0; i < _maxcnt; i++) {
        root[String(i)] = _listName[i] ;
    }
}
void pattern_list::list_cnt(uint8_t & value) {value = _maxcnt;}




int patternList_itemGetPosByName(pattern_list * ptr, const String &search) {return ptr->item_getPosByName(search);}

void            patternList_setPtr(pattern_list * ptr)  {patternList_ptr = ptr;}
pattern_list    * patternList_instance()                {return patternList_ptr;}





pattern_loop * patternLoop_ptr;

pattern_loop    * patternLoop_instance()                {return patternLoop_ptr;}
void            patternLoop_setPtr(pattern_loop * ptr)  {patternLoop_ptr = ptr;}

pattern_loop::pattern_loop(pattern_list * ptr){
    patternLoop_ptr = this;
    _timer          = new adri_timer(_delayCurrent*1000,  true);
    _patterns_list  = ptr;
}

void pattern_loop::remainingTime(uint32_t & v){_timer->get_remainingTime(v);}
void pattern_loop::loop(mod_pattern_loop & mod, String & v1, boolean delayInMin){
    if ( !isPlaying() ) {
        return;
    } 
    if (_timer->loop(_delayCurrent*(delayInMin ? 60000 : 1000))) {
        loop_next();
        String name;
        _patterns_list->item_getName(name, _pos);
        mod     = mpl_next;
        v1      = name;     
    }
}

void pattern_loop::loop_next(){
    if (_random)    rnd();
    else            next();
}
void pattern_loop::next(){
    _pos++;
    if (_pos > _posMax) _pos = _posMin;
}
void pattern_loop::prev(){
    _pos--;
    if (_pos < _posMin) _pos = _posMax;
}
void pattern_loop::rnd(){
    _pos = random(_posMin, _posMax);
    if (_pos > _posMax) _pos = _posMin;
}
void pattern_loop::rnd(String & value){
    _pos = random(_posMin, _posMax);
    if (_pos > _posMax) _pos = _posMin;
    _patterns_list->item_getName(value, _pos);
    _timer->set_duration(millis());
}


void pattern_loop::pattern_set(boolean dir){
    if (dir)    next();
    else        prev();
}
boolean pattern_loop::pattern_setPosByPos(const String & search){
    return _patterns_list->item_getPosByName(search, _pos);
}

boolean pattern_loop::isPlaying()       {return _play;}
boolean pattern_loop::isRndPlaying()    {return _random;}

mod_pattern_loop pattern_loop::statuDefault()   {return mpl_off;}
mod_pattern_loop pattern_loop::statuNext()      {return mpl_next;}

void pattern_loop::pos_set      (uint8_t value){_pos    = value; _timer->set_duration(millis());}
void pattern_loop::posMin_set   (uint8_t value){_posMin = value;}
void pattern_loop::posMax_set   (uint8_t value){_posMax = value;}

void pattern_loop::pos_get      (uint8_t & value){value = _pos;}
void pattern_loop::posMin_get   (uint8_t & value){value = _posMin;}
void pattern_loop::posMax_get   (uint8_t & value){value = _posMax;}

void pattern_loop::delay_reset      ()                      {_timer->set_duration(millis());}
void pattern_loop::delay_get        (uint32_t   & value)    {value = _delay;}
void pattern_loop::delayCurrent_get (uint32_t   & value)    {value = _delayCurrent;}
void pattern_loop::delayManual_get  (boolean    & value)    {value = _delayManual;}

void pattern_loop::play_set             (boolean    value){_play            = value;}
void pattern_loop::playRnd_set          (boolean    value){_random          = value;}

void pattern_loop::delayCurrent_set     (uint32_t   value){_delayCurrent    = value;}
void pattern_loop::delay_set            (uint32_t   value){_delay           = value;}
void pattern_loop::delayManual_set      (boolean    value){_delayManual     = value;}


