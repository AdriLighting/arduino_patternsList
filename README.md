# arduino_patternsList
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

## FONCTIONNEMENT

### SETUP

<details>
<summary>MAIN INSATNCE</summary>

```c++
MAIN INSATNCE  
  Program (uint8_t nbLb , boolean fs );
  nbLb  nb of basic list
  fs    filesystem management
EX:
  boolean fs = FILESYSTEM.begin();
  _Program = new Program(2, fs);
```
<hr>
</details>
<details>
<summary>basic lists</summary>

```c++
initialization of basic lists 
  void initialize_lb(uint8_t p, const char * name, uint8_t items, const char * const * arr);
  p       position of basic list array  
  name    id of list
  items   size of items array
  arr     static const char* const items[] PROGMEM
EX: 
  _Program->initialize_lb(0, "full",  ARRAY_SIZE(LPALLNAMES)          , LPALLNAMES);
  _Program->initialize_lb(1, "cat",   ARRAY_SIZE(LPALLNAMES_CAT)      , LPALLNAMES_CAT);

loading one of the basic list     
  void initialize(const uint16_t & , const char* const* arr, const char  * const &, SORT_TYPE t = ST_BASE); 
          size of items array
          static const char* const items[] PROGMEM
          id of basic list
          classification type 
EX:
  _Program->initialize(ARRAY_SIZE(LPALLNAMES), LPALLNAMES, "full", SORT_TYPE::ST_AB);

```
<hr>
</details>
<details>
<summary>PLAYLIST</summary>

```c++
the items of the playlists correspond to the items of the basic list attach to this playlist 
initialization
  void initialize_playlist(uint8_t , const uint8_t * const &, const char ** const &);
          nb of playlist
          playlist item size
          id of basic list
  EX:
    uint8_t plC       = 5;
    uint8_t iC[]      = {20,      20,        20,      0,        0       };  // nb items max
    const char * Ln[] = {"full",  "full",   "full",   "null",   "null"  };
    _Program->initialize_playlist(plC, iC, Ln);  
```
<hr>
</details>

<hr>

## API REST

### REQUEST

<details>
<summary>REQUEST</summary>

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

<details>
<summary>CURL</summary>
  
```html
curl --location --request POST 'http://192.168.0.157/api' \
--header 'Content-Type: application/json' \
--data-raw '{"op":0,"type":"HTTP_POST","set":[{"n":"","v":""}],"get":["loop","",""]} 
```
<hr>  
</details>  
<details>
<summary>JAVASCRIPT - JSON</summary>
  
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
<summary>JAVASCRIPT - XHR</summary>
  
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
</details>


### SETTER  

<details>
<summary>ID WITH REPONSE</summary>

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