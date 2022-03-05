#include "webserverRequest.h"
#include "constants.h"
#include "program.h"


WebserverRequest::WebserverRequest() {}

WebserverRequest::~WebserverRequest() {}

void webserverRequest_reponse::set_ra(RA action)            {_action = action;}
void webserverRequest_reponse::set_value(const String & v1) {_value = v1;}
void webserverRequest_reponse::make_reponse(DynamicJsonDocument & doc){
  JsonObject obj,root;
  Program * pPtr = ProgramPtrGet();
  DynamicJsonDocument temp(2048);
  // Serial.printf_P(PSTR("[webserverRequest_reponse::make_reponse]\n\t[RA: %d][value: %s]\n"), _action, _value);

  switch (_action) { 
    case RA::RA_PLAY_LT:        
    case RA::RA_ITEM:        
    case RA::RA_ITEM_PREV:        
    case RA::RA_ITEM_RND:             
    case RA::RA_ITEM_NEXT:   
      if (!doc.containsKey(FPSTR(REQ_005))){
        doc.createNestedObject(FPSTR(REQ_005));}

      if (pPtr->_plStatu.isPlaying && pPtr->_plStatu.isSet) {
        if (!doc[FPSTR(REQ_005)][FPSTR(REP_001)]) {
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->set_json_pl(root);
          doc[FPSTR(REQ_005)][FPSTR(REP_001)] = temp; 
        }
        if (!doc[FPSTR(REQ_005)][FPSTR(REP_002)]) {
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->set_json_plt(root);
          doc[FPSTR(REQ_005)][FPSTR(REP_002)] = temp; 
        }
      } else {
        if (!doc[FPSTR(REQ_005)][FPSTR(REP_003)]) {
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->set_json_lb(root);
          doc[FPSTR(REQ_005)][FPSTR(REP_003)] = temp; 
        }
        if (!doc[FPSTR(REQ_005)][FPSTR(REP_004)]) {
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->set_json_lbt(root);
          doc[FPSTR(REQ_005)][FPSTR(REP_004)] = temp; 
        }
      }
    break;
    case RA::RA_PLAY_DELAY:       
    case RA::RA_PLAY_DELAYMIN:    
    case RA::RA_PLAY_DELAYMINON:  
    case RA::RA_PLAY_DELAYMINOFF: 
    case RA::RA_PLAY_RND:             
    case RA::RA_PLAY_START:       
    case RA::RA_PLAY_STOP:        
    case RA::RA_PLAY_PAUSE:       
    case RA::RA_PLAY_TOGGLE:      
      if (!doc.containsKey(FPSTR(REQ_005))){
        doc.createNestedObject(FPSTR(REQ_005));}

      if (!doc[FPSTR(REQ_005)][FPSTR(REP_005)]) {
        temp.clear();
        root = temp.to<JsonObject>();
        pPtr->set_json_statu(root);
        doc[FPSTR(REQ_005)][FPSTR(REP_005)] = temp;  
      } 
    break;

    case RA::RA_PLAY_PL: 
      if (!doc.containsKey(FPSTR(REQ_005))){
        doc.createNestedObject(FPSTR(REQ_005));}

      if (pPtr->_plStatu.isPlaying && pPtr->_plStatu.isSet) {
        if (!doc[FPSTR(REQ_005)][FPSTR(REP_001)]) {
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->set_json_pl(root);
          doc[FPSTR(REQ_005)][FPSTR(REP_001)] = temp; 
        }
        if (!doc[FPSTR(REQ_005)][FPSTR(REP_002)]) {
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->set_json_plt(root);
          doc[FPSTR(REQ_005)][FPSTR(REP_002)] = temp; 
        }
      }    
    break;
    case RA::RA_PLAY_LB:   
      if (!doc.containsKey(FPSTR(REQ_005))){
        doc.createNestedObject(FPSTR(REQ_005));}

      if (!doc[FPSTR(REQ_005)][FPSTR(REP_003)]) {
        temp.clear();
        root = temp.to<JsonObject>();
        pPtr->set_json_lb(root);
        doc[FPSTR(REQ_005)][FPSTR(REP_003)] = temp; 
      }
      if (!doc[FPSTR(REQ_005)][FPSTR(REP_004)]) {
        temp.clear();
        root = temp.to<JsonObject>();
        pPtr->set_json_lbt(root);
        doc[FPSTR(REQ_005)][FPSTR(REP_004)] = temp; 
      }       
    break;


    case RA::RA_LSET_PL:
    break;


    case RA::RA_LGET_PL:
      if (!doc.containsKey(FPSTR(REP_007))){
        doc.createNestedObject(FPSTR(REP_007));
        temp.clear();
        root = temp.to<JsonObject>();   
        pPtr->pl_currentJson(_value.toInt(), root, true);
        doc[FPSTR(REP_007)] = temp;
      } 
    break;

    case RA::RA_PLI_NEW:
    case RA::RA_PLI_REM:
    case RA::RA_PLI_REP:
      if (!doc.containsKey(FPSTR(REQ_003))){
        doc.createNestedObject(FPSTR(REQ_003));
        temp.clear();
        root = temp.to<JsonObject>();   
        pPtr->pl_json(root);
        doc[FPSTR(REQ_003)] = temp;
      } 
    break;
    
    default:break;
  }
}
void WebserverRequest::parsingRequest(DynamicJsonDocument & doc, String & rep, const String & upd) {
    JsonObject var, pl;
    JsonArray arr;
    uint8_t sizeReponse, posReponse, size;
    Program * pPtr = ProgramPtrGet();

    // need to determine size befor
    DynamicJsonDocument      reponse(5000);
    webserverRequest_reponse * _webserverRequest_reponse = nullptr;

    // cli
    // porvenance: http server, websocket server, udp/multi
    // if (doc.containsKey(F("cli")))   Serial.printf("cli: %s\n",   doc[F("cli")].as<String>().c_str()); 

    // op
    // fonction avec laquelle traiter le parsing + execution des commandes
    /*
      0 = ?
      1 = playlist items management
    */
    if (doc.containsKey(F("op"))) {
      uint8_t op = doc[F("op")].as<uint8_t>();
      if (op == 1) {
        Serial.printf("op: %s\n", doc[F("op")].as<String>().c_str());
        pPtr->pl_item_new(doc, reponse);
        pPtr->pl_item_remove(doc, reponse);
      }
    }

    // type    
    // type de format (nom de commande avc String,int ETC...)
    // if (doc.containsKey(F("type"))) Serial.printf("type: %s\n",   doc[F("type")].as<String>().c_str());   



    if (doc.containsKey(F("set"))) {
      arr                       = doc[F("set")].as<JsonArray>();
      sizeReponse               = arr.size();
      _webserverRequest_reponse = new webserverRequest_reponse[sizeReponse];
      posReponse                = 0;

      for (uint8_t i = 0; i < sizeReponse; i++) {
        JsonObject item = arr[i];
        String  n     = item[F("n")].as<String>();
        String  v     = item[F("v")].as<String>();
                size  = ARRAY_SIZE(RAALLNAMES);
        for (int j = 0; j < size; ++j) {
          if (n == "") continue;
          if ( n == FPSTR(RAALLNAMES[j]) ) {
            RA action = RAARR[j];
            _webserverRequest_reponse[posReponse].set_ra(action);
            _webserverRequest_reponse[posReponse].set_value(v);
            ProgramPtrGet()->remote_action(action, v.c_str(), upd.c_str(), NULL);
            posReponse++;          
          }
          
        }
      }
    }


    // GETTER
    if (doc.containsKey(F("get"))) {
      arr = doc[F("get")].as<JsonArray>();
      for (size_t i = 0; i < arr.size(); i++) {
        String req = arr[i].as<String>();
        if (req == "") continue;

        // list_alllb
      	if (req == FPSTR(REQ_001)) {var = reponse.createNestedObject(FPSTR(REQ_001));pPtr->lb_json(var);}
        // list_lb
        if (req == FPSTR(REQ_002)) {var = reponse.createNestedObject(FPSTR(REQ_002));pPtr->jsonObject(var);} 
        // list_allpl
        if (req == FPSTR(REQ_003)) {var = reponse.createNestedObject(FPSTR(REQ_003));pPtr->pl_json(var);} 
        // list_pl
        if (req == FPSTR(REQ_004)) {var = reponse.createNestedObject(FPSTR(REQ_004));pPtr->pl_currentJson(var);} 
        // loop
        if (req == FPSTR(REQ_005)) {var = reponse.createNestedObject(FPSTR(REQ_005));pPtr->set_json_v1(var);} 
        // list
        if (req == FPSTR(REQ_006)) {
          var = reponse.createNestedObject(FPSTR(REQ_006));
          pl = var.createNestedObject(FPSTR(REP_003));
          pPtr->lb_json(pl,false);
          pl = var.createNestedObject(FPSTR(REP_001));
          pPtr->pl_json(pl,false);
        } 
        // list_lbs
        if (req == FPSTR(REQ_007)) {var = reponse.createNestedObject(FPSTR(REQ_006));pl = var.createNestedObject(FPSTR(REP_003));pPtr->lb_json(pl,false);} 
        // list_pls
        if (req == FPSTR(REQ_008)) {var = reponse.createNestedObject(FPSTR(REQ_006));pl = var.createNestedObject(FPSTR(REP_001));pPtr->pl_json(pl,false);
        } 
        // list_ra
        if (req == FPSTR(REQ_009)) {
          size = ARRAY_SIZE(RAALLNAMES);
          JsonArray arr2 = reponse.createNestedArray(FPSTR(REQ_009));
          for (int j = 0; j < size; ++j) {arr2.add(FPSTR(RAALLNAMES[j]));}          
        }  
        // list_req         
        if (req == FPSTR(REQ_010)) {
          size = ARRAY_SIZE(REQALL);
          JsonArray arr2 = reponse.createNestedArray(FPSTR(REQ_010));
          for (int j = 0; j < size; ++j) {arr2.add(FPSTR(REQALL[j]));}          
        } 
        // list_lbpl
        if (req == FPSTR(REQ_011)) {
          var = reponse.createNestedObject(FPSTR(REQ_002));pPtr->jsonObject(var);
          var = reponse.createNestedObject(FPSTR(REQ_004));pPtr->pl_currentJson(var);
        }
        // js_init
        if (req == FPSTR(REQ_012)) {
          
          if ((pPtr->_plStatu.isPlaying && pPtr->_plStatu.isSet))
            {var = reponse.createNestedObject(FPSTR(REQ_004));pPtr->pl_currentJson(var);}
          else
            {var = reponse.createNestedObject(FPSTR(REQ_002));pPtr->jsonObject(var);}

          var = reponse.createNestedObject(FPSTR(REQ_005));pPtr->set_json_v1(var);
          var = reponse.createNestedObject(FPSTR(REQ_006));
            pl = var.createNestedObject(FPSTR(REP_003));
            pPtr->lb_json(pl,false);
            pl = var.createNestedObject(FPSTR(REP_006));
            ProgramPtrGet()->pl_json(pl,false,true);          
        }
        // list_plsc
        if (req == FPSTR(REQ_013)) {var = reponse.createNestedObject(FPSTR(REQ_006));pl = var.createNestedObject(FPSTR(REP_006));pPtr->pl_json(pl,false, true);}
        
        // listc
        if (req == FPSTR(REQ_014)) {
          var = reponse.createNestedObject(FPSTR(REQ_006));
          pl = var.createNestedObject(FPSTR(REP_003));
          pPtr->lb_json(pl,false);
          pl = var.createNestedObject(FPSTR(REP_006));
          pPtr->pl_json(pl,false, true);
        } 

      }
    }
    if (_webserverRequest_reponse!=nullptr) {
      for (int j = 0; j < posReponse; ++j) {
        _webserverRequest_reponse[j].make_reponse(reponse);
      }
      delete[] _webserverRequest_reponse;
      yield(); 
    }
    
    serializeJson(reponse, rep);

}

/*
  if (cmd == "eff_rnd") {
  if (cmd == "eff_next") {
  if (cmd == "eff_prev") {
  if (cmd == "autoplay")      {_programme->autoPlay(); _programme->aytoDelayReset();}
  if (cmd == "autoplay_rnd")    _programme->autoPlayRnd();
  if (cmd == "autoplay_delay")  _programme->delayCurrent_set(value.toInt());


  static const char PROGMEM KEYEFFLIST_playlistPos            [] = "playlistPos";
  static const char PROGMEM KEYEFFLIST_playlistItemPos        [] = "playlistItemPos";
  static const char PROGMEM KEYEFFLIST_playlist               [] = "playlist";
  static const char PROGMEM KEYEFFLIST_patternPos             [] = "patternPos";
  static const char PROGMEM KEYEFFLIST_patternDelayCurrent    [] = "patternDelayCurrent";
  static const char PROGMEM KEYEFFLIST_patternIsRndPlaying    [] = "patternIsRndPlaying";
  static const char PROGMEM KEYEFFLIST_patternIsPlaying       [] = "patternIsPlaying";
  static const char PROGMEM KEYEFFLIST_patternDelayInMin      [] = "patternDelayInMin";

  if (cmd == "toggle"){
  if (cmd == "compo_prev") {
  if (cmd == "compo_next") {
  if (cmd == "compo") {
  if (cmd == "compo_add") {
  if (cmd == "compo_del") {
*/



WebserverRequest _WebserverRequest;