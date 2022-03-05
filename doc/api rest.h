/*

API REST ------------------------------------------------------------------------------------
  
  REQUETTE ----------------------------------------------------------------------------------
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
      console.log(api_request(0, "SOKCET", [{"n":"s1", "v":1},{"n":"s2", "v":2}],["g1", "g2"])
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