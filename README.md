# arduino_patternsList
---
## Lirairies

<details>
<summary>List</summary>

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

## API REST

### REQUEST

<details>
<summary>REQUEST</summary>

```html
HTTP, JS, UDP, SOCKET 
  JSON SYNTAX
    op    object  fonction avec laquelle traiter le parsing + execution des commandes
          0 = ?
          1 = playlist items management
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
```

<details>
<summary>CURL</summary>
  
```html
curl --location --request POST 'http://192.168.0.157/api' \
--header 'Content-Type: application/json' \
--data-raw '{"op":0,"type":"HTTP_POST","set":[{"n":"","v":"5"}],"get":["loop","",""]} 
```
  
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
  
</details>
</details>


### SETTER  

<details>
<summary>NAMES WITH REPONSE</summary>

```html
RA_ITEM:             arg1
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
RA_PLAY_DELAY:       arg1
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

RA_LSET_PL:         arg1

RA_LGET_PL:
  pld = pl_currentJson(uint8_t p, JsonObject & doc, boolean pI = true);
    similaire au getter "list_pl" mais possibilité de choisir la playlist et retourne avec un nom d'objet different    

RA_PLI_NEW:         arg1 
  list_allpl      
RA_PLI_REP:         arg1, arg2 
  list_allpl
RA_PLI_REM:         arg1, arg2  
  list_allpl    
RA_PL_TOFS:         arg1


  
```
</details>