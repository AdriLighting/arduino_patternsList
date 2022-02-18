#include "programme.h"

programme_loop * programmeLoop_ptr;

programme_loop::programme_loop(){
    
}


        
void programme_loop::patternList_initialize(uint8_t maxCnt, String n){
    programmeLoop_ptr = this;
    _playlist       = playlist_instance();
    if (_patterns_list) delete _patterns_list;
    if (_patterns)      delete _patterns;
    _patterns_list  = new pattern_list(maxCnt, n);
    _patterns       = new pattern_loop(_patterns_list);
}
void programme_loop::patternList_getName(String & value){
    _patterns_list->get_name(value);
}
void programme_loop::listReSort(int pos, const String & value){
    _patterns_list->listReSort(pos, value);
}
void programme_loop::patternList_item_add(const String &value){
    uint8_t cnt = _patterns_list->item_add(value);
    _patterns->posMax_set(cnt-1);
}
void programme_loop::patternList_item_new(const String &value, const uint16_t & id){
    _patterns_list->item_new(value, id);
}

void programme_loop::patternList_item_getName(String & value, uint8_t pos){
    _patterns_list->item_getName(value, pos);
}

boolean programme_loop::patternList_itemGetPosByName(const String &search, int & result){
    return _patterns_list->item_getPosByName(search, result);
}
boolean programme_loop::pattern_list_changeEffect(const String &search, int & result){
    if (patternList_itemGetPosByName(search, result)){
        _patterns->pos_set(result);
        return true;
    }
    return false;
}
void programme_loop::patternList_randomEffect(String & ret){
    uint8_t pos=0;
    String value;   
    _patterns->rnd();
    _patterns->pos_get(pos);
    _patterns_list->item_getName(ret, pos);
    // lampServer_instance()->lampEffects_instance()->changeEffectByName(value);
}
boolean programme_loop::pattern_list_changeEffect(boolean dir, String & ret){
    _patterns->pattern_set(dir);
    uint8_t pos;
    _patterns->pos_get(pos);
    String value;
    _patterns_list->item_getName(ret, pos);
    // lampServer_instance()->lampEffects_instance()->changeEffectByName(value);
    return true;
}
void programme_loop::pattern_list_setEffect(boolean dir, uint8_t & ret){
    _patterns->pattern_set(dir);
    _patterns->pos_get(ret);
}
void programme_loop::patternList_print(){
    _patterns_list->print();
}
void programme_loop::patternList_jsonObject(JsonObject & root){
    _patterns_list->jsonObject(root);
}
void programme_loop::patternList_cnt(uint8_t & value) {_patterns_list->list_cnt(value);}


void programme_loop::patternLoop_delay_reset        ()                  {_patterns->delay_reset();}
void programme_loop::patternLoop_delay_get          (uint32_t & value)  {_patterns->delay_get(value);}
void programme_loop::patternLoop_delay_set          (int pos)           {_patterns->delay_set(pos);}
void programme_loop::patternLoop_posMin_get         (uint8_t & value)   {_patterns->posMin_get(value);}
void programme_loop::patternLoop_posMax_get         (uint8_t & value)   {_patterns->posMax_get(value);}
void programme_loop::patternLoop_pos_get            (uint8_t & value)   {_patterns->pos_get(value);}
void programme_loop::patternLoop_pos_set            (uint8_t pos)       {_patterns->pos_set(pos);}




mod_pattern_loop    programme_loop::statuDefault()  {return _patterns->statuDefault();}
mod_pattern_loop    programme_loop::statuNext()     {return _patterns->statuNext();}
boolean             programme_loop::isPlaying()     {return _patterns->isPlaying();}
boolean             programme_loop::isRndPlaying()  {return _patterns->isRndPlaying();}

void programme_loop::play_set                       (boolean pos)       {_patterns->play_set(pos);}
void programme_loop::playRnd_set                    (boolean pos)       {_patterns->playRnd_set(pos);}
void programme_loop::autoPlay () {
    if (isPlaying())    play_set(false);
    else                play_set(true);
}
void programme_loop::autoPlayRnd () {
    if (isRndPlaying()) playRnd_set(false);
    else                playRnd_set(true);
}
void programme_loop::delayCurrent_set   (int pos)           {_patterns->delayCurrent_set(pos);}
void programme_loop::delayManual_set    (boolean pos)       {_patterns->delayManual_set(pos);}
void programme_loop::delayCurrent_get   (uint32_t & value)  {_patterns->delayCurrent_get(value);}
void programme_loop::delayManual_get    (boolean & value)   {_patterns->delayManual_get(value);}

void programme_loop::item_getName(String & value, uint8_t pos){
    if (_playlist->list_statu()) {
        _playlist->item_patternName(pos, value);
        _playlist->item_pos_set(pos);
        
    } else {
        patternList_item_getName(value, pos);
        _patterns->pattern_setPosByPos(value);
    }   
}

void programme_loop::effectRnd(String & name){ 
    if (_playlist->list_statu()) {
        _playlist->item_rnd(name);
    } else {
        _patterns->rnd(name);
    }
}
void programme_loop::effectSet(boolean dir, String & name){
    if (_playlist->list_statu()) {
        if (dir)    _playlist->item_next();
        else        _playlist->item_prev();
        _playlist->item_patternName(name);
    } else {
        uint8_t pos;
        pattern_list_setEffect(dir, pos);
        patternList_item_getName(name, pos);
    }
    aytoDelayReset();
}
void programme_loop::effectCurrentGet(String & ret){
    if (_playlist->list_statu()) {
         _playlist->item_patternName(ret);
    } else {
        uint8_t p;
        patternLoop_pos_get(p);
        patternList_item_getName(ret, p);
    }
}
void programme_loop::aytoDelayReset(){
    (_playlist->list_statu()) ? _playlist->delay_reset() : patternLoop_delay_reset();
}
void programme_loop::remainingTime(uint32_t & v){
    (_playlist->list_statu()) ? _playlist->remainingTime(v) : remainingTime(v);
}
void programme_loop::loop(mod_pattern_loop & mod, String & v1, boolean & playList, boolean delayInMin){
    // boolean _playList;
    playlist_statu(playList);

    if (playList)   _playlist->item_loop(_patterns, mod, v1, delayInMin);
    else            _patterns->loop(mod, v1, delayInMin);
}


programme_loop * programmeLoop_instance(){return programmeLoop_ptr;}
