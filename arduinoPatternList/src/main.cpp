/* AFAIRE

  construire un heritage des classe -> FAIT 
    pattern
      loop
      playlist
        programe
      
--
  control utilisateur -> A FINIR SUIVANT BESION UTILISATEUR
--  
  sauveardes/chargement FS -> AFAIRE
  APIREST/export json -> A FINIR SUIVANT BESION UTILISATEUR
*/

/*

FONCTIONNEMENT ------------------------------------------------------------------------------

  LISTE DE BASE -----------------------------------------------------------------------------
  
    initialisation du nombre de liste   Program (uint8_t nbLb );
      size
    EX:
      _Program = new Program(2);

    initialisation des listes           initialize_lb(uint8_t p, const char * name, uint8_t items, const char * const * arr);
      position de la liste  
      id
      size: size of items array
      items: static const char* const items[] PROGMEM
    EX: 
      _Program->initialize_lb(0, "full",  ARRAY_SIZE(LPALLNAMES)          , LPALLNAMES);
      _Program->initialize_lb(1, "cat",   ARRAY_SIZE(LPALLNAMES_CAT)      , LPALLNAMES_CAT);

    chargment d'une des liste de base    
    EX:
      _Program->initialize(ARRAY_SIZE(LPALLNAMES), LPALLNAMES, "full", SORT_TYPE::ST_AB);

  PLAYLIST ----------------------------------------------------------------------------------

    les items des playlist correspondes aux items de la liste de base ratacher a cette playliste

    initialisation des playlist               initialize_playlist(uint8_t , const uint8_t * const &, const char ** const &);
      nb de playlist
      nb d'items par playlist
      liste de basse ratacher a la playliste
    EX:
      uint8_t plC   = 5;                                              // x5 playliste
      uint8_t iC[]  = {10, 5, 4, 3, 2};                               // nb items max par playlist
      const char * Ln[] = {"full", "cat", "null", "null", "null"};   // liste de bae attribuer pour chaque playliste
      _Program->initialize_playlist(plC, iC, Ln);

  CONTROL UTILISATEUR -----------------------------------------------------------------------
    EX:
                              command             arg1  arg2
      _Program->remote_action(RA::RA_LSET_PL,     "0",  NULL);  // chargment de la playlist
      _Program->remote_action(RA::RA_PLAY_LB,     NULL);        // selection des item via la liste de base
      _Program->remote_action(RA::RA_PLAY_PL,     NULL);        // selection des item via la playlist
      _Program->remote_action(RA::RA_ITEM_NEXT,   NULL);        // selection de l'item voullu
      _Program->remote_action(RA::RA_ITEM_PREV,   NULL);        // **
      _Program->remote_action(RA::RA_ITEM_RND,    NULL);        // **
      _Program->remote_action(RA::RA_ITEM,        "0");         // **

      _Program->remote_action(RA::RA_PLAY_DELAY,  "10", NULL);  // delay de rotation des items
      _Program->remote_action(RA::RA_PLAY_STOP,   NULL);        // play/stop/pause rotation des items

API REST ------------------------------------------------------------------------------------
  
  REQUETTE ----------------------------------------------------------------------------------
    HTTP, JS, UDP, SOCKET 
      JSON SYNTAX
        op    object  fonction avec laquelle traiter le parsing + execution des commandes
        type  object  type de format (nom de commande avc String,int ETC...)
        cli   object  porvenance: http server, websocket server, udp/multi
        set   array   setter
              [
                n object  id commande
                v object  valeur commoand si besoin
              ] , 
              [**]
        get   array   getter
              [  id commande ] , 
              [**]  

  CLIENT->SERVER ----------------------------------------------------------------------------
    HTTP - POSTMAN https://web.postman.co/ --------------------------------------------------
      POST /api HTTP/1.1
      Host: 192.168.0.157
      Content-Type: application/json
      Content-Length: 59
      {"op":0,"type":"HTTP_POST", "set":[{"n":"","v":"5"}],"get":["loop","",""]}  
      {"op":"","type":"HTTP_POST","set":[{"n":"","v":"5"}],"get":["loop",""]}
    CURL ------------------------------------------------------------------------------------
      curl --location --request POST 'http://192.168.0.157/api' \
      --header 'Content-Type: application/json' \
      --data-raw '{"op":0,"type":"HTTP_POST","set":[{"n":"","v":"5"}],"get":["loop","",""]}
    JAVASCRIPT - JSON -----------------------------------------------------------------------
      function api_request(op, type, oS, oG){
        var json = {};
        json.op = op; 
        json.type = type; 

        var getter = [];
        oG.forEach(function(item, index, array) {
          getter.push(item);
        });

        var setter = {}.set = [];
        oS.forEach(function(item, index, array) {
          setter.push({ 
            "n" : item.n,
            "v" : item.v
          })
        }); 

        json.set = setter;
        json.get = getter;  
        
        return JSON.stringify(json) ;;
      }
      console.log(api_request(0, "SOKCET", [{"n":0, "v":1}],[])
    JAVASCRIPT - XHR ------------------------------------------------------------------------
      var data = JSON.stringify({
        "op": 0,
        "type": "HTTP_POST",
        "set": [{"n":"","v":"5"}],
        "get": [
          "loop",
          "",
          ""
        ]
      });

      var xhr = new XMLHttpRequest();
      xhr.withCredentials = true;

      xhr.addEventListener("readystatechange", function() {
        if(this.readyState === 4) {
          console.log(this.responseText);
        }
      });

      xhr.open("POST", "http://192.168.0.157/api");
      xhr.setRequestHeader("Content-Type", "application/json");

      xhr.send(data);
    JAVASCRIPT - JQUERY ---------------------------------------------------------------------
      var settings = {
        "url": "http://192.168.0.157/api",
        "method": "POST",
        "timeout": 0,
        "headers": {
          "Content-Type": "application/json"
        },
        "data": JSON.stringify({
          "op": 0,
          "type": "HTTP_POST",
          "set": [{"n":"","v":"5"}],
          "get": [
            "loop",
            "",
            ""
          ]
        }),
      };

      $.ajax(settings).done(function (response) {
        console.log(response);
      });
    raw-data JSON ---------------------------------------------------------------------------
      {"op":0,"type":"HTTP_POST","set":[],"get":["loop","",""]}                   
      {"op":1024,"type":"HTTP_POST","set":[{"n":"RA_ITEM_NEXT","v":""},{"n":"RA_PLAY_DELAY","v":5}],"get":["loop","list","getter_3"]}
      {
         "op":1024,
         "type":"HTTP_POST",
         "set":[
            {
               "n":"RA_ITEM_NEXT",
               "v":""
            },
            {
               "n":"RA_PLAY_DELAY",
               "v":5
            }
         ],
         "get":[
            "getter_1",
            "getter_2",
            "getter_3"
         ]
      }

  JSON SETTER WITH REPONSE ------------------------------------------------------------------
    RA_PLAY_LT        
    RA_ITEM        
    RA_ITEM_PREV        
    RA_ITEM_RND             
    RA_ITEM_NEXT  
      loop
        pl, plt || lb, lbt    
            
    RA_PLAY_DELAY       
    RA_PLAY_DELAYMIN    
    RA_PLAY_DELAYMINON  
    RA_PLAY_DELAYMINOFF 
    RA_PLAY_RND             
    RA_PLAY_START       
    RA_PLAY_STOP        
    RA_PLAY_PAUSE       
    RA_PLAY_TOGGLE 
      loop
        statu

    RA_PLAY_PL    
      loop
        pl, plt

    RA_PLAY_LB    
      loop
        lb, lbt

    RA_LGET_PL 
      pld = pl_currentJson(uint8_t p, JsonObject & doc, boolean pI = true);
        similaire au getter "list_pl" mais possibilité de choisir la playlist et retourne avec un nom d'objet different
  
  JSON GETTER REPONSE -----------------------------------------------------------------------
      1er objetKey = getter array value from request

    'list de base'
      list_alllb  object
                  cmax  object (réelle = -1)
                  items array
                        n     object
                        cmax  object (réelle = -1)
                        items array
                              value(s)
      list_lb   object
                cmax  object (réelle = -1)
                items array 
    'playlist'
      list_pl   object
                    cmax  object (réelle = -1)
                    cnt   object (réelle = -1)  
                    pos   object   
                    lbl   object   
                    lref  object   
                    items array  
                          id    object
                          lbl   object   
                          ib    object   
                          ibcfg object  
      list_allpl    object
                    cmax  object (réelle = -1)
                    items array
                          cmax  object (réelle = -1)
                          cnt   object (réelle = -1)  
                          pos   object   
                          lbl   object   
                          lref  object   
                          items array  
                                id    object
                                lbl   object   
                                ib    object   
                                ibcfg object                      
    'list sans items'
      list_lbs
        list  object
              lb  object
                  cmax  object (réelle = -1)
                  items array
                        n     object 
                        cmax  object 
      list_pls
        list  object
              pl  object || 
                  cmax  object (réelle = -1)
                  items array
                        cmax  object 
                        cnt   object 
                        pos   object 
                        lbl   object 
                        lref  object
      list_plsc
        list  object
              plc object (return uniquement les playlist de ref)
                  cmax  object (réelle = -1)
                  items array  
                        cmax  object 
                        cnt   object 
                        pos   object 
                        lbl   object 
                        lref  object                      
    'setter names'
        list_ra   array
                  value(s)
    'req names'
        list_req  array
                  value(s)
    'autoplay'
      loop  obejct  autoplay/loop
            statu object   
                  play      object 
                  pause     object 
                  rnd       object 
                  delay     object 
                  delaymin  object 
                  rt        object 
            pl    object
                  set       object boolean
                  play      object boolean
                  pos       object uint8_t of playlist position
                  cnt       object uint8_t of total playlist
                  ib        object String item base
            plt   object
                  pos       object int
                  min       object uint8_t (réelle)
                  max       object uint8_t (réelle)
            lb    object
                  name      object const char  * of cureent list base name
                  pos       object uint8_t of current listbase position
                  cnt       object uint8_t of total listbase
                  iname     object String current listbase item name
                  icnt      object uint16_t of total item                       (réelle = -1)
                  icmax     object uint16_t of maximum size of listbase array   (réelle = -1)
            lbt   object;
                  pos       object int
                  min       object uint8_t (réelle)
                  max       object uint8_t (réelle)

    POSTCUMUL 
      listc (requete) -> list (return main object name)
        list_lbs
        list_plsc 

      list (requete + return main object name)
        list_lbs
        list_pls
      list_lbpl (requete + return main object name)
        list_lb
        list_pl
      js_init (pour le serveur web de DEMO)
        list_lb
        list_pl
        loop
        list
          list_lbs
          list_plsc 

  HTTP_GET-----------------------------------------------------------------------------------
    /edit (if edit/.html)
    /request_reset
    /statu?
      sys         arg
        system    value
        network   **
        firmware  **
        repoonse -> application/json
          device  object
                  system    object
                            Memory - Free Heap            object
                            Filesystem - Total Bytes      object
                            Filesystem - Used Bytes       object
                            Filesystem - Free Bytes       object
                            Uptime                        object
                            Memory - Heap Fragmentation   object              
                  network   object
                            Current IP                    object
                            Hostname                      object
                            AP IP                         object
                            AP SSID                       object
                            AP PSK                        object
                            STA PSK                       object
                            STA SSID                      object
                            STA                           object              
                  firmware  object          
                            Pic - Compilation Board Name  object
                            Pic - Board Name              object
                            Firmware - Framework          object
                            Firmware - Platform           object
                            Pic - cpu Frequency           object
                            Pic - Ram Size                object
                            Pic - Flash Size              object

*/

#include "main.h"

#include <Arduino.h>
#include <ArduinoJson.h>

#ifndef FSOK
  #ifdef WSERVER_LITTLEFS
    #define FSOK
  #endif
#endif
#ifdef FSOK
  #include <LittleFS.h>  
#endif

#ifdef WEBSERVEROK
  #include <Wificonnect.h>
  #include "webserver/webserver.h"
#endif

#include "program.h"
#include "tools.h"
#include "webserverRequest.h"

#ifdef WEBSERVEROK
  WifiConnect * _DeviceWifi;
  espwebServer  clientServer(80);   
#endif
#ifdef WEBSOCKETSERVEROK
  #include "webserver/socketserver.h"
  socketserver  Socketserver(81);   
#endif

Program * _Program = nullptr;

HeapStatu _HeapStatu;
HeapStatu _HeapStatu_2;

#ifdef DEBUGSERIAL
  //debug files
  #include "menu_serial.h"
  SerialRead * _serial;
  void serial_menu(const String & cmd, const String & value)    {_serial->menu();}
  void serial_ESPreset(const String & cmd, const String & value){ESP.restart();}   
  void serial_freeHeap(const String & cmd, const String & value){fsprintf("freeHeap: %d\n", ESP.getFreeHeap());}  
  void serial_menu_cmd(const String & cmd, const String & value){
    Serial.printf_P(PSTR("[serial_menu_cmd] cmd[%s] value[%s]\n"), cmd.c_str(), value.c_str());
    uint8_t p       = value.toInt();
    String  v       = "";
    int     rSize   = 0;
    String  * arg   = LH_explode(value, ',', rSize) ;
    if (rSize>0) {v = arg[1];}

    #ifdef WEBSOCKETSERVEROK
      DynamicJsonDocument doc(1024);
      JsonArray           arr;
      JsonObject          var;
      String              reponse;

      doc[F("op")]    = 0;
      doc[F("type")]  = "ESP";

      arr = doc.createNestedArray(F("set"));  
      var = arr.createNestedObject();
      var[F("n")] = FPSTR(RAALLNAMES[p]);
      var[F("v")] = v;

      arr = doc.createNestedArray(F("get"));  
      arr.add("loop");

      _WebserverRequest.parsingRequest(doc, reponse, "");
      Socketserver.sendTXT(0,reponse);
    #else
      RA action = RAARR[p];
      _Program->remote_action(action, v.c_str(), NULL);    
    #endif    
  }  
  void serial_menu_p_3(const String & cmd, const String & value){_Program->print(PM_LB); _Program->print(PM_PL); } 
  void serial_menu_p_1(const String & cmd, const String & value){_Program->print(PM_LOOP); } 
  void serial_menu_p_2(const String & cmd, const String & value){
    uint8_t cnt = ARRAY_SIZE(RAALLNAMES);
    for(int i=0; i<cnt; i++){ Serial.printf_P(PSTR("[%-3d][%-25S]\n"), i, RAALLNAMES[i]);} // debug Serial
  } 
#endif

#ifdef WEBSERVEROK
  void wehnSTAisReady(){
    clientServer.begin();
    #ifdef WEBSOCKETSERVEROK
      Socketserver.setup();  
    #endif
  }
  void wehnAPisReady(){
    clientServer.begin();
    #ifdef WEBSOCKETSERVEROK
      Socketserver.setup();  
    #endif
  }  
#endif

void _Program_handleCallback(const String itemBaseName, const uint16_t & itemBasePos, boolean updWebserver){

  String heap, time;
  on_timeD(time);
  _HeapStatu.update();_HeapStatu.print(heap);
  Serial.printf_P(PSTR("[user_callback]\n\t[%d] %s\n\t%-15s%s\n"), itemBasePos, itemBaseName.c_str(), time.c_str(), heap.c_str());
  ProgramPtrGet()->print(PM_LLI);

  if (!updWebserver) return; 
   
  #ifdef WEBSOCKETSERVEROK
      String                    rep;
      DynamicJsonDocument       reponse(2048);
      webserverRequest_reponse  * _webserverRequest_reponse = new webserverRequest_reponse[1];

      _webserverRequest_reponse[0].set_ra(RA::RA_ITEM_NEXT);
      _webserverRequest_reponse[0].make_reponse(reponse);
      serializeJson(reponse, rep); 

      delete[] _webserverRequest_reponse; 
      
      Socketserver.sendTXT(0,rep);   
  #endif
}
void webserver_parsingRequest(String s){
    DynamicJsonDocument doc(1024);  
    DeserializationError error = deserializeJson(doc, s);
    if (error) {      
        Serial.printf_P(PSTR("[webserver_parsingRequest][deserializeJson ERROR: %d]\nstring:\n\t%s"), error, s.c_str());  
    } else {
      String reponse;
      _WebserverRequest.parsingRequest(doc, reponse, "");
      // Serial.printf_P(PSTR("[webserver_parsingRequest]\n\t[request]\n\t\t%s\n\t[result]\n\t\t%s\n"), s.c_str(), reponse.c_str());  
      #ifdef WEBSOCKETSERVEROK
        Socketserver.sendTXT(0, reponse);
      #endif
    }
}  


void setup() {
  Serial.begin(115200);

  for(unsigned long const serialBeginTime = millis(); !Serial && (millis() - serialBeginTime > 5000); ) { }
  delay(300);

  HeapStatu::setupHeap_v1();

  Serial.println(F("\n##################################################\n"));

  Serial.printf_P(PSTR("\t[freeheap: %d]\n"), ESP.getFreeHeap());

  Serial.setDebugOutput(false);

  #ifdef DEBUGSERIAL
    //debug
    debug_printBuffer = new char[1024];
    _serial = new SerialRead();
    _serial->cmd_array(1, 6); 
    _serial->cmd_item_add(  1,  "menu",               "a", serial_menu);
    _serial->cmd_item_add(  1,  "ESPreset",           "z", serial_ESPreset);
    _serial->cmd_item_add(  1,  "freeHeap",           "e", serial_freeHeap);
    _serial->cmd_item_add(  1,  "debug prog",         "r", serial_menu_p_1);
    _serial->cmd_item_add(  1,  "remote action list", "t", serial_menu_p_2);
    _serial->cmd_item_add(  1,  "lb+allpl",           "y", serial_menu_p_3);
    _serial->cmd_array(2, 1); 
    _serial->cmd_item_add(  2, "remote action",       "e", serial_menu_cmd);   
    Serial.printf_P(PSTR("\t[0 | freeheap: %d]\n"), ESP.getFreeHeap()); // debug Serial
  #endif

  boolean fs = false;

  #ifdef FSOK
    fs = LittleFS.begin();

    // boolean fsFormat = false;
    // if (fsFormat) LittleFS.format();  
  #endif

  #ifdef WEBSERVEROK
    _DeviceWifi = new WifiConnect();
    _DeviceWifi->setFunc_STAinitServer( std::bind(wehnSTAisReady));
    _DeviceWifi->setFunc_APinitServer(  std::bind(wehnAPisReady));
    clientServer.filesystem_ok(fs);   
    clientServer.setup(true);  
  #endif
  #ifdef WEBSOCKETSERVEROK
    Socketserver._parse = webserver_parsingRequest;  
  #endif

  _Program = new Program(2);
  _Program->set_callback(_Program_handleCallback);

  // LISTEREF_ADD-------------------------------------------------------------------------------
    Serial.printf_P(PSTR(">>>>%d<<<\n"), ESP.getFreeHeap()); 
    _Program->initialize_lb(0, "full",  ARRAY_SIZE(LPALLNAMES)          , LPALLNAMES);
    _Program->initialize_lb(1, "cat",   ARRAY_SIZE(LPALLNAMES_CAT)      , LPALLNAMES_CAT);
    Serial.printf_P(PSTR(">>>>%d<<<\n"), ESP.getFreeHeap()); 

    // LISTEREF_CREATE
    _Program->initialize(ARRAY_SIZE(LPALLNAMES),      LPALLNAMES,     "full", SORT_TYPE::ST_AB);
    _Program->print(PM_LB); 

    // _Program->initialize(ARRAY_SIZE(LPALLNAMES_CAT),  LPALLNAMES_CAT, "cat",  SORT_TYPE::ST_BASE);
    // _Program->print(PM_LB); 
    _Program->print(PM_LBNAME); 
  //--------------------------------------------------------------------------------------------

  // PLAYLIST_INITIALIZE -----------------------------------------------------------------------
    uint8_t plC     = 5;
    uint8_t iC[]      = {10,      5,        4,        3,        2       };  // nb items max
    const char * Ln[] = {"full",  "full",   "full",   "null",   "null"  };
    _Program->initialize_playlist(plC, iC, Ln);


    // PLAYLIST ADD ITEMS
    _Program->pl_item_toArray(0, 255, "lbl_1", "Pépita",                "iref_cfg");
    _Program->pl_item_toArray(0, 255, "lbl_2", "Réglisse(Mon RéRé)",    "iref_cfg");
    _Program->pl_item_toArray(0, 255, "lbl_3", "Crevette(Pot de cole)", "iref_cfg");

    _Program->pl_item_toArray(1, 255, "lbl_1", "Philippe",  "iref_cfg");
    _Program->pl_item_toArray(1, 255, "lbl_2", "Catherine", "iref_cfg");
    _Program->pl_item_toArray(1, 255, "lbl_3", "Papie",     "iref_cfg");
    _Program->pl_item_toArray(1, 255, "lbl_3", "Axel",      "iref_cfg");
    _Program->pl_item_toArray(1, 255, "lbl_3", "Eliza",     "iref_cfg");
    _Program->pl_item_toArray(1, 255, "lbl_3", "iref_3",    "iref_cfg");
    _Program->print(PM_PL); 
  //
  //--------------------------------------------------------------------------------------------


  // SETUP PROGRAMM LOOP -----------------------------------------------------------------------
    _Program->remote_action(RA::RA_LSET_PL,     "0",  "", NULL);
    _Program->remote_action(RA::RA_PLAY_LB,     NULL);
    _Program->remote_action(RA::RA_PLAY_DELAY,  "10", "", NULL);
    _Program->remote_action(RA::RA_PLAY_STOP,   NULL);
    _Program->print(PM_LOOP);
  //--------------------------------------------------------------------------------------------
   

  String heap, time;
  on_timeD(time);_HeapStatu_2.setupHeap_v2();_HeapStatu_2.update();_HeapStatu_2.print(heap);
  Serial.printf_P(PSTR("[HEAP MONITOR]\n\t%-15s%s\n##########################\n"), time.c_str(), heap.c_str()); 
}


void loop() {

  #ifdef DEBUGSERIAL
    _serial->loop();  
  #endif

/*
static uint32_t _timer_2;
  if ((millis()-_timer_2)>10000) {
  // uint32_t tR;
  // _Program->get_remainingTime(tR); 
  // char t[100];
  // millis2time(tR, t);
   // Serial.printf("rt: %d\n", 10000 - tR );
   _timer_2 = millis(); 
    String heap;
    String time;
    on_timeD(time);
    _HeapStatu_2.update();
    _HeapStatu_2.print(heap);
    Serial.printf_P(PSTR("[HEAP MONITOR]\n\t%-15s%s\n"), time.c_str(), heap.c_str()); 
  }
*/

  #ifdef WEBSERVEROK
    _DeviceWifi->handleConnection();

    if (_DeviceWifi->WIFIsetupIsReady()) {
      ArduinoOTA.handle();
      clientServer.handleClient();
      #ifdef WEBSOCKETSERVEROK
        Socketserver.loop();  
      #endif
    }
  #endif

  ProgramPtrGet()->handle();

}


