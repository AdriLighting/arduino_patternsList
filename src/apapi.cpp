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
#include <functional>
#define DEBUG
#ifndef DEBUG
  #ifdef DEBUG_APAPI
    #define DEBUG
  #endif
#endif
#ifdef DEBUG
  #define LOG(func, ...) APTRACEC(func, __VA_ARGS__)
#else
  #define LOG(func, ...) ;
#endif

AP_userApi      _AP_userApi;
AP_Api          _AP_Api;
AP_ApiItemList  _apapiItemList;

AP_ApiItem::AP_ApiItem(const char * const v1, _apapiItem_f v2){
  _id = v1;
  _generateJson = std::move(v2);
}
AP_ApiItem::~AP_ApiItem(){}
AP_ApiItemList::AP_ApiItemList(){}
AP_ApiItemList::~AP_ApiItemList(){}
void AP_ApiItemList::add (const char * const v1, _apapiItem_f v2){
  _list.add(new AP_ApiItem(v1, v2));
}
void AP_ApiItemList::parsingRequest(const String & v1, DynamicJsonDocument & doc){
  for(int i = 0; i < _list.size(); ++i) {
    if (v1 == ch_toString( _list[i]->_id)){
      // Serial.printf("[AP_ApiItemList::parsingRequest][%s]\n", v1.c_str());
      _list[i]->_generateJson(doc);
      LOG(APPT_DEBUGREGION_APAPI,"END [heap: %d]\n", ESP.getFreeHeap());
    }
  }
}

void apapiItemList_obj(const char * const v1, std::function<void(JsonObject & obj)> func, DynamicJsonDocument& obj, boolean isReply ) {
   if (isReply && obj[FPSTR(v1)]) return;
    JsonObject var_1 = obj.createNestedObject(FPSTR(v1));
    func(var_1);    

}
void apapiItemList_obj(const char * const v1, std::function<void(JsonObject & obj)> func, JsonObject& obj ) {
  JsonObject var_1 = obj.createNestedObject(FPSTR(v1));
  func(var_1);
}
void apapiItemList_obj(const char * const v1, const char * const v2, std::function<void(JsonObject & obj)> func, DynamicJsonDocument& obj ) {
    LOG(APPT_DEBUGREGION_APAPI,"");
    JsonObject var_1;
    JsonObject var_2;
    if (!obj[FPSTR(v1)]) {
      LOG(APPT_DEBUGREGION_APAPI,"");
      var_1 = obj.createNestedObject(FPSTR(v1));
      if (!obj[FPSTR(v1)][FPSTR(v2)]) {
         LOG(APPT_DEBUGREGION_APAPI,"");
        var_2 = var_1.createNestedObject(FPSTR(v2));
        func(var_2); 
      } else {
        LOG(APPT_DEBUGREGION_APAPI,"[heap: %d]\n", ESP.getFreeHeap());
        // StaticJsonDocument <2048> temp;
        DynamicJsonDocument temp(2048);
        var_2 = temp.to<JsonObject>();
        func(var_2);
        obj[FPSTR(v1)][FPSTR(v2)] = temp; 
        temp.clear();        
      }
    } else {
       LOG(APPT_DEBUGREGION_APAPI,"");
      if (!obj[FPSTR(v1)][FPSTR(v2)]) {
        LOG(APPT_DEBUGREGION_APAPI,"[heap: %d]\n", ESP.getFreeHeap());
        // StaticJsonDocument <2048> temp;
        DynamicJsonDocument temp(2048);
        var_2 = temp.to<JsonObject>();
        func(var_2);
        obj[FPSTR(v1)][FPSTR(v2)] = temp;
        temp.clear(); 
      }
    }  
}

void AP_ApiItemList_setup(){
  // list_alllb
  _apapiItemList.add(APPT_REQ_001, [](DynamicJsonDocument & reply){ // n: cmax: items:[n: cmax: items:[]]
    apapiItemList_obj(APPT_REQ_001, std::bind(&Program::get_json_lb_items, ProgramPtrGet(), std::placeholders::_1, true), reply, true); // A
  });
  // list_lb
  _apapiItemList.add(APPT_REQ_002, [](DynamicJsonDocument & reply){ // n: cmax: items:[]
    apapiItemList_obj(APPT_REQ_002, std::bind(&Program::jsonObject, ListbasePtrGet(), std::placeholders::_1), reply, true); // A
  }); 
  // list_allpl
  _apapiItemList.add(APPT_REQ_003, [](DynamicJsonDocument & reply){ // items:[ cmax: cnt: pos: lbl: lref: items:[id: lbl: ib: ibcfg:] ]
    apapiItemList_obj(APPT_REQ_003, std::bind(&Program::get_json_allpl_items, ProgramPtrGet(), std::placeholders::_1, true, false), reply, true); // A
  }); 
  // list_pl
  _apapiItemList.add(APPT_REQ_004, [](DynamicJsonDocument & reply){ // cmax: cnt: pos: lbl: lref: items:[id: lbl: ib: ib: ibcfg:]
    apapiItemList_obj(APPT_REQ_004, std::bind(&Program::get_json_pl_items, ProgramPtrGet(), std::placeholders::_1, true), reply, true); // A
  }); 
  // loop
  _apapiItemList.add(APPT_REQ_005, [](DynamicJsonDocument & reply){
    /*
      statu:{play: pause: rnd: delay: delaymin: rt: }, 
      pl:{set: play: pos: cnt: ib},
      plt:{pos: min: max:},
      lb:{name: pos: cnt: iname: icnt: icmax:},
      lbt:{pos: min: max:}
    */     
    apapiItemList_obj(APPT_REQ_005, std::bind(&Program::get_json_jsInit, ProgramPtrGet(), std::placeholders::_1), reply, true); // A
  }); 
  // list = lb - cmax: items:[n: cmax:] , pl - cmax: items:[ cmax: cnt: pos: lbl: lref: ]
  _apapiItemList.add(APPT_REQ_006, [](DynamicJsonDocument & reply){
    LOG(APPT_DEBUGREGION_APAPI,"");
    JsonObject var;
    if (!reply[FPSTR(APPT_REQ_006)]) {
      LOG(APPT_DEBUGREGION_APAPI,"");
      var = reply.createNestedObject(FPSTR(APPT_REQ_006));
      apapiItemList_obj(APPT_REP_003, std::bind(&Program::get_json_lb_items, ProgramPtrGet(), std::placeholders::_1, false), var);
      LOG(APPT_DEBUGREGION_APAPI,"");  
      apapiItemList_obj(APPT_REP_001, std::bind(&Program::get_json_allpl_items, ProgramPtrGet(), std::placeholders::_1, false, false), var);       
    } else {
      LOG(APPT_DEBUGREGION_APAPI,"");
      
      if (!reply[FPSTR(APPT_REQ_006)][FPSTR(APPT_REP_003)]) {
        LOG(APPT_DEBUGREGION_APAPI,"[heap: %d]\n", ESP.getFreeHeap());
        DynamicJsonDocument temp(2048);;
        var = temp.to<JsonObject>();        
        apapiItemList_obj(APPT_REP_003, std::bind(&Program::get_json_lb_items, ProgramPtrGet(), std::placeholders::_1, false), var); 
        reply[FPSTR(APPT_REQ_006)][FPSTR(APPT_REP_003)] = temp;
      }
      if (!reply[FPSTR(APPT_REQ_006)][FPSTR(APPT_REP_001)]) {
        LOG(APPT_DEBUGREGION_APAPI,"[heap: %d]\n", ESP.getFreeHeap());
        DynamicJsonDocument temp(2048);
        var = temp.to<JsonObject>();
        apapiItemList_obj(APPT_REP_001, std::bind(&Program::get_json_allpl_items, ProgramPtrGet(), std::placeholders::_1, false, false), var); 
        reply[FPSTR(APPT_REQ_006)][FPSTR(APPT_REP_001)] = temp;
      }       
    }

  }); 
  // list_lbs return  list
  _apapiItemList.add(APPT_REQ_007, [](DynamicJsonDocument & reply){ // lb - cmax: items:[n: cmax:]
    apapiItemList_obj(APPT_REQ_006, APPT_REP_003, std::bind(&Program::get_json_lb_items, ProgramPtrGet(), std::placeholders::_1, false), reply);
  }); 
  // list_pls return  list
  _apapiItemList.add(APPT_REQ_008, [](DynamicJsonDocument & reply){ // pl - items:[ cmax: cnt: pos: lbl: lref: ]
    apapiItemList_obj(APPT_REQ_006, APPT_REP_001, std::bind(&Program::get_json_allpl_items, ProgramPtrGet(), std::placeholders::_1, false, false), reply);
  }); 
  // list_ra
  _apapiItemList.add(APPT_REQ_009, [](DynamicJsonDocument & reply){
    uint8_t size = ARRAY_SIZE(APPT_SETTER_ARRAY); JsonArray arr = reply.createNestedArray(FPSTR(APPT_REQ_009));
    for (int j = 0; j < size; ++j) {arr.add(FPSTR(APPT_SETTER_ARRAY[j]));} 
  });  
  // list_req 
  _apapiItemList.add(APPT_REQ_010, [](DynamicJsonDocument & reply){
    uint8_t size = ARRAY_SIZE(APPT_REQ_ARRAY); JsonArray arr = reply.createNestedArray(FPSTR(APPT_REQ_010));
    for (int j = 0; j < size; ++j) {arr.add(FPSTR(APPT_REQ_ARRAY[j]));} 
  });  
/*  // list_lbpl 
  _apapiItemList.add(APPT_REQ_011, [](DynamicJsonDocument & reply){
    // list_lb - cmax: items:[]
    apapiItemList_obj(APPT_REQ_002, std::bind(&Program::jsonObject, ListbasePtrGet(), std::placeholders::_1), reply);
    // list_pl - cmax: cnt: pos: lbl: lref: items:[id: lbl: ib: ib: ibcfg:]
    apapiItemList_obj(APPT_REQ_004, std::bind(&Program::get_json_pl_items, ProgramPtrGet(), std::placeholders::_1, true), reply);
  }); */
  // js_init 
  _apapiItemList.add(APPT_REQ_012, [](DynamicJsonDocument & reply){
    if ((ProgramPtrGet()->_plStatu.isPlaying && ProgramPtrGet()->_plStatu.isSet)) { // list_pl - cmax: cnt: pos: lbl: lref: items:[id: lbl: ib: ib: ibcfg:]
      apapiItemList_obj(APPT_REQ_004, std::bind(&Program::get_json_pl_items, ProgramPtrGet(), std::placeholders::_1, true), reply, true);} // A
    else { // list_lb - cmax: items:[]
      apapiItemList_obj(APPT_REQ_002, std::bind(&Program::jsonObject, ListbasePtrGet(), std::placeholders::_1), reply, true);} // A     
  }); 
  // list_plsc return  list
  _apapiItemList.add(APPT_REQ_013, [](DynamicJsonDocument & reply){ // plc - items:[ cmax: cnt: pos: lbl: lref: ]
    apapiItemList_obj(APPT_REQ_006, APPT_REP_006, std::bind(&Program::get_json_allpl_items, ProgramPtrGet(), std::placeholders::_1, false, true), reply);
  });  
  // listc return  list
  _apapiItemList.add(APPT_REQ_014, [](DynamicJsonDocument & reply){
    JsonObject var = reply.createNestedObject(FPSTR(APPT_REQ_006));
    // lb - cmax: items:[]
    apapiItemList_obj(APPT_REP_003, std::bind(&Program::get_json_lb_items, ProgramPtrGet(), std::placeholders::_1, false), var);
    // plc - items:[ cmax: cnt: pos: lbl: lref: ] 
    apapiItemList_obj(APPT_REP_006, std::bind(&Program::get_json_allpl_items, ProgramPtrGet(), std::placeholders::_1, false, true), var);
  }); 

  // "get":[{"loop_select":["statu"]}]}
  // loop_select - statu
  _apapiItemList.add(APPT_REP_005, [](DynamicJsonDocument & reply){ // loop:{statu:{play: pause: rnd: delay: delaymin: rt:}}
    LOG(APPT_DEBUGREGION_APAPI,"[heap: %d] START\n", ESP.getFreeHeap());
    apapiItemList_obj(APPT_REQ_005, APPT_REP_005, std::bind(&Program::get_json_statu, ProgramPtrGet(), std::placeholders::_1), reply);
  });
  // loop_select - pl
  _apapiItemList.add(APPT_REP_001, [](DynamicJsonDocument & reply){ // loop:{pl:{set: play: pos: cnt: ib:}} 
    LOG(APPT_DEBUGREGION_APAPI,"[heap: %d] START\n", ESP.getFreeHeap());
    apapiItemList_obj(APPT_REQ_005, APPT_REP_001, std::bind(&Program::get_json_pl, ProgramPtrGet(), std::placeholders::_1), reply);
  });
  // loop_select - plt
  _apapiItemList.add(APPT_REP_002, [](DynamicJsonDocument & reply){ // loop:{plt:{pos: min: max:}} 
    LOG(APPT_DEBUGREGION_APAPI,"[heap: %d] START\n", ESP.getFreeHeap());
    apapiItemList_obj(APPT_REQ_005, APPT_REP_002, std::bind(&Program::get_json_plt, ProgramPtrGet(), std::placeholders::_1), reply);
  });
  // loop_select - lb
  _apapiItemList.add(APPT_REP_003, [](DynamicJsonDocument & reply){ // loop:{lb:{name: pos: cnt: iname: icnt: icmax: }}  
    LOG(APPT_DEBUGREGION_APAPI,"[heap: %d] START\n", ESP.getFreeHeap());
    apapiItemList_obj(APPT_REQ_005, APPT_REP_003, std::bind(&Program::get_json_lb, ProgramPtrGet(), std::placeholders::_1), reply);
  });
  // loop_select - lbt
  _apapiItemList.add(APPT_REP_004, [](DynamicJsonDocument & reply){ // loop:{lbt:{pos: min: max:}} 
    LOG(APPT_DEBUGREGION_APAPI,"[heap: %d] START\n", ESP.getFreeHeap());
    apapiItemList_obj(APPT_REQ_005, APPT_REP_004, std::bind(&Program::get_json_lbt, ProgramPtrGet(), std::placeholders::_1), reply);
  });

}

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
/**
 * @fn          void AP_userApi::set_json_id(JsonArray & doc)
 *
 * @brief       
 * @details     
 *
 * @author      Adrien Grellard
 * @date        15/03/2022
 *
 * @param[out]  doc       
 */
void AP_userApi::set_json_id(JsonArray & doc)  {
  String cReq;
  for (int i = 0; i < _arrayCntMax; ++i) {
    if (!_array[i].isEnabled()) continue;
    _array[i].get_id(cReq);
    doc.add(cReq);
  }
}


AP_Api::AP_Api() {}

AP_Api::~AP_Api() {}

/**
 * @fn          void AP_ApiReply::set_ra(apSetter_t action) 
 *
 * @author      Adrien Grellard
 * @date        15/03/2022
 *
 * @param[in]   action    
 */
void AP_ApiReply::set_ra(apSetter_t action) {
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
  // Serial.printf_P(PSTR("[AP_ApiReply::reply_generate]\n\t[apSetter_t: %d][value: %s]\n"), _action, _value);

  switch (_action) { 
    case apSetter_t::APSET_PLAY_LT:        
    case apSetter_t::APSET_ITEM:        
    case apSetter_t::APSET_ITEM_PREV:        
    case apSetter_t::APSET_ITEM_RND:             
    case apSetter_t::APSET_ITEM_NEXT:   
      // loop
      if (!doc.containsKey(FPSTR(APPT_REQ_005))){ 
        doc.createNestedObject(FPSTR(APPT_REQ_005));}

      if (pPtr->_plStatu.isPlaying && pPtr->_plStatu.isSet) {
        // loop-pl
        if (!doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_001)]) { 
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->get_json_pl(root);
          doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_001)] = temp; 
        }
        // loop-plt
        if (!doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_002)]) {
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->get_json_plt(root);
          doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_002)] = temp; 
        }
      } else {
        // loop-lb
        if (!doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_003)]) {
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->get_json_lb(root);
          doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_003)] = temp; 
        }
        // loop-lbt
        if (!doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_004)]) {
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->get_json_lbt(root);
          doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_004)] = temp; 
        }
      }
    break;
    case apSetter_t::APSET_PLAY_DELAY:       
    case apSetter_t::APSET_PLAY_DELAYMIN:    
    case apSetter_t::APSET_PLAY_DELAYMINON:  
    case apSetter_t::APSET_PLAY_DELAYMINOFF: 
    case apSetter_t::APSET_PLAY_RND:             
    case apSetter_t::APSET_PLAY_START:       
    case apSetter_t::APSET_PLAY_STOP:        
    case apSetter_t::APSET_PLAY_PAUSE:       
    case apSetter_t::APSET_PLAY_TOGGLE:      
      // loop
      if (!doc.containsKey(FPSTR(APPT_REQ_005))){
        doc.createNestedObject(FPSTR(APPT_REQ_005));}
      // loop-statu
      if (!doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_005)]) {
        temp.clear();
        root = temp.to<JsonObject>();
        pPtr->get_json_statu(root);
        doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_005)] = temp;  
      } 
    break;

    case apSetter_t::APSET_PLAY_PL: 
      // loop
      if (!doc.containsKey(FPSTR(APPT_REQ_005))){
        doc.createNestedObject(FPSTR(APPT_REQ_005));}

      if (pPtr->_plStatu.isPlaying && pPtr->_plStatu.isSet) {
        // loop-pl
        if (!doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_001)]) {
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->get_json_pl(root);
          doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_001)] = temp; 
        }
        // loop-plt
        if (!doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_002)]) {
          temp.clear();
          root = temp.to<JsonObject>();
          pPtr->get_json_plt(root);
          doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_002)] = temp; 
        }
      }    
    break;
    case apSetter_t::APSET_PLAY_LB:   
      // loop
      if (!doc.containsKey(FPSTR(APPT_REQ_005))){
        doc.createNestedObject(FPSTR(APPT_REQ_005));}
      // loop-lb
      if (!doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_003)]) {
        temp.clear();
        root = temp.to<JsonObject>();
        pPtr->get_json_lb(root);
        doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_003)] = temp; 
      }
      // loop-lbt
      if (!doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_004)]) {
        temp.clear();
        root = temp.to<JsonObject>();
        pPtr->get_json_lbt(root);
        doc[FPSTR(APPT_REQ_005)][FPSTR(APPT_REP_004)] = temp; 
      }       
    break;


    case apSetter_t::APSET_LSET_PL:
    break;

    case apSetter_t::APSET_PLI_NEW:
    case apSetter_t::APSET_PLI_REM:
    case apSetter_t::APSET_PLI_REP:
      // list_allpl
      if (!doc.containsKey(FPSTR(APPT_REQ_003))){
        doc.createNestedObject(FPSTR(APPT_REQ_003));
        temp.clear();
        root = temp.to<JsonObject>();   
        pPtr->get_json_allpl_items(root);
        doc[FPSTR(APPT_REQ_003)] = temp;
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
  DynamicJsonDocument reply(2048);
  parsingRequest(doc, reply, upd);
  serializeJson(reply, rep);
}
void AP_Api::parsingRequest(DynamicJsonDocument & doc, DynamicJsonDocument & reply, const String & upd) {
    JsonArray arr;
    uint8_t sizeReponse, posReponse, size;
    Program * pPtr = ProgramPtrGet();

    // need to determine size befor
       
    // DynamicJsonDocument       reply(2048);
    AP_ApiReply  * _webserverRequest_reply = nullptr;

    /*
      op
      0 = ?
      1 = playlist items management
    */
    if (doc.containsKey(FPSTR(APPT_004))) {
      uint8_t op = doc[FPSTR(APPT_004)].as<uint8_t>();
      if (op == 1) {
        pPtr->pl_item_new(doc, reply);
        pPtr->pl_item_remove(doc, reply);
      }
    }

    if (doc.containsKey(FPSTR(APPT_005))) {
      arr                       = doc[FPSTR(APPT_005)].as<JsonArray>();
      sizeReponse               = arr.size();
      _webserverRequest_reply = new AP_ApiReply[sizeReponse];
      posReponse                = 0;

      for (uint8_t i = 0; i < sizeReponse; i++) {
        JsonObject item = arr[i];
        String  n     = item[FPSTR(APPT_002)].as<String>();
        String  v     = item[FPSTR(APPT_003)].as<String>();
                size  = ARRAY_SIZE(APPT_SETTER_ARRAY);
        for (int j = 0; j < size; ++j) {
          if (n == "") continue;
          if ( n == FPSTR(APPT_SETTER_ARRAY[j]) ) {
            apSetter_t action = RAARR[j];
            _webserverRequest_reply[posReponse].set_ra(action);
            _webserverRequest_reply[posReponse].set_value(v);
            ProgramPtrGet()->remote_action(action, v.c_str(), upd.c_str());
            posReponse++;          
          }
        }
      }
    }

    // GETTER
    if (doc.containsKey(FPSTR(APPT_006))) {
      arr = doc[FPSTR(APPT_006)].as<JsonArray>();
      for (size_t i = 0; i < arr.size(); i++) {

        JsonObject  item  = arr[i];
        String      req   = arr[i].as<String>();

        if (!item && req == "") continue;

        _apapiItemList.parsingRequest(req, reply);
          
        if (item) {
          // Serial.printf("loop_select: %s\n", item[F("loop_select")].as<String>().c_str());

          // loop_select
          if (item[FPSTR(APPT_REQ_015)]) { //  "get":[{"loop_select":["statu"]}]}

            JsonArray arr_2 = item[FPSTR(APPT_REQ_015)].as<JsonArray>();
            if (arr_2.size() == 0) continue;

            if (!reply.containsKey(FPSTR(APPT_REQ_005))) reply.createNestedObject(FPSTR(APPT_REQ_005)); // loop
            for (size_t j = 0; j < arr_2.size(); j++) {
              String req_2 = arr_2[j].as<String>();  
              _apapiItemList.parsingRequest(req_2, reply);
            }
          }
          else if (item[FPSTR(APPT_001)]) { // gv "get":[{"gv":{"n":"","v":""}}]
            String  gGet = item[FPSTR(APPT_001)][FPSTR(APPT_002)].as<String>();
            if (gGet == FPSTR(APPT_REQ_016)) {
              if (!reply.containsKey(FPSTR(APPT_REP_007))){
                reply.createNestedObject(FPSTR(APPT_REP_007)); // pld
                DynamicJsonDocument temp(2048); 
                JsonObject root = temp.to<JsonObject>();   
                /*
                  cmax: cnt: pos: lbl: lref: items:[id: lbl: ib: ibcfg:]
                */
                pPtr->get_json_selectedpl_items(item[FPSTR(APPT_001)][FPSTR(APPT_003)].as<int>(), root, true);
                reply[FPSTR(APPT_REP_007)] = temp;
              }               
            }
          }
        } //>>> item

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

}

