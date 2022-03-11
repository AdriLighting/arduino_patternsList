#include "../include/program.h"

#ifdef FSOK
  #if defined USE_SPIFFS
    #include <FS.h>
  #elif defined USE_LITTLEFS
    #include <LittleFS.h> 
  #endif
#endif

#include "../include/Stringsort.h"
#include "LList/LList.h"
#include "../include/webserverRequest.h"


// #define DEBUG

LList<ListSortItems*> ListSortItemsPtr; 
void ListSortItems_sort(SORT_TYPE _effSort) {
  switch(_effSort){
    case SORT_TYPE::ST_BASE :
      ListSortItemsPtr.sort([](ListSortItems *&a, ListSortItems *&b){ return (((a->_id&0xFF) - (b->_id&0xFF))<<8) + (((a->_id&0xFF00) - (b->_id&0xFF00))>>8);});
    break;
    case SORT_TYPE::ST_END :
      ListSortItemsPtr.sort([](ListSortItems *&a, ListSortItems *&b){ return b->_id - a->_id;}); 
    break;
    case SORT_TYPE::ST_IDX :
      // ListSortItemsPtr.sort([](ListSortItems *&a, ListSortItems *&b){ return (int)(a->getMS() - b->getMS());});
    break;
    case SORT_TYPE::ST_AB2 :
    case SORT_TYPE::ST_AB :
    break;
    default:
    break;
  }
}
void ListSortItems_print(){
  for(int i=0; i<ListSortItemsPtr.size(); i++){ Serial.printf_P(PSTR("[%-3d][pos: %3d][name: %S]\n"), i, ListSortItemsPtr[i]->_id, ListSortItemsPtr[i]->_name.c_str());}  
}
void ListSortItems_delete(){
  while (ListSortItemsPtr.size()) {
    ListSortItems *eff = ListSortItemsPtr.shift();
    delete eff;
  }
  ListSortItemsPtr.clear();  
}
/*
  Serial.printf_P(PSTR(">>>>%-15d\n"), ESP.getFreeHeap());   
  const char * const * arr= LPALLNAMES;
  uint8_t cnt = ARRAY_SIZE(LPALLNAMES);
  // PGM_P _menu_labels = *LPALLNAMES;
  char buf[20];
  for(uint8_t i=0; i<cnt; i++){ 
    const char * menu_pgm_ptr = arr[i];
    // String s = FPSTR(menu_pgm_ptr[i]);
    strcpy_P(buf,menu_pgm_ptr);
    Serial.println(buf);
  }    
  Serial.printf_P(PSTR(">>>>%-15d\n"), ESP.getFreeHeap()); 
*/
void LBnames::setup(const char * name,  uint8_t items, const char * const * arr){
  _name   = name;
  _cnt    = items; 
  _arr    = arr;
}
void LBnames::print(uint8_t p){
  char buf[80];
  Serial.printf_P(PSTR("[LBnames::print][%d]]\n"), p);
  for(int i=0; i<_cnt; i++){ 
    const char * menu_pgm_ptr = _arr[i];
    strcpy_P(buf,menu_pgm_ptr);
    Serial.printf_P(PSTR("[%5d] %S\n"), i, buf);}  
}
void LBnames::json(JsonArray & arr){
  char buf[80];
  for(int i=0; i<_cnt; i++) {
    const char * menu_pgm_ptr = _arr[i];
    strcpy_P(buf,menu_pgm_ptr);
    arr.add(buf);
  }

}

Program * ProgramPtr = nullptr;
Program * ProgramPtrGet(){return ProgramPtr;}
Program::Program(uint8_t nbLB, boolean fs){
  ProgramPtr = this;

  #ifdef FSOK
    if (!fs) {
      _fs_setup = FILESYSTEM.begin();
    } else {
      _fs_setup = fs;
    }

  #endif

  _plStatu.cnt          = 0;
  _plStatu.pos          = 0;
  _plStatu.isSet        = false;
  _plStatu.isPlaying    = false;

  _lbStatu.cnt          = nbLB;
  _lbStatu.pos          = 0;  
  _LBnames = new LBnames[nbLB];
}
Program::~Program(){
#ifdef DEBUG
   Serial.printf("Program::destructor\n");  
 #endif 
  delete[] _Playlists;    
  delete[] _LBnames;    
}
void Program::set_fs_pl(boolean v) {
  #ifdef FSOK
    if (!_fs_setup) _fs_pl = false; else _fs_pl = v;
  #else 
    _fs_setup = false;
    _fs_pl    = false;
  #endif
};

void Program::initialize_lb(uint8_t p, const char * name, uint8_t items, const char * const * arr){
  _LBnames[p].setup(name, items, arr);
}
void Program::lb_print() {
  Serial.printf_P(PSTR("[Program::lb_print][%d]]\n"), _lbStatu.cnt);
  for(uint8_t i=0; i<_lbStatu.cnt; i++) {_LBnames[i].print(i); }
}
void Program::get_json_lb_items(JsonObject & doc, uint8_t p, boolean pItems) {
  doc[F("n")]     = _LBnames[p]._name;
  doc[F("cmax")]  = _LBnames[p]._cnt;
  if (!pItems) return;
  JsonArray arr = doc.createNestedArray(F("items"));
  _LBnames[p].json(arr);
}
void Program::get_json_lb_items(JsonObject & doc, boolean pItems) {
  doc[F("cmax")] = _lbStatu.cnt;
  JsonArray arr = doc.createNestedArray(F("items"));
  for(uint8_t i=0; i<_lbStatu.cnt; i++) {
    // JsonObject var = doc.createNestedObject(String(i));
    JsonObject var = arr.createNestedObject();    
    get_json_lb_items(var, i, pItems); 
  }
}

void Program::print(PRINT_MOD mod){
  const char * pN;
  uint16_t pC, pCmax;
  String ilbN;
  get_itemNameByPos(_lbtFlag._pos, ilbN);
  get_name(pN);
  get_cnt(pC);
  get_cntMax(pCmax);
  uint32_t tR, del;
  boolean delMin;
  boolean tPlay = isPlaying();
  boolean tPause = isPause();
  boolean tRnd = isRndPlaying();
  get_remainingTime(tR);
  get_delay(del);
  get_delayMin(delMin);
  tR = (del*(delMin ? 60000 : 1000))- tR;
  switch (mod) {
    case PM_LOOP:
      Serial.printf_P(PSTR("[pl] isSet: %d - isPLaying: %d playlist[%d/%d] timer[pos: %d - min: %d - max: %d]\n"),
        _plStatu.isSet, _plStatu.isPlaying, _plStatu.pos, _plStatu.cnt,  
        _pltFlag._pos,  _pltFlag._posMin,  _pltFlag._posMax);
      Serial.printf_P(PSTR("[lb] name: %s items[%d/%d] timer[pos: %d - min: %d - max: %d][iName: %s]\n"), 
        pN,pC,pCmax,
        _lbtFlag._pos,  _lbtFlag._posMin,  _lbtFlag._posMax, ilbN.c_str()); 
      Serial.printf_P(PSTR("[lloop] play: %d - pause: %d - rnd: %d - rt: %d\n"), tPlay, tPause, tRnd, tR);             
    break;
    case PM_LB:
      Listbase::print();
    break;
    case PM_LBNAME:
      lb_print();
    break;
    case PM_PL:
      pl_print();
    break;
    case PM_LL:
      Serial.printf_P(PSTR("\t[lloop] play: %d - pause: %d - rnd: %d - rt: %d\n"), tPlay, tPause, tRnd, tR);  
    break;
    case PM_LLI:
      Serial.printf_P(PSTR("\t[pl] isSet: %d - isPLaying: %d playlist[%d/%d] timer[pos: %d - min: %d - max: %d]\n"),
        _plStatu.isSet, _plStatu.isPlaying, _plStatu.pos, _plStatu.cnt,  
        _pltFlag._pos,  _pltFlag._posMin,  _pltFlag._posMax);
      Serial.printf_P(PSTR("\t[lb] name: %s items[%d/%d] timer[pos: %d - min: %d - max: %d]\n"), 
        pN,pC,pCmax,
        _lbtFlag._pos,  _lbtFlag._posMin,  _lbtFlag._posMax); 
    break;
    default:
    break;
  }  
}

void Program::get_json_statu(JsonObject & obj){
  uint32_t tR, del;
  get_remainingTime(tR); 
  boolean tPlay   = isPlaying();
  boolean tPause  = isPause();
  boolean tRnd    = isRndPlaying();  
  boolean delMin;
  get_delay(del);
  get_delayMin(delMin);
  tR = (del*(delMin ? 60000 : 1000))- tR;

  obj[F("play")]      = tPlay;
  obj[F("pause")]     = tPause;
  obj[F("rnd")]       = tRnd;
  obj[F("delay")]     = del;
  obj[F("delaymin")]  = delMin;
  obj[F("rt")]        = tR;  
}
void Program::get_json_pl(JsonObject & obj){
  String iplNiRef;
  _Playlists[_plStatu.pos].get_itemBaseByArrayPos(_pltFlag._pos, iplNiRef);
  obj[F("set")]       = _plStatu.isSet;
  obj[F("play")]      = _plStatu.isPlaying;
  obj[F("pos")]       = _plStatu.pos;
  obj[F("cnt")]       = _plStatu.cnt;
  obj[F("ib")]        = iplNiRef;
}
void Program::get_json_plt(JsonObject & obj){
  obj[F("pos")]       = _pltFlag._pos;
  obj[F("min")]       = _pltFlag._posMin;
  obj[F("max")]       = _pltFlag._posMax;
}
void Program::get_json_lb(JsonObject & obj){
  const char * lbN;
  String ilbN;
  uint16_t pC, pCmax;
  get_name(lbN);
  get_itemNameByPos(_lbtFlag._pos, ilbN);
  get_cnt(pC);
  get_cntMax(pCmax); 
  
  obj[F("name")]      = lbN;
  obj[F("pos")]       = _lbStatu.pos;
  obj[F("cnt")]       = _lbStatu.cnt;
  obj[F("iname")]     = ilbN;
  obj[F("icnt")]      = pC;
  obj[F("icmax")]     = pCmax;
}
void Program::get_json_lbt(JsonObject & obj){
  obj[F("pos")]       = _lbtFlag._pos;
  obj[F("min")]       = _lbtFlag._posMin;
  obj[F("max")]       = _lbtFlag._posMax;
}
void Program::get_json_jsInit(JsonObject & doc){
  JsonObject obj;

  obj = doc.createNestedObject(F("statu"));
  get_json_statu(obj);

  obj = doc.createNestedObject(F("pl"));
  get_json_pl(obj);

  obj = doc.createNestedObject(F("plt"));
  get_json_plt(obj);

  obj = doc.createNestedObject(F("lb"));
  get_json_lb(obj);

  obj = doc.createNestedObject(F("lbt"));
  get_json_lbt(obj);

}
void Program::initialize(const uint16_t & maxCnt, const char* const* arr, const char  * const & n, SORT_TYPE t){
#ifdef DEBUG
  Serial.printf("Program::initialize\n");  
#endif

  Listbase::initialize(maxCnt, n);

  stringList aStringList;   // An instance of the stringList class we created above.

  switch (t) {
      case SORT_TYPE::ST_AB:
        for (int i = 0; i < maxCnt; ++i) {
          String s = FPSTR(arr[i]);
          aStringList.addString(&s);
        }
        aStringList.printStrings();                
        aStringList.dumpList();               
      break;
      default:
        for (int i = 0; i < maxCnt; ++i) {ListSortItemsPtr.add(new ListSortItems(FPSTR(arr[i]), i));} 
        ListSortItems_sort(t);
        for (int i = 0; i < ListSortItemsPtr.size(); ++i) {item_add(ListSortItemsPtr[i]->_name.c_str());}  
        ListSortItems_delete();
      break;
  }

  uint16_t pC; 
  Listbase::get_cnt(pC);
  ListLoop::setup(&_lbtFlag, pC-1);
}



void Program::get_itemPosBase(uint16_t & v1){   
  String name = "";
  get_itemBase(name);
  (_plStatu.isPlaying && _plStatu.isSet) ? get_itemPosByName(name, v1) : v1 = _lbtFlag._pos; 
}
void Program::get_itemPos(uint16_t & v1){ 
  v1 = (_plStatu.isPlaying && _plStatu.isSet) ? _pltFlag._pos : _lbtFlag._pos;
}
void Program::get_itemBase(String & v1){ 
  if (_plStatu.isPlaying) {_Playlists[_plStatu.pos].get_itemBaseByArrayPos(_pltFlag._pos, v1);} 
  else {get_itemNameByPos(_lbtFlag._pos, v1);}  
}
void Program::set_itemNext(){ 
  (_plStatu.isPlaying && _plStatu.isSet) ? loop_next(&_pltFlag) : loop_next(&_lbtFlag);
  reset();
}
void Program::set_itemPrev(){ 
  (_plStatu.isPlaying && _plStatu.isSet) ? loop_prev(&_pltFlag) : loop_prev(&_lbtFlag);
  reset();
}
void Program::set_itemRnd(){ 
  (_plStatu.isPlaying && _plStatu.isSet) ? rnd(&_pltFlag) : rnd(&_lbtFlag);
  reset();
}
void Program::set_itemPos(const uint16_t & p){ 
  uint16_t sP = p;  
  if (_plStatu.isPlaying && _plStatu.isSet) {
    if (sP > _pltFlag._posMax) sP = _pltFlag._posMin;
    if (sP < _pltFlag._posMin) sP = _pltFlag._posMax;
    _pltFlag._pos = sP;
  } else {
    if (sP > _lbtFlag._posMax) sP = _lbtFlag._posMin;
    if (sP < _lbtFlag._posMin) sP = _lbtFlag._posMax;  
    _lbtFlag._pos = sP;  
  }
  reset();
}


void Program::handle(){
  if ( !isPlaying() ) {
    return;
  } 
  uint32_t    del;
  boolean     delMin;
  get_delay(del);
  get_delayMin(delMin);
  if (_timer->loop(del*(delMin ? 60000 : 1000))) {
    String    name;
    uint16_t  pB = 0;

    set_itemNext();

    get_itemBase(name);
    get_itemPosBase(pB);

    if(_callback)_callback(name, pB, true);

  }
}
void Program::set_callback(callback_function_t f)  {_callback = std::move(f);}

void Program::remote_action(RA action, ...){
  #ifdef DEBUG
    Serial.printf_P(PSTR("RA %-4d[%-20s]: "), action, RAALLNAMES[action]);  
  #endif
  char *key = NULL, *val = NULL, *key_val = NULL;
  va_list prm;
  va_start(prm, action);
  while ((key_val = (char *)va_arg(prm, char *)) && (val = (char *)va_arg(prm, char *))) {
     key = key_val;
  }
  va_end(prm);
  #ifdef DEBUG
    Serial.println("");  
    if(String(key) != "") Serial.printf_P( PSTR("\t[key: %s]\n"), key);  
    if(String(val) != "") Serial.printf_P( PSTR("\t[val: %s]\n"), val);     
  #endif

  const char * pN;

  switch (action) { 
    case RA::RA_ITEM:             if(String(key)!="") set_itemPos((uint16_t)atoi(key)); break;;
    case RA::RA_ITEM_NEXT:        set_itemNext();   break;
    case RA::RA_ITEM_PREV:        set_itemPrev();   break;
    case RA::RA_ITEM_RND:         set_itemRnd();    break;

    case RA::RA_PLAY_START:       start();  print(PM_LL);   break;
    case RA::RA_PLAY_STOP:        stop();   print(PM_LL);   break;
    case RA::RA_PLAY_PAUSE:       pause();  print(PM_LL);   break;
    case RA::RA_PLAY_TOGGLE:      (isPlaying()) ? pause(): start(); break;
    case RA::RA_PLAY_DELAY:       if(String(key)!="") set_delayCurrent((uint8_t)atoi(key)); break;
    case RA::RA_PLAY_DELAYMIN:    set_delayMin();                                     break;
    case RA::RA_PLAY_DELAYMINON:  set_delayMin(true);                                 break;
    case RA::RA_PLAY_DELAYMINOFF: set_delayMin(false);                                break;
    case RA::RA_PLAY_RND:         set_rnd();                                          break;

    case RA::RA_PLAY_PL:          if (_plStatu.isSet) _plStatu.isPlaying = true;      break;
    case RA::RA_PLAY_LB:          _plStatu.isPlaying = false;                         break;
    case RA::RA_PLAY_LT:          _plStatu.isPlaying = !_plStatu.isPlaying ;          break;
  
    case RA::RA_LSET_PL:
      if(String(key) != ""){
        get_name(pN);
        pl_set_listPos((uint8_t)atoi(key), pN);
        reset();}
    break;
    case RA::RA_LGET_PL:
    break;
    case RA::RA_PLI_NEW: if(String(key) != "") pl_item_new((uint8_t)atoi(key)); break;    
    case RA::RA_PLI_REP: if((String(key) != "") && (String(val) != ""))pl_item_new((uint8_t)atoi(key), (uint8_t)atoi(val)); break;    
    case RA::RA_PLI_REM: if((String(key) != "") && (String(val) != ""))pl_item_remove((uint8_t)atoi(key), (uint8_t)atoi(val)); break;    
    case RA::RA_PL_TOFS:
    #ifdef FSOK
      if(String(key) != "") pl_fs((uint8_t)atoi(key)); 
    #endif
    break;    

    default:break;
  }

  // USER CALLBACK
  switch (action) { 
    case RA::RA_ITEM:       ;
    case RA::RA_ITEM_NEXT:  ;
    case RA::RA_ITEM_PREV:  ;
    case RA::RA_ITEM_RND:   
    {
      if(_callback) {
        String    name;
        uint16_t  pB = 0;
        get_itemBase(name);
        get_itemPosBase(pB);
        _callback(name, pB, (String(val) == "upd" )?true:false);}
    }
    break;
    default:break;
  }  
}




void Program::initialize_playlist(uint8_t lCount,  const uint8_t * const &Icount,  const char ** const &Lname) {
  _plStatu.cnt   = lCount;
  _Playlists      = new Playlist_list[lCount];
  for(uint8_t i=0; i<_plStatu.cnt; i++){
    uint8_t c = Icount[i];
    _Playlists[i].set_items_max(c);
    _Playlists[i].set_listRef(Lname[i]);
    _Playlists[i].set_pos(i);
    _Playlists[i].set_lbl("playlist_" + String(i));
    _Playlists[i]._Playlist_itemArray = new Playlist_item[c];
    _Playlists[i].item_df();    
  }
}

// boolean Program::get_itemIdByArrayPos(uint8_t pP, uint8_t aP, uint8_t & result) { 
//   return _Playlists[pP].get_itemIdByArrayPos(aP, result);
// }
boolean Program::pl_set_listPos(uint8_t pos, const char * currentList){
  const char * sLref;
  _Playlists[pos].get_listRef(sLref);

  if (_plStatu.cnt == 0) return false;

  if (pos >= _plStatu.cnt) {
    #ifdef DEBUG
      Serial.printf_P(PSTR("[Program::list_pos][EROR postion][requete: %d >= %d]\n"),pos ,_plStatu.cnt);  
    #endif
    _plStatu.isPlaying =false; 
    _plStatu.isSet =false; 
    return false;
  }
  if (sLref != currentList) {
    #ifdef DEBUG
      Serial.printf_P(PSTR("[Program::list_pos][EROR listeBase][pos: %d][requete: %s <> %s]\n"),pos, sLref ,currentList);  
    #endif
    _plStatu.isPlaying =false; 
    _plStatu.isSet =false; 
    return false;
  }

  uint16_t pC = 0;
  _Playlists[pos].get_items_cnt(pC);

  if (pC == 0) {
    #ifdef DEBUG
      Serial.printf_P(PSTR("[Program::list_pos][EROR nb of item low: %d]\n"),pC);  
    #endif
    _plStatu.isPlaying =false; 
    _plStatu.isSet =false; 
    return false;
  }

  _plStatu.isSet      = true;
  _plStatu.pos        = pos;

  ListLoop::setup(&_pltFlag, pC-1);

  return true;
}

void Program::pl_item_new(uint8_t pP) {
  String ilbN;
  get_itemNameByPos(_lbtFlag._pos, ilbN);

  pl_item_toArray(pP, 255, ilbN, ilbN, ilbN); 

  uint16_t pC = 0;
  _Playlists[_plStatu.pos].get_items_cnt(pC);
  ListLoop::updatePos(&_pltFlag, pC-1);

  JsonObject root;
  DynamicJsonDocument temp(2048);
  DynamicJsonDocument reponse(2048);
  reponse.createNestedObject(FPSTR(REP_007));
  root = temp.to<JsonObject>();   
  get_json_pl_items(pP, root, true);
  reponse[FPSTR(REP_007)] = temp;

  #ifdef FSOK
    // pl_fs(pP, reponse);  
  #endif
}
void Program::pl_item_new(uint8_t pP, uint8_t iP) {
  String ilbN;
  get_itemNameByPos(_lbtFlag._pos, ilbN);

  pl_item_toArray(pP, iP, ilbN, ilbN, ilbN); 

  uint16_t pC = 0;
  _Playlists[_plStatu.pos].get_items_cnt(pC);
  ListLoop::updatePos(&_pltFlag, pC-1);

  JsonObject root;
  DynamicJsonDocument temp(2048);
  DynamicJsonDocument reponse(2048);
  reponse.createNestedObject(FPSTR(REP_007));
  root = temp.to<JsonObject>();   
  get_json_pl_items(pP, root, true);
  reponse[FPSTR(REP_007)] = temp;

  #ifdef FSOK
    // pl_fs(pP, reponse);  
  #endif
}
void Program::pl_item_remove(uint8_t pP, uint8_t aP) {

  pl_item_removeitemIdByArrayPos(pP, aP);

  uint16_t pC = 0;
  _Playlists[_plStatu.pos].get_items_cnt(pC);
  ListLoop::updatePos(&_pltFlag, pC-1);

  JsonObject root;
  DynamicJsonDocument temp(2048);
  DynamicJsonDocument reponse(2048);
  reponse.createNestedObject(FPSTR(REP_007));
  root = temp.to<JsonObject>();   
  get_json_pl_items(pP, root, true);
  reponse[FPSTR(REP_007)] = temp;

  #ifdef FSOK
    // pl_fs(pP, reponse);  
  #endif
}

void Program::pl_item_new(DynamicJsonDocument & doc, DynamicJsonDocument & reponse) {
  if (!doc.containsKey(F("pl_item_new"))) return;
  uint8_t pP          = doc[F("pl_item_new")][F("pP")].as<uint8_t>();
  uint8_t iP          = doc[F("pl_item_new")][F("iP")].as<uint8_t>();
  String  lbl         = doc[F("pl_item_new")][F("lbl")].as<String>();
  String  itemBase    = doc[F("pl_item_new")][F("itemBase")].as<String>();
  String  itemBaseCfg = doc[F("pl_item_new")][F("itemBaseCfg")].as<String>();  
  pl_item_toArray(pP, iP, lbl, itemBase, itemBaseCfg); 
  uint16_t pC = 0;
  _Playlists[_plStatu.pos].get_items_cnt(pC);
  ListLoop::updatePos(&_pltFlag, pC-1);
  JsonObject root;
  DynamicJsonDocument temp(2048);
  reponse.createNestedObject(FPSTR(REP_007));
  root = temp.to<JsonObject>();   
  get_json_pl_items(pP, root, true);
  reponse[FPSTR(REP_007)] = temp;

  #ifdef FSOK
    // pl_fs(pP, reponse); 
  #endif
}
void Program::pl_item_remove(DynamicJsonDocument & doc, DynamicJsonDocument & reponse) {
  if (!doc.containsKey(F("pl_item_remove"))) return;

  uint8_t pP = doc[F("pl_item_remove")][F("pP")].as<uint8_t>();
  uint8_t aP = doc[F("pl_item_remove")][F("iP")].as<uint8_t>();
  pl_item_removeitemIdByArrayPos(pP, aP);

  uint16_t pC = 0;
  _Playlists[_plStatu.pos].get_items_cnt(pC);
  ListLoop::updatePos(&_pltFlag, pC-1);

  JsonObject root;
  DynamicJsonDocument temp(2048);
  reponse.createNestedObject(FPSTR(REP_007));
  root = temp.to<JsonObject>();   
  get_json_pl_items(pP, root, true);
  reponse[FPSTR(REP_007)] = temp;

  #ifdef FSOK
    // pl_fs(pP, reponse);  
  #endif
}

void Program::pl_item_toArray(uint8_t pP, uint8_t iP, const String & lbl, const String & itemBase, const String & itemBaseCfg) {
  uint8_t pos = _Playlists[pP].item_toArray(iP,lbl,itemBase,itemBaseCfg);
  // pl_fs( pP,  pos);
}
boolean Program::pl_item_removeitemIdByArrayPos(uint8_t pP, uint8_t aP) {
  uint8_t iId;
  if (!_Playlists[pP].get_itemIdByArrayPos(aP, iId)) return false;
  _Playlists[pP].item_remove(iId);
  return true;
}

void Program::pl_print(uint8_t pP) {
  Serial.printf_P(PSTR("[Program::pl_print][%d/%d]\n"), pP, _plStatu.cnt);
  _Playlists[pP].print();
  _Playlists[pP].item_print();
}
void Program::pl_print() {
  Serial.printf_P(PSTR("[Program::pl_print][%d]\n"), _plStatu.cnt);
  for(uint8_t i=0; i<_plStatu.cnt; i++) {_Playlists[i].print(); _Playlists[i].item_print();}
}

void Program::get_json_allpl_items(JsonObject & doc, boolean pL, boolean lRef) {
  const char * sPLref;
  const char * sLref;
  uint8_t cnt = 0;
  JsonArray arr = doc.createNestedArray(F("items"));
  for(uint8_t i=0; i<_plStatu.cnt; i++) {
    if (lRef) {
      _Playlists[i].get_listRef(sPLref);
      get_name(sLref);
      if (sPLref != sLref) continue;
    }
    cnt++;
    JsonObject var = arr.createNestedObject();
    _Playlists[i].item_json(var, pL);
  }  
  doc[F("cmax")] = cnt;

}

#ifdef FSOK
  void Program::pl_fs(){
    if (!_fs_setup) return;
    if (!_fs_pl)    return;

    String path = (String)FPSTR(FOPATH_PLAYLIST) + (String)FPSTR(FNPREFIX_PLAYLIST) + String(_plStatu.pos) + (String)FPSTR(FNEXT_PLAYLIST) ;
    File f=FILESYSTEM.open(path,"w");
    if (!f) {
      #ifdef DEBUG
        Serial.printf_P(PSTR("[Program::pl_fs][Error open /w]\n\t%s\n"), path.c_str());  
      #endif
      return;
    }
    DynamicJsonDocument doc(5000);
    DynamicJsonDocument temp(2048);
    JsonObject          root = temp.to<JsonObject>();   

    doc.createNestedObject(FPSTR(REP_007));
    get_json_pl_items(_plStatu.pos, root, true);
    doc[FPSTR(REP_007)] = temp; 

    temp.clear();
    serializeJson(doc, f);     
    doc.clear();
    f.close();  
    delay(2); 
  }

  void Program::pl_fs(uint8_t pP){
    if (!_fs_setup) return;

    #ifdef DEBUG
      Serial.printf_P(PSTR("[Program::pl_fs][%d]\n"), pP);   
    #endif
    String path = (String)FPSTR(FOPATH_PLAYLIST) + (String)FPSTR(FNPREFIX_PLAYLIST) + String(pP) + (String)FPSTR(FNEXT_PLAYLIST) ;
    File f=FILESYSTEM.open(path,"w");
    if (!f) {
      #ifdef DEBUG
        Serial.printf_P(PSTR("[Program::pl_fs][Error open /w]\n\t%s\n"), path.c_str());  
      #endif
      return;
    }
    uint32_t tNow = millis();

    DynamicJsonDocument doc(3072);
    DynamicJsonDocument temp(2048);
    JsonObject          root = temp.to<JsonObject>();   

    doc.createNestedObject(FPSTR(REP_007));
    get_json_pl_items(pP, root, true);
    doc[FPSTR(REP_007)] = temp;  

    temp.clear();
    serializeJson(doc, f);
    doc.clear();
    f.close();  
    delay(2);

    uint32_t tDif = millis() - tNow;
    Serial.printf("[duration: %d]\n", tDif);
  }
  void Program::pl_fs(uint8_t pPos, DynamicJsonDocument & doc){
    if (!_fs_setup) return;
    if (!_fs_pl)    return;

    String path = (String)FPSTR(FOPATH_PLAYLIST) + (String)FPSTR(FNPREFIX_PLAYLIST) + String(pPos) + (String)FPSTR(FNEXT_PLAYLIST) ;
    File f=FILESYSTEM.open(path,"w");
    if (!f) {
      #ifdef DEBUG
        Serial.printf_P(PSTR("[Program::pl_fs][Error open /w]\n\t%s\n"), path.c_str());     
      #endif
      return;
    }
    #ifdef DEBUG
      uint32_t tNow = millis();  
    #endif    
    serializeJson(doc, f);
    f.close();  
    delay(1);  
    #ifdef DEBUG
      uint32_t tDif = millis() - tNow;
      Serial.printf("[duration: %d]\n", tDif);   
    #endif   
  }

  void Program::pl_fs_restore(uint8_t pPos){
    if (!_fs_setup) return;

    String path = (String)FPSTR(FOPATH_PLAYLIST) + (String)FPSTR(FNPREFIX_PLAYLIST) + String(pPos) + (String)FPSTR(FNEXT_PLAYLIST) ;
    DynamicJsonDocument doc(2048);
    char buffer[80];
    sprintf(buffer, "%s", path.c_str());    
    if (!AP_deserializeFile(doc, buffer)) {
      #ifdef DEBUG
        Serial.printf_P(PSTR("[Program::pl_fs_restore][Error open /r]\n\t%s\n"), path.c_str());     
      #endif
      return;       
    }  
    _Playlists[pPos].item_df();
    _Playlists[pPos].item_restore(doc);
  } 
  void Program::pl_fs_restore() {
    if (!_fs_setup) return;

    for(uint8_t i=0; i<_plStatu.cnt; i++){
      pl_fs_restore(i);
    }
  }     
#endif

void Program::get_json_pl_items(JsonObject & doc, boolean pI) {
  #ifdef DEBUG
    Serial.printf_P(PSTR("[Program::get_json_pl_items][%d][%d]\n"),pI,_plStatu.pos);   
  #endif
  _Playlists[_plStatu.pos].item_json(doc, pI);
}
void Program::get_json_pl_items(uint8_t p, JsonObject & doc, boolean pI) {
  #ifdef DEBUG
    Serial.printf_P(PSTR("[Program::get_json_pl_items][%d]\n"), p);  
  #endif
  _Playlists[p].item_json(doc, pI);
}