/**
 * @defgroup   APAPI apapi
 *
 * @brief      This file implements apapi.
 *
 * @author     Adri
 * @date       2022
 */

#include "../include/apapi.h"
#include "../include/constants.h"
#include "../include/program.h"
#include "../include/tools.h"


AP_userApiItem::AP_userApiItem() {}

/**
 * @fn          void AP_userApiItem::set_mod(WSURM v1) 
 *
 * @author      Adrien Grellard
 * @date        15/03/2022
 *
 * @param[in]   v1        
 */
void AP_userApiItem::set_mod(WSURM v1) {
  _mod = v1;}
/**
 * @fn          void AP_userApiItem::set_id(const char * const & v1) 
 *
 * @author      Adrien Grellard
 * @date        15/03/2022
 *
 * @param[in]   v1        
 */
void AP_userApiItem::set_id(const char  * const & v1) {
  _id = v1;}
/**
 * @fn          void AP_userApiItem::set_replyCallback(_wsur_cb_f v1) 
 *
 * @author      Adrien Grellard
 * @date        15/03/2022
 *
 * @param[in]   v1        
 */
void AP_userApiItem::set_replyCallback(_wsur_cb_f v1) {
  _reply = std::move(v1);}
/**
 * @fn          void AP_userApiItem::get_id(String & v1) 
 *
 * @author      Adrien Grellard
 * @date        15/03/2022
 *
 * @param[out]  v1        
 */
void AP_userApiItem::get_id(String & v1) {
  v1 = ch_toString(_id);}
/**
 * @fn          void AP_userApiItem::get_mod(WSURM & v1) 
 *
 * @author      Adrien Grellard
 * @date        15/03/2022
 *
 * @param[out]  v1        
 */
void AP_userApiItem::get_mod(WSURM & v1) {
  v1 = _mod;}

/**
 * @fn          boolean AP_userApiItem::isEnabled() 
 *
 * @author      Adrien Grellard
 * @date        15/03/2022
 */
boolean AP_userApiItem::isEnabled() {
  return (_id == (const char *)"")?false:true;}
/**
 * @fn          boolean AP_userApiItem::cb(const String & v1, DynamicJsonDocument & doc) 
 *
 * @author      Adrien Grellard
 * @date        15/03/2022
 *
 * @param[in]   v1        
 * @param[out]  doc       
 */
boolean AP_userApiItem::cb(const String & v1, DynamicJsonDocument & doc)  {
  if (!_reply) return false;
  _reply(v1, doc);
  return true;
}


AP_userApi::AP_userApi(){}
/**
 * @fn          void AP_userApi::initialize(uint8_t cmax) 
 *
 * @brief       Initializes then aray int the setup().  
 * @details     
 *
 * @author      Adrien Grellard
 * @date        15/03/2022
 *
 * @param[in]   size  of AP_userApiItem object array     
 */
void AP_userApi::initialize(uint8_t cmax)  {
  _arrayCntMax = cmax;
  _array = new AP_userApiItem[cmax];
  for (int i = 0; i < cmax; ++i) {
    _array[i].set_mod(WSURM::WSURM_GETTER);
    _array[i].set_id("");
  }
}
/**
 * @fn          void AP_userApi::getter(const String & req, DynamicJsonDocument & doc) 
 *
 * @brief       
 * @details     
 *
 * @author      Adrien Grellard
 * @date        15/03/2022
 *
 * @param[in]   req       
 * @param[out]  doc       
 */
void AP_userApi::getter(const String & req, DynamicJsonDocument & doc)  {
  String cReq;
  for (int i = 0; i < _arrayCntMax; ++i) {
    if (!_array[i].isEnabled()) continue;
    _array[i].get_id(cReq);
    if (cReq != req) continue;
    if (!_array[i].cb(req, doc)) continue;
  }
}
/**
 * @fn          void AP_userApi::set_request(uint8_t p, const char * const & id, _wsur_cb_f f) 
 *
 * @brief       
 * @details     
 *
 * @author      Adrien Grellard
 * @date        15/03/2022
 *
 * @param[in]   p     position of the obejct array
 * @param[in]   id    id used when an API getter request is received 
 * @param[in]   f     The callback associate with identifier         
 */
void AP_userApi::set_request(uint8_t p, const char * const & id, _wsur_cb_f f)  {
  _array[p].set_id(id);
  _array[p].set_replyCallback(f);
  _array[p].set_mod(WSURM::WSURM_GETTER);
}
AP_userApi _AP_userApi;



AP_Api::AP_Api() {}

AP_Api::~AP_Api() {}

/**
 * @fn          void AP_ApiReply::set_ra(RA action) 
 *
 * @author      Adrien Grellard
 * @date        15/03/2022
 *
 * @param[in]   action    
 */
void AP_ApiReply::set_ra(RA action) {
  _action = action;}
/**
 * @fn          void AP_ApiReply::set_value(const String & v1) 
 *
 * @brief       
 * @details     
 *
 * @author      Adrien Grellard
 * @date        15/03/2022
 *
 * @param[in]   v1        
 */
void AP_ApiReply::set_value(const String & v1) {
  _value = v1;}
/**
 * @fn          void AP_ApiReply::reply_generate(DynamicJsonDocument & doc)
 *
 * @brief       
 * @details     
 *
 * @author      Adrien Grellard
 * @date        15/03/2022
 *
 * @param[out]  doc       
 */
void AP_ApiReply::reply_generate(DynamicJsonDocument & doc){
  JsonObject obj,root;
  Program * pPtr = ProgramPtrGet();
  DynamicJsonDocument temp(3000);
  // Serial.printf_P(PSTR("[AP_ApiReply::reply_generate]\n\t[RA: %d][value: %s]\n"), _action, _value);

  switch (_action) { 
    case RA::RA_PLAY_LT:        
    case RA::RA_ITEM:        
    case RA::RA_ITEM_PREV:        
    case RA::RA_ITEM_RND:             
    case RA::RA_ITEM_NEXT:   
      // loop
      if (!doc.containsKey(FPSTR(REQ_005))){ 
        doc.createNestedObject(FPSTR(REQ_005));}

      if (pPtr->_plStatu.isPlaying && pPtr->_plStatu.isSet) {
        // loop-pl
        if (!doc[FPSTR(REQ_005)][FPSTR(REP_001)]) { 
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->get_json_pl(root);
          doc[FPSTR(REQ_005)][FPSTR(REP_001)] = temp; 
        }
        // loop-plt
        if (!doc[FPSTR(REQ_005)][FPSTR(REP_002)]) {
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->get_json_plt(root);
          doc[FPSTR(REQ_005)][FPSTR(REP_002)] = temp; 
        }
      } else {
        // loop-lb
        if (!doc[FPSTR(REQ_005)][FPSTR(REP_003)]) {
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->get_json_lb(root);
          doc[FPSTR(REQ_005)][FPSTR(REP_003)] = temp; 
        }
        // loop-lbt
        if (!doc[FPSTR(REQ_005)][FPSTR(REP_004)]) {
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->get_json_lbt(root);
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
      // loop
      if (!doc.containsKey(FPSTR(REQ_005))){
        doc.createNestedObject(FPSTR(REQ_005));}
      // loop-statu
      if (!doc[FPSTR(REQ_005)][FPSTR(REP_005)]) {
        temp.clear();
        root = temp.to<JsonObject>();
        pPtr->get_json_statu(root);
        doc[FPSTR(REQ_005)][FPSTR(REP_005)] = temp;  
      } 
    break;

    case RA::RA_PLAY_PL: 
      // loop
      if (!doc.containsKey(FPSTR(REQ_005))){
        doc.createNestedObject(FPSTR(REQ_005));}

      if (pPtr->_plStatu.isPlaying && pPtr->_plStatu.isSet) {
        // loop-pl
        if (!doc[FPSTR(REQ_005)][FPSTR(REP_001)]) {
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->get_json_pl(root);
          doc[FPSTR(REQ_005)][FPSTR(REP_001)] = temp; 
        }
        // loop-plt
        if (!doc[FPSTR(REQ_005)][FPSTR(REP_002)]) {
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->get_json_plt(root);
          doc[FPSTR(REQ_005)][FPSTR(REP_002)] = temp; 
        }
      }    
    break;
    case RA::RA_PLAY_LB:   
      // loop
      if (!doc.containsKey(FPSTR(REQ_005))){
        doc.createNestedObject(FPSTR(REQ_005));}
      // loop-lb
      if (!doc[FPSTR(REQ_005)][FPSTR(REP_003)]) {
        temp.clear();
        root = temp.to<JsonObject>();
        pPtr->get_json_lb(root);
        doc[FPSTR(REQ_005)][FPSTR(REP_003)] = temp; 
      }
      // loop-lbt
      if (!doc[FPSTR(REQ_005)][FPSTR(REP_004)]) {
        temp.clear();
        root = temp.to<JsonObject>();
        pPtr->get_json_lbt(root);
        doc[FPSTR(REQ_005)][FPSTR(REP_004)] = temp; 
      }       
    break;


    case RA::RA_LSET_PL:
    break;


    case RA::RA_LGET_PL:
      // pld
      if (!doc.containsKey(FPSTR(REP_007))){
        doc.createNestedObject(FPSTR(REP_007)); // 
        temp.clear();
        root = temp.to<JsonObject>();   
        pPtr->get_json_pl_items(_value.toInt(), root, true);
        doc[FPSTR(REP_007)] = temp;
      } 
    break;

    case RA::RA_PLI_NEW:
    case RA::RA_PLI_REM:
    case RA::RA_PLI_REP:
      // list_allpl
      if (!doc.containsKey(FPSTR(REQ_003))){
        doc.createNestedObject(FPSTR(REQ_003));
        temp.clear();
        root = temp.to<JsonObject>();   
        pPtr->get_json_allpl_items(root);
        doc[FPSTR(REQ_003)] = temp;
      } 
    break;
    
    default:break;
  }
}
/**
 * @fn          void AP_Api::parsingRequest(DynamicJsonDocument & doc, String & rep, const String & upd) 
 *
 * @brief       
 * @details     
 *
 * @author      Adrien Grellard
 * @date        15/03/2022
 *
 * @param[out]  doc       
 * @param[out]  rep       
 * @param[in]   upd       
 */
void AP_Api::parsingRequest(DynamicJsonDocument & doc, String & rep, const String & upd) {
    JsonObject var, pl, root;
    JsonArray arr;
    uint8_t sizeReponse, posReponse, size;
    Program * pPtr = ProgramPtrGet();

    // need to determine size befor
    DynamicJsonDocument       temp(2048);    
    DynamicJsonDocument       reply(2048);
    AP_ApiReply  * _webserverRequest_reply = nullptr;

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
        // Serial.printf("op: %s\n", doc[F("op")].as<String>().c_str());
        pPtr->pl_item_new(doc, reply);
        pPtr->pl_item_remove(doc, reply);
      }
    }

    // type    
    // type de format (nom de commande avc String,int ETC...)
    // if (doc.containsKey(F("type"))) Serial.printf("type: %s\n",   doc[F("type")].as<String>().c_str());   


    if (doc.containsKey(F("set"))) {
      arr                       = doc[F("set")].as<JsonArray>();
      sizeReponse               = arr.size();
      _webserverRequest_reply = new AP_ApiReply[sizeReponse];
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
            _webserverRequest_reply[posReponse].set_ra(action);
            _webserverRequest_reply[posReponse].set_value(v);
            ProgramPtrGet()->remote_action(action, v.c_str(), upd.c_str());
            posReponse++;          
          }
          
        }
      }
    }

    // GETTER
    if (doc.containsKey(F("get"))) {
      arr = doc[F("get")].as<JsonArray>();
      for (size_t i = 0; i < arr.size(); i++) {
        JsonObject  item  = arr[i];
        String      req   = arr[i].as<String>();
        if (!item && req == "") continue;

        // list_alllb
        if (req == FPSTR(REQ_001)) {
          var = reply.createNestedObject(FPSTR(REQ_001));
          pPtr->get_json_lb_items(var);}
        // list_lb
        else if (req == FPSTR(REQ_002)) {
          var = reply.createNestedObject(FPSTR(REQ_002));
          pPtr->jsonObject(var);} 
        // list_allpl
        else if (req == FPSTR(REQ_003)) {
          var = reply.createNestedObject(FPSTR(REQ_003));
          pPtr->get_json_allpl_items(var);} 
        // list_pl
        else if (req == FPSTR(REQ_004)) {
          var = reply.createNestedObject(FPSTR(REQ_004));
          pPtr->get_json_pl_items(var);} 
        // loop
        else if (req == FPSTR(REQ_005)) {
          var = reply.createNestedObject(FPSTR(REQ_005));
          pPtr->get_json_jsInit(var);} 
        // list
        else if (req == FPSTR(REQ_006)) {
          var = reply.createNestedObject(FPSTR(REQ_006));
          pl = var.createNestedObject(FPSTR(REP_003)); // lb
          pPtr->get_json_lb_items(pl,false);
          pl = var.createNestedObject(FPSTR(REP_001)); // pl
          pPtr->get_json_allpl_items(pl,false);} 
        // list_lbs
        else if (req == FPSTR(REQ_007)) {
          var = reply.createNestedObject(FPSTR(REQ_006));
          pl = var.createNestedObject(FPSTR(REP_003)); // lb;
          pPtr->get_json_lb_items(pl,false);} 
        // list_pls
        else if (req == FPSTR(REQ_008)) {
          var = reply.createNestedObject(FPSTR(REQ_006));
          pl = var.createNestedObject(FPSTR(REP_001)); // pl
          pPtr->get_json_allpl_items(pl,false);} 
        // list_ra
        else if (req == FPSTR(REQ_009)) {
          size = ARRAY_SIZE(RAALLNAMES);
          JsonArray arr2 = reply.createNestedArray(FPSTR(REQ_009));
          for (int j = 0; j < size; ++j) {arr2.add(FPSTR(RAALLNAMES[j]));}}  
        // list_req         
        else if (req == FPSTR(REQ_010)) {
          size = ARRAY_SIZE(REQALL);
          JsonArray arr2 = reply.createNestedArray(FPSTR(REQ_010));
          for (int j = 0; j < size; ++j) {arr2.add(FPSTR(REQALL[j]));}} 
        // list_lbpl
        else if (req == FPSTR(REQ_011)) {
          var = reply.createNestedObject(FPSTR(REQ_002)); // list_lb
          pPtr->jsonObject(var);
          var = reply.createNestedObject(FPSTR(REQ_004)); // list_pl
          pPtr->get_json_pl_items(var);}
        // js_init
        else if (req == FPSTR(REQ_012)) {
          if ((pPtr->_plStatu.isPlaying && pPtr->_plStatu.isSet)) {
            var = reply.createNestedObject(FPSTR(REQ_004)); // list_pl
            pPtr->get_json_pl_items(var);}
          else {
            var = reply.createNestedObject(FPSTR(REQ_002)); // list_lb
            pPtr->jsonObject(var);}

          // var = reply.createNestedObject(FPSTR(REQ_005)); // loop
          // pPtr->get_json_jsInit(var);
          // var = reply.createNestedObject(FPSTR(REQ_006)); // list
          // pl = var.createNestedObject(FPSTR(REP_003)); // lb
          // pPtr->get_json_lb_items(pl,false);
          // pl = var.createNestedObject(FPSTR(REP_006)); // plc
          // pPtr->get_json_allpl_items(pl,false,true);
          }
        // list_plsc
        else if (req == FPSTR(REQ_013)) {
          var = reply.createNestedObject(FPSTR(REQ_006)); // list
          pl = var.createNestedObject(FPSTR(REP_006)); // plc
          pPtr->get_json_allpl_items(pl,false, true);}      
        // listc
        else if (req == FPSTR(REQ_014)) {
          var = reply.createNestedObject(FPSTR(REQ_006)); // list
          pl = var.createNestedObject(FPSTR(REP_003)); // lb
          pPtr->get_json_lb_items(pl,false);
          pl = var.createNestedObject(FPSTR(REP_006)); // plc
          pPtr->get_json_allpl_items(pl,false, true);}

        if (item) {
          // Serial.printf("loop_select: %s\n", item[F("loop_select")].as<String>().c_str());
          if (item[F("loop_select")]) {
            JsonArray arr_2 = item[F("loop_select")].as<JsonArray>();
            if (arr_2.size() == 0) continue;
            if (!reply.containsKey(FPSTR(REQ_005))) reply.createNestedObject(FPSTR(REQ_005)); // loop
            for (size_t j = 0; j < arr_2.size(); j++) {
              String req_2 = arr_2[j].as<String>();  
              // Serial.printf("%d : %s\n", j, req_2.c_str());
              if (!reply[FPSTR(REQ_005)][FPSTR(REP_005)]){
                if (req_2 == FPSTR(REP_005)) {
                  temp.clear();
                  root = temp.to<JsonObject>();                    
                  pPtr->get_json_statu(root);
                  reply[FPSTR(REQ_005)][FPSTR(REP_005)] = temp; 
                }
              }
              if (!reply[FPSTR(REQ_005)][FPSTR(REP_001)]){
                if (req_2 == FPSTR(REP_001)) {
                  temp.clear();
                  root = temp.to<JsonObject>();
                  pPtr->get_json_pl(root);
                  reply[FPSTR(REQ_005)][FPSTR(REP_001)] = temp; 
                }
              }
              if (!reply[FPSTR(REQ_005)][FPSTR(REP_002)]){
                if (req_2 == FPSTR(REP_002)) {
                  temp.clear();
                  root = temp.to<JsonObject>();
                  pPtr->get_json_plt(root);
                  reply[FPSTR(REQ_005)][FPSTR(REP_002)] = temp; 
                }
              }
              if (!reply[FPSTR(REQ_005)][FPSTR(REP_003)]){
                if (req_2 == FPSTR(REP_003)) {
                  temp.clear();
                  root = temp.to<JsonObject>();
                  pPtr->get_json_lb(root);
                  reply[FPSTR(REQ_005)][FPSTR(REP_003)] = temp; 
                }
              }
              if (!reply[FPSTR(REQ_005)][FPSTR(REP_004)]){
                if (req_2 == FPSTR(REP_004)) {
                  temp.clear();
                  root = temp.to<JsonObject>();                  
                  pPtr->get_json_lbt(root);
                  reply[FPSTR(REQ_005)][FPSTR(REP_004)] = temp; 
                }
              }

            }
          }
        } //>>> array in object value loop_select from getter item array | "get":["list",{"loop_select":["statu"]}]}

        _AP_userApi.getter(req, reply); // user request
      } //>>> GETTER for
    }
    if (_webserverRequest_reply) {
      for (int j = 0; j < posReponse; ++j) {
        _webserverRequest_reply[j].reply_generate(reply);
      }
      delete[] _webserverRequest_reply;
      yield(); 
    }
    serializeJson(reply, rep);
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



AP_Api _AP_Api;
