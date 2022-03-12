# arduino_patternsList


- [functioning](#FUNCTIONING )

---
## Lirairies

<details>
<summary>Lirairies</summary>

#### Jim Lee - leftCoast  
- ![lists](https://github.com/leftCoast/LC_baseTools) 
  -   `https://drive.google.com/file/d/1lT-wj2V2yDMdxxD3QI82Enb6GBCkEgv-/view?usp=sharing`  
- ![resizeBuff](https://github.com/leftCoast/LC_baseTools) 
  - `https://drive.google.com/file/d/1kAhTUemvxnZG_V7tRndZjkLcULPDAXVh/view?usp=sharing`  
---
#### ivanseidel    
- ![LList](https://github.com/DmytroKorniienko/FireLamp_JeeUI/blob/master/include/LList.h) 
  - `https://drive.google.com/file/d/1BZLGuW0Qv7570ETxc4MnCrJ27n74m-89/view?usp=sharing`  
---
#### Benoit Blanchon  
- ![ArduinoJson](https://github.com/bblanchon/ArduinoJson) 
--- 
#### ESP8266 
- ![framework-arduinoespressif8266 3.30002.0 (3.0.2) ](https://github.com/esp8266/Arduino) 
---
#### Grellard Adrien  
- ![AdriLighting_millisTimer](https://github.com/AdriLighting/millisTimer.git) 

</details>

<hr>

## FUNCTIONING 

### SETUP

<details>
<summary>main insatnce</summary>

```c++
Program::Program (uint8_t nbLb , boolean fs );
-nbLb   nb of basic list
-fs     filesystem management
```
><b>MAIN INSATNCE</b>
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
void Program::initialize_lb(uint8_t p, const char * name, uint8_t items, const char * const * arr);
- position of basic list array  
- id of list
- size of items array
- static const char* const items[] PROGMEM
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
void Program::initialize(const uint16_t & , const char* const* arr, const char  * const &, SORT_TYPE t = ST_BASE); 
- size of items array
- static const char* const items[] PROGMEM
- id of basic list
- classification type

```
```c++ 
// examples: 
_Program->initialize(ARRAY_SIZE(LPALLNAMES), LPALLNAMES, "full", SORT_TYPE::ST_AB);   
```

<hr>
</details>

<details>
<summary>playlist</summary>

<br>

><b>the items of the playlists correspond to the items of the basic list attach to this playlist </b>
<br>

initialization  

```c++
void Program::initialize_playlist(uint8_t , const uint8_t * const &, const char ** const &);
- nb of playlist
- playlist item size
- id of basic list
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
><b>load saved playlists items from spiff memory</b>
```c++
// examples: 
_Program->pl_fs_restore(); 
```
<hr>
</details>

### USER MANAGEMENT

<details>
<summary>global command</summary>
<br>

```c++
void Program::remote_action(RA action,  const char * const & v1 = "",  const char * const & v2 = "");  
```
><b>function used for control general list, items, etc...</b>

```c++
// examples: 
_Program->remote_action(RA::RA_ITEM,              "0");
_Program->remote_action(RA::RA_ITEM_NEXT,         );
_Program->remote_action(RA::RA_ITEM_PREV,         );
_Program->remote_action(RA::RA_ITEM_RND,          );

_Program->remote_action(RA::RA_PLAY_START,        );
_Program->remote_action(RA::RA_PLAY_STOP,         );
_Program->remote_action(RA::RA_PLAY_PAUSE,        );
_Program->remote_action(RA::RA_PLAY_TOGGLE,       );
_Program->remote_action(RA::RA_PLAY_DELAY,        "10");
_Program->remote_action(RA::RA_PLAY_DELAYMIN,     );
_Program->remote_action(RA::RA_PLAY_DELAYMINON,   );
_Program->remote_action(RA::RA_PLAY_DELAYMINOFF,  );
_Program->remote_action(RA::RA_PLAY_RND,          );

_Program->remote_action(RA::RA_PLAY_PL,           );
_Program->remote_action(RA::RA_PLAY_LB,           );
_Program->remote_action(RA::RA_PLAY_LT,           );

_Program->remote_action(RA::RA_LSET_PL,           "0");
_Program->remote_action(RA::RA_PLI_NEW,           "0");
_Program->remote_action(RA::RA_PLI_REP,           "0", "0");
_Program->remote_action(RA::RA_PLI_REM,           "0", "0");
_Program->remote_action(RA::RA_PL_TOFS,           "0");
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
><b>callback function used when an item is loaded</b>
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
  DynamicJsonDocument       reponse(2048);
  webserverRequest_reponse  * _webserverRequest_reponse = new webserverRequest_reponse[1];

  _webserverRequest_reponse[0].set_ra(RA::RA_ITEM_NEXT);
  _webserverRequest_reponse[0].make_reponse(reponse);
  serializeJson(reponse, rep); 

  delete[] _webserverRequest_reponse; 
  _Webserver.socket_send(rep);   
}

_Program->set_callback(_Program_cb);

```
<hr>
</details>

### REQUEST MANAGEMENT

<details>
<summary>parsing</summary>
<br>

```c++
class WebserverRequest;
```
><b>a static instance is already instanced</b>
```c++
extern WebserverRequest _WebserverRequest;
```
```c++
void WebserverRequest::parsingRequest(DynamicJsonDocument & doc, String & r, const String & upd);
-doc  query
-r    reponse json in String format
-upd  parmaeter for send a reponse query
```
><b>function to use to process a user request formulated with a query in json format </b>
```c++
// examples: request received from HTT_POST method

-with ESP8266WebServer library from framework-arduinoespressif8266
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
      String reponse;
      _WebserverRequest.parsingRequest(doc, reponse, "");
      server.send(200, "application/json", reponse);
    } 
    
  }  
}

-with ESPAsyncWebServer
web_server.on(requestName, HTTP_POST, [](AsyncWebServerRequest * request){}, NULL, [=](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
  String _httpCallbackData     = "";
  for (size_t i = 0; i < len; i++) {_httpCallbackData += (char) data[i];} 

  // the rest of the parsing process must be executed asynchronously, but for example I put it here      
  DynamicJsonDocument doc(2048);  
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    request->send(200, "text/plain", "");
  } else {
    String reponse;
    _WebserverRequest.parsingRequest(doc, reponse, "");
    request->send(200, "application/json", reponse);
  } 
});
```
<hr>
</details>

<hr>

## API REST

### REQUEST

<details>
<summary>request syntax</summary>

```html
HTTP_POST, UDP, SOCKET 
  JSON SYNTAX
    op    object    fonction avec laquelle traiter le parsing + execution des commandes
          0 = ?
          1 = playlist items management
    type  object    type de format (nom de commande avc String,int ETC...)
    cli   object    porvenance: http server, websocket server, udp/multi
    set   array     setter
          [
            n object    id commande
            v object    valeur commoand si besoin
          ] , 
          [**]
    get   array   getter
          [  id commande ] , 
          [**]  
```
</details>
<details>
<summary>curl</summary>
  
```html
curl --location --request POST 'http://192.168.0.157/api' \
--header 'Content-Type: application/json' \
--data-raw '{"op":0,"type":"HTTP_POST","set":[{"n":"","v":""}],"get":["loop","",""]} 
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
console.log(api_request(0, "SOKCET", [{"n":"s1", "v":1},{"n":"s2", "v":2}],["g1", "g2"])
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


### SETTER  

<details>
<summary>id with reponse</summary>

```html
RA_ITEM:             arg1: position of items list array
  loop        
    pl, plt || lb, lbt
RA_ITEM_NEXT: 
  loop        
    pl, plt || lb, lbt
RA_ITEM_PREV:
  loop        
    pl, plt || lb, lbt
RA_ITEM_RND:          
  loop        
    pl, plt || lb, lbt

RA_PLAY_START:   
  loop
    statu
RA_PLAY_STOP: 
  loop
    statu
RA_PLAY_PAUSE:  
  loop
    statu
RA_PLAY_TOGGLE:  
  loop
    statu
RA_PLAY_DELAY:       arg1: value of delay
  loop
    statu
RA_PLAY_DELAYMIN:    
  loop
    statu
RA_PLAY_DELAYMINON:  
  loop
    statu
RA_PLAY_DELAYMINOFF: 
  loop
    statu
RA_PLAY_RND:         
  loop
    statu

RA_PLAY_PL:   
   loop
    pl, plt
              
RA_PLAY_LB:   
  loop
    lb, lbt

RA_PLAY_LT:  
  loop        
    pl, plt || lb, lbt

RA_LSET_PL:         arg1: position of playlist list array

RA_LGET_PL:
  pld = pl_currentJson(uint8_t p, JsonObject & doc, boolean pI = true);
    similaire au getter "list_pl" mais possibilité de choisir la playlist et retourne avec un nom d'objet different    

RA_PLI_NEW:         arg1 
  list_allpl      
RA_PLI_REP:         arg1, arg2 
  list_allpl
RA_PLI_REM:         arg1, arg2  
  list_allpl    
RA_PL_TOFS:         arg1: position of playlist list array
```
<hr>
</details>

### GETTER  

#### BASE-LIST WITH ITEMS 

<details>
<summary>list_alllb</summary>

```html
  list_alllb  object
              cmax  object (réelle = -1)
              items array
                    n     object
                    cmax  object (réelle = -1)
                    items array
                          value(s)
```
<hr>
</details>
<details>
<summary>list_lb</summary>

```html
  list_lb   object
            cmax  object (réelle = -1)
            items array 
```
<hr>
</details>

#### BASE-LIST WITHOUT ITEMS 

<details>
<summary>list_lbs</summary>

```html
  list_lbs
    list  object
          lb  object
              cmax  object (réelle = -1)
              items array
                    n     object 
                    cmax  object 
```
<hr>
</details>

#### PLAYLIST WITH ITEMS 

<details>
<summary>list_pl</summary>

```html
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
```
<hr>
</details>
<details>
<summary>list_allpl</summary>

```html
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
```
<hr>
</details>

#### PLAYLIST WITHOUT ITEMS 

<details>
<summary>list_pls</summary>

```html
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
```
<hr>
</details>
<details>
<summary>list_plsc</summary>

```html
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
```
<hr>
</details>

#### PLAYING STATU

<details>
<summary>loop</summary>

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

#### DIVERS
<details>
<summary>list_ra</summary>

```html
    list_ra   array
              value(s)
```
<hr>
</details>

<details>
<summary>list_req</summary>

```html
    list_req  array
              value(s)
```
</details>