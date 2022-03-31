# arduino_patternsList


- [setup](#SETUP )
- [mangement](#USER_MANAGEMENT)
- [request](#REQUEST_FUNCTION)
- [api](#API_REST)

---
## Lirairies

<details>
<summary>Lirairies</summary>

#### Jim Lee - leftCoast  
- ![LC_baseTools](https://github.com/leftCoast/LC_baseTools.git) 
---
#### ivan seidel    
- ![LinkedList](https://github.com/ivanseidel/LinkedList.git)  
i hav used my fork  
- ![LinkedList](https://github.com/AdriLighting/LinkedList.git)
---
#### Benoit Blanchon  
- ![ArduinoJson](https://github.com/bblanchon/ArduinoJson) 
--- 
#### ESP8266 
- ![framework-arduinoespressif8266 3.30002.0 (3.0.2) ](https://github.com/esp8266/Arduino) 
---
#### Grellard Adrien  
- ![millisTimer](https://github.com/AdriLighting/arduino_ALmillisTimer.git) 

</details>

<hr>

## FUNCTIONING 

### PREPROCESSOR

<details>
<summary>define</summary>

<br>
use the define via the options of your IDE or from the target files    
<br>
<br>

if AP_DEFAULT isnt defined we set default define (see include/def.h)
````c++
#define AP_DEFAULT        // include/def.h
```` 
if FSOK is defined the filesystem can be used  
define USE_SPIFFS for SPIFFS or define USE_LITTLEFS for LittleFS  
````c++
#define FSOK              // include/def.h
#define USE_LITTLEFS      // include/def.h   
#define USE_SPIFFS        // include/def.h
````
keyboard for serial monitor  
````c++
#define DEBUGSERIAL       // include/def.h
````
trace macro 
````c++
#define DEBUG_AP          // include/def.h
````
degug 
````c++
#define DEBUG_BASICLIST   // src/patterns.cpp  
#define DEBUG_PROGRAM     // src/program.cpp
#define DEBUG_PLAYLIST    // src/playlist.cpp
#define DEBUG_TASK        // examples/..._asyncwebserver/TaskScheduler.cpp
#define DEBUG_WEBSERVER   // examples/..._asyncwebserver/apwebserver.cpp
#define DEBUG_QUEUE       // examples/..._asyncwebserver/queue.cpp
````

</details>

### SETUP

<details>
<summary>main insatnce</summary>

```c++
/**
 * @brief      Constructs a new instance.
 *
 * @param[in]  nbLb  nb of basic list
 * @param[in]  fs    enabled filesystem management
 */
Program::Program (uint8_t nbLb , boolean fs );
```
**`MAIN INSATNCE`**
```c++
// examples:
Program * _Program = nullptr;
boolean fs = FILESYSTEM.begin();
_Program = new Program(2, fs);

// examples:
Program * _Program = nullptr;
_Program = new Program(2, false);
```
<hr>
</details>

<details>
<summary>basic lists</summary>
<br>
initialization  

```c++
/**
 * @brief      setup basiclist.
 *
 * @param[in]  p      position of basic list array  
 * @param[in]  name   id of list
 * @param[in]  items  size of items array
 * @param      arr    static const char* const items[] PROGMEM
 */
void Program::initialize_lb(uint8_t p, const char * name, uint8_t items, const char * const * arr);
```  
```c++
// examples: 
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))  
static const char LPNAME_001[] PROGMEM = "toto";
static const char LPNAME_002[] PROGMEM = "tata";
static const char LPNAME_003[] PROGMEM = "tonton";
static const char LPNAME_004[] PROGMEM = "felix";
static const char LPNAME_005[] PROGMEM = "mimine";
static const char LPNAME_006[] PROGMEM = "the cat";
static const char* const LPALLNAMES[] PROGMEM = {
  LPNAME_001, LPNAME_002, LPNAME_003
};  
static const char* const LPALLNAMES_CAT[] PROGMEM = {
  LPNAME_004, LPNAME_005, LPNAME_006
};  
_Program->initialize_lb(0, "full",  ARRAY_SIZE(LPALLNAMES)          , LPALLNAMES);
_Program->initialize_lb(1, "cat",   ARRAY_SIZE(LPALLNAMES_CAT)      , LPALLNAMES_CAT);  
```  
<br>
loading one of the basic list   

```c++ 
/**
 * @brief      loading one of the basic list   
 *
 * @param[in]  <unnamed>  size of items array
 * @param      arr        static const char* const items[] PROGMEM
 * @param      <unnamed>  id of basic list
 * @param[in]  t          classification type
 */
void Program::initialize(const uint16_t & , const char* const* arr, const char  * const &, apListSortType_t t = ST_BASE); 
```
```c++ 
// examples: 
_Program->initialize(ARRAY_SIZE(LPALLNAMES), LPALLNAMES, "full", apListSortType_t::ST_AB);   
```

<hr>
</details>

<details>
<summary>playlist</summary>

<br>

**`the items of the playlists correspond to the items of the basic list attach to this playlist`**

<br>

initialization  

```c++
/**
 * @brief      Initializes the playlist.
 *
 * @param[in]  <unnamed>  nb of playlist
 * @param      <unnamed>  playlist item size
 * @param      <unnamed>  id of basic list
 */
void Program::initialize_playlist(uint8_t , const uint8_t * const &, const char ** const &);
```
```c++
// examples: 
uint8_t plC       = 5;
uint8_t iC[]      = {20,      20,        20,      0,        0       };  // nb items max
const char * Ln[] = {"full",  "full",   "full",   "null",   "null"  };
_Program->initialize_playlist(plC, iC, Ln);  
```
<hr>
</details>

<details>
<summary>filesystem</summary>

```c++
void Program::pl_fs_restore();  
```
**`load saved playlists items from spiff memory`**
```c++
// examples: 
_Program->pl_fs_restore(); 
```
<hr>
</details>

<details>
<summary>program handle</summary>

```c++
void Program::handle();  
```
**`can be used in ur main loop`**
```c++
// examples: 
void loop() {
  _Program->handle();
}
```
<hr>
</details>

<details>
<summary>user api</summary>
<br>

```c++
class AP_userApi;
```
**`a static instance is already instanced`**
```c++
extern AP_userApi _AP_userApi;
```
```c++
/**
 * @brief      Initializes then aray int the void setup().
 *
 * @param[in]  size of AP_userApiItem object array
 */
void AP_userApi::initialize(uint8_t cmax)
```
```c++
// examples: 
void setup() {
  _AP_userApi.initialize(2);
}
```
```c++
typedef std::function<void(const String & v1, DynamicJsonDocument & doc)> _wsur_cb_f;
```
```c++
/**
 * @param[in]   p     position of the obejct array
 * @param[in]   id    id used when an API getter request is received 
 * @param[in]   f     The callback associate with identifier         
 */
void AP_userApi::set_request(uint8_t p, const char * const & id, _wsur_cb_f f)
```
**`function to initialize a request with its return function allowing to add the response to the json object `**
```c++
// examples: 
void setup() {
  _AP_userApi.initialize(2);
  _AP_userApi.set_request(0, "user", [](const String & v1, DynamicJsonDocument & doc){
    Serial.printf("[user getter][req: %s]\n", v1.c_str());
    JsonObject var = doc.createNestedObject(FPSTR(APPT_REQ_005));
    _Program->get_json_jsInit(var);});
}
```



<hr>
</details>

### USER_MANAGEMENT

<details>
<summary>global command</summary>
<br>

```c++
void Program::remote_action(apSetter_t action,  const char * const & v1 = "",  const char * const & v2 = "");  
```
**`function used for control general list, items, etc...`**
```c++
// examples: 
_Program->remote_action(apSetter_t::APSET_ITEM,              "0");
_Program->remote_action(apSetter_t::APSET_ITEM_NEXT,         );
_Program->remote_action(apSetter_t::APSET_ITEM_PREV,         );
_Program->remote_action(apSetter_t::APSET_ITEM_RND,          );

_Program->remote_action(apSetter_t::APSET_PLAY_START,        );
_Program->remote_action(apSetter_t::APSET_PLAY_STOP,         );
_Program->remote_action(apSetter_t::APSET_PLAY_PAUSE,        );
_Program->remote_action(apSetter_t::APSET_PLAY_TOGGLE,       );
_Program->remote_action(apSetter_t::APSET_PLAY_DELAY,        "10");
_Program->remote_action(apSetter_t::APSET_PLAY_DELAYMIN,     );
_Program->remote_action(apSetter_t::APSET_PLAY_DELAYMINON,   );
_Program->remote_action(apSetter_t::APSET_PLAY_DELAYMINOFF,  );
_Program->remote_action(apSetter_t::APSET_PLAY_RND,          );

_Program->remote_action(apSetter_t::APSET_PLAY_PL,           );
_Program->remote_action(apSetter_t::APSET_PLAY_LB,           );
_Program->remote_action(apSetter_t::APSET_PLAY_LT,           );

_Program->remote_action(apSetter_t::APSET_LSET_PL,           "0");
_Program->remote_action(apSetter_t::APSET_PLI_NEW,           "0");
_Program->remote_action(apSetter_t::APSET_PLI_REP,           "0", "0");
_Program->remote_action(apSetter_t::APSET_PLI_REM,           "0", "0");
_Program->remote_action(apSetter_t::APSET_PL_TOFS,           "0");
```
<hr>
</details>
</details>

<details>
<summary>items roation</summary>
<br>

```c++
typedef std::function<void(const String & v1, const uint16_t & v2, boolean upd)> callback_function_t;
void Program::set_callback(callback_function_t);
```
**`callback function used when an item is loaded`**
```c++
// examples: 
void _Program_cb(const String itemBaseName, const uint16_t & itemBasePos, boolean updWebserver){

  String heap, time;
  on_timeD(time);
  _HeapStatu.update();_HeapStatu.print(heap);
  Serial.printf_P(PSTR("[user_callback]\n\t[%d] %s\n\t%-15s%s\n"), itemBasePos, itemBaseName.c_str(), time.c_str(), heap.c_str());
  ProgramPtrGet()->print(PM_LLI);

  if (!updWebserver) return; 
   
  String                    rep;
  DynamicJsonDocument       reply(2048);
  AP_ApiReply  * _webserverRequest_reply = new AP_ApiReply[1];

  _webserverRequest_reply[0].set_ra(apSetter_t::APSET_ITEM_NEXT);
  _webserverRequest_reply[0].reply_generate(reply);
  serializeJson(reply, rep); 

  delete[] _webserverRequest_reply; 
  _Webserver.socket_send(rep);   
}

_Program->set_callback(_Program_cb);

```
<hr>
</details>

<hr>

## API_REST

### REQUEST_FUNCTION

<details>
<summary>parsing</summary>
<br>

```c++
class AP_Api;
```
**`a static instance is already instanced`**
```c++
extern AP_Api _AP_Api;
```
```c++
/**
 * @brief      function to use to process a user request formulated with a query in json format
 *
 * @param[out]  doc   query
 * @param[out]  r     reply json in String format
 * @param[in]   upd   parmaeter for send a reply query
 */
void AP_Api::parsingRequest(DynamicJsonDocument & doc, String & r, const String & upd);
```
**`function to use to process a user request formulated with a query in json format`**
```c++
// examples: request received from HTT_POST method

- with ESP8266WebServer library from framework-arduinoespressif8266
server.on("/api", HTTP_POST, std::bind(&espwebServer::handleJson, this));
void espwebServer::handleJson() {
  if (server.method() == HTTP_POST) {
    String json;
    for (uint8_t i = 0; i < server.args(); i++) {json +=  server.arg(i) + "\n";}        
    DynamicJsonDocument doc(2048);  
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
      server.send(200, "text/plain", "");
    } else {
      String reply;
      _AP_Api.parsingRequest(doc, reply, "");
      server.send(200, "application/json", reply);
    } 
    
  }  
}

- with ESPAsyncWebServer
web_server.on(requestName, HTTP_POST, [](AsyncWebServerRequest * request){}, NULL, [=](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
  String _httpCallbackData     = "";
  for (size_t i = 0; i < len; i++) {_httpCallbackData += (char) data[i];} 

  // the rest of the parsing process must be executed asynchronously, but for example I put it here      
  DynamicJsonDocument doc(2048);  
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    request->send(200, "text/plain", "");
  } else {
    String reply;
    _AP_Api.parsingRequest(doc, reply, "");
    request->send(200, "application/json", reply);
  } 
});
```
<hr>
</details>

### REQUEST

<details>
<summary>request syntax</summary>

```html
HTTP_POST, UDP, SOCKET 
  JSON SYNTAX

    op    object    fonction avec laquelle traiter le parsing + execution des commandes 
          0 = ?
          1 = playlist items management

    type  object    (optional?) type de format (nom de commande avc String,int ETC...)  

    cli   object    (optional?) porvenance: http server, websocket server, udp/multi

    set   array     setter
          [
            {"id setter ":"value of setter"},
          ] 

    get   array   simple
          [  "id commande", "..." ] 

    get   array   advanced
          [  "", {"":["",""]}] 

    get   array   advanced
          [  "", {"":{"":""}}] 
          

// example:
{"op":0,"cli":"","set":[{"n":"1","v":"1"}, {"n":"2","v":"2"}],"get":["list",{"loop_select":["statu", "lb"]}]}   
{"op":0,"cli":"","set":[],"get":[{"gv":{"n":"list_pld","v":"1"}}]}       
  {"op":0,"cli":"HTTP_POST","set":[],"get":["loop"]}   
  {"op":0,"cli":"SOKCET","set":[{"n":"APSET_ITEM_NEXT","v":""}],"get":[]}       
  {"op":0,"cli":"SOKCET","set":[{"n":"APSET_PLAY_DELAY","v":"35"}],"get":[]}       
```
</details>
<details>
<summary>curl</summary>
  
```html
curl  --location --request POST 'http://192.168.0.157/api'  \
      --header 'Content-Type: application/json'             \
      --data-raw '{"op":0,"type":"HTTP_POST","set":[{"n":"APSET_PLAY_DELAY","v":"35"}],"get":["loop","list_pl",""]}'
```
<hr>  
</details>  
<details>
<summary>javascript - json</summary>
  
```javascript
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
console.log(api_request(0, "SOKCET", [{"n":"APSET_PLAY_DELAY", "v":10},{"n":"APSET_PLAY_START", "v":""}],["loop", "list_pl"])
```
<hr>  
</details> 
<details>
<summary>javascript - xhr</summary>
  
```javascript
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
```
<hr>  
</details>


#### SETTER  

<details>
<summary>id with reply</summary>

```html
APSET_ITEM:             arg1: position of items list array
  loop        
    pl, plt || lb, lbt
APSET_ITEM_NEXT: 
  loop        
    pl, plt || lb, lbt
APSET_ITEM_PREV:
  loop        
    pl, plt || lb, lbt
APSET_ITEM_RND:          
  loop        
    pl, plt || lb, lbt

APSET_PLAY_START:   
  loop
    statu
APSET_PLAY_STOP: 
  loop
    statu
APSET_PLAY_PAUSE:  
  loop
    statu
APSET_PLAY_TOGGLE:  
  loop
    statu
APSET_PLAY_DELAY:       arg1: value of delay
  loop
    statu
APSET_PLAY_DELAYMIN:    
  loop
    statu
APSET_PLAY_DELAYMINON:  
  loop
    statu
APSET_PLAY_DELAYMINOFF: 
  loop
    statu
APSET_PLAY_RND:         
  loop
    statu

APSET_PLAY_PL:   
   loop
    pl, plt
              
APSET_PLAY_LB:   
  loop
    lb, lbt

APSET_PLAY_LT:  
  loop        
    pl, plt || lb, lbt

APSET_LSET_PL:         arg1: position of playlist list array

APSET_LGET_PL:
  pld = pl_currentJson(uint8_t p, JsonObject & doc, boolean pI = true);
    similaire au getter "list_pl" mais possibilité de choisir la playlist et retourne avec un nom d'objet different    

APSET_PLI_NEW:         arg1 
  list_allpl      
APSET_PLI_REP:         arg1, arg2 
  list_allpl
APSET_PLI_REM:         arg1, arg2  
  list_allpl    
APSET_PL_TOFS:         arg1: position of playlist list array
```
<hr>
</details>

#### GETTER  

<details>
<summary>id</summary>

##### BASIC-LIST WITH ITEMS 


`get all items of all basic list`  

<details>
<summary>list_alllb</summary>

```json
{"op":0,"cli":"HTTP_POST","set":[],"get":["list_alllb"]}
```
```html
  list_alllb  object
              cmax  object          nb of list
              items array
                    n     object    list name
                    cmax  object    nb of maximum items
                    items array
                          value(s)  item names
```
<hr>
</details>

`get all items of the current basic list`

<details>
<summary>list_lb</summary>

```json
{"op":0,"cli":"HTTP_POST","set":[],"get":["list_lb"]}
```
```html
  list_lb   object
            cmax  object          nb of maximum items
            items array           item names
```
<hr>
</details>



##### BASIC-LIST WITHOUT ITEMS 


`get all basic list name and size`

<details>
<summary>list_lbs</summary>

```json
{"op":0,"cli":"HTTP_POST","set":[],"get":["list_lbs"]}
```
```html
  list_lbs
    list  object
          lb  object
              cmax  object                        nb of list
              items array
                    n     object                  list name
                    cmax  object                  nb of maximum items
```
</details>

<hr>

##### PLAYLIST WITH ITEMS 

`get all items object of current playlist`

<details>
<summary>list_pl</summary>

```json
{"op":0,"cli":"HTTP_POST","set":[],"get":["list_pl"]}      
```    
```html
  list_pl   object
                cmax  object                      maximum item size
                cnt   object                      current item size
                pos   object                      list array position
                lbl   object                      label of the lsit
                lref  object                      basiclist reference
                items array       
                      id    object                unique id of the item
                      lbl   object                label
                      ib    object                basiclist item
                      ibcfg object                extra
```
<hr>
</details>

`get all items object of all playlist`

<details>
<summary>list_allpl</summary>

```json
{"op":0,"cli":"HTTP_POST","set":[],"get":["list_allpl"]}      
```   
```html
  list_allpl    object 
                items array
                      cmax  object                maximum item size
                      cnt   object                current item size
                      pos   object                list array position
                      lbl   object                label of the lsit
                      lref  object                basiclist reference
                      items array       
                            id    object          unique id of the item
                            lbl   object          label
                            ib    object          basiclist 
                            ibcfg object          extra 
```
<hr>
</details>

`get all items object of slected playlist`

<details>
<summary>list_pld</summary>

```json
{"op":0,"type":"HTTP_POST","set":[],"get":[{"gv":{"n":"list_pld","v":"0"}}]}      
```   
```html
  list_pld      object 
                cmax  object                maximum item size
                cnt   object                current item size
                pos   object                list array position
                lbl   object                label of the lsit
                lref  object                basiclist reference
                items array       
                      id    object          unique id of the item
                      lbl   object          label
                      ib    object          basiclist 
                      ibcfg object          extra 
```
<hr>
</details>

##### PLAYLIST WITHOUT ITEMS 

`get all playlist`

<details>
<summary>list_pls</summary>

```json
{"op":0,"cli":"HTTP_POST","set":[],"get":["list_pls"]}      
```   
```html
  list_pls
    list  object
          pl  object
              cmax  object                      playlist size                                
              items array
                    cmax  object                maximum item size
                    cnt   object                current item size
                    pos   object                list array position
                    lbl   object                label of the lsit
                    lref  object                basiclist reference
```
<hr>
</details>

`get all playlist by basiclist reference`

<details>
<summary>list_plsc</summary>

```json
{"op":0,"cli":"HTTP_POST","set":[],"get":["list_plsc"]}      
```   
```html
  list_plsc
    list  object
          plc object (return uniquement les playlist de ref)
              cmax  object                      playlist size                                
              items array
                    cmax  object                maximum item size
                    cnt   object                current item size
                    pos   object                list array position
                    lbl   object                label of the lsit
                    lref  object                basiclist reference
```
</details>

<hr>

##### STATU

`currents statu of player, basiclist, playlist`

<details>
<summary>loop</summary>

```json
{"op":0,"cli":"HTTP_POST","set":[],"get":[{"loop_select":["statu"]}]}      
```  
```json
{"op":0,"cli":"HTTP_POST","set":[],"get":["statu"]}      
```  
```html
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
              name      object const char  * of cureent basiclist  name
              pos       object uint8_t of current basiclist position
              cnt       object uint8_t of total basiclist
              iname     object String current basiclist item name
              icnt      object uint16_t of total item                       (réelle = -1)
              icmax     object uint16_t of maximum size of listbase array   (réelle = -1)
        lbt   object;
              pos       object int
              min       object uint8_t (réelle)
              max       object uint8_t (réelle) 
```
<hr>
</details>

</details>

<hr>

##### DIVERS

`list of avaible setter`

<details>
<summary>list_ra</summary>

```json
{"op":0,"cli":"HTTP_POST","set":[],"get":["list_ra"]}      
```  
```html
    list_ra   array
              value(s)
```
<hr>
</details>

`list of avaible getter`

<details>
<summary>list_req</summary>

```json
{"op":0,"cli":"HTTP_POST","set":[],"get":["list_req"]}      
```  
```html
    list_req  array
              value(s)
```
</details>
