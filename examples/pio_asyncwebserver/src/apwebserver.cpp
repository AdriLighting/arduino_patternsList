#include "apwebserver.h"



#if defined(ESP8266)
  #include <LittleFS.h>
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#elif defined(ESP32)
  #include <FS.h>
  #include <LITTLEFS.h>
  #include <WiFi.h>
  #include <AsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>
#include <functional>

#ifndef DEBUG
  #ifdef DEBUG_WEBSERVER
    #define DEBUG
  #endif
#endif
#ifdef DEBUG
  #define LOG(func, ...) ALT_TRACEC(func, __VA_ARGS__)
#else
  #define LOG(func, ...) ;
#endif



AsyncWebServer    web_server(80);
AsyncWebSocket    web_socket("/ws");
AsyncEventSource  event("/events");
Webserver         _Webserver;
Task              * _task_socketCallback      = nullptr;
Task              * _task_socketCleanupClient = nullptr;

extern socketQueueReply * _socketQueueReply;
extern socketQueue      * _socketQueueSetter;  


void not_found_server(AsyncWebServerRequest * request);

#define AP_WS_FS
#ifdef AP_WS_FS
const char FileSystemStatu_html[] PROGMEM = "<p id=\"FileSystem_statu\" >Free Storage: %FREESPIFFS% | Used Storage: %USEDSPIFFS% | Total Storage: %TOTALSPIFFS%</p>";
const char FileSystemIndex_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8">
  <style>
    #dbg{font-family:monaco;font-size:12px;line-height:13px;color:#aaa;margin:0;padding:0;padding-left:4px}body{font-family:Arial,Helvetica,sans-serif}*{box-sizing:border-box}.form-inline{display:flex;flex-flow:row wrap;align-items:center}.form-inline label{margin:5px 10px 5px 0}.form-inline input{vertical-align:middle;margin:5px 10px 5px 0;padding:10px;background-color:#fff;border:1px solid #ddd}.form-inline button{padding:10px 20px;background-color:#1e90ff;border:1px solid #ddd;color:#fff;cursor:pointer}.form-inline button:hover{background-color:#4169e1}@media (max-width:800px){.form-inline input{margin:10px 0}.form-inline{flex-direction:column;align-items:stretch}}.row{--bs-gutter-x:1.5rem;--bs-gutter-y:0;display:flex;flex-wrap:wrap;margin-top:calc(var(--bs-gutter-y) * -1);margin-right:calc(var(--bs-gutter-x) * -.5);margin-left:calc(var(--bs-gutter-x) * -.5)}.row>*{flex-shrink:0;width:100%;max-width:100%;padding-right:calc(var(--bs-gutter-x) * .5);padding-left:calc(var(--bs-gutter-x) * .5);margin-top:var(--bs-gutter-y)}.col{width:auto}
  </style>   
</head>
<body id="body"  onload="onBodyLoad()">
  <pre onclick="clearEvent()" id="dbg"></pre>
  <p><h1>FileSystem</h1></p>
  <p id="FileSystem_statu" >Free Storage: %FREESPIFFS% | Used Storage: %USEDSPIFFS% | Total Storage: %TOTALSPIFFS%</p>
  <div class=row>
    <div class="col">
      <div class="form-inline">
        <input id="fileupload" type="file" name="data">
        <button onclick="uploadFile()">Upload File</button>
      </div>
    </div>
    <div class="col">
      <div class="form-inline">
        <input type="text" id="filePath" value="%FOPATH%">
        <button onclick="sendPath()">update</button>
      </div>
    </div>
  </div>
  <div id="FileSystem_list" >%FILELIST%</div>
  <script type="text/javascript">
    function ge(e){return document.getElementById(e)}function ce(e){return document.createElement(e)}function stb(){window.scrollTo(0,document.body.scrollHeight||document.documentElement.scrollHeight)}function clearEvent(){ge("dbg").innerText=""}function addMessage(e){var t=ce("div");t.innerText=e,ge("dbg").appendChild(t),stb()}function startEvents(){var e=new EventSource("/events");e.onopen=function(e){},e.onerror=function(e){e.target.readyState!=EventSource.OPEN&&addMessage("Events Closed")},e.onmessage=function(e){addMessage(e.data)},e.addEventListener("ota",function(e){addMessage(e.data)},!1)}function onBodyLoad(){startEvents()}function request_format_http(e,t,n){return"/"+e+"?"+t+"="+n}function request_format_httpX2(e,t,n,s,o){return"/"+e+"?"+t+"="+n+"&"+s+"="+o}function request_http(e,t){var n=new XMLHttpRequest;n.onreadystatechange=function(){4==this.readyState&&200==this.status&&t&&t(n.responseText)},n.open("GET",e,!0),n.send()}function request_send_http(e,t,n,s){request_http(request_format_http(e,t,n),s)}function request_send_httpX2(e,t,n,s,o,a){request_http(request_format_httpX2(e,t,n,s,o),a)}function sendPath(){request_send_http("FileSystem_upload","path",document.getElementById("filePath").value,null)}function downloadDeleteButton(e,t){if("delete"==t&&request_send_httpX2("FileSystem_set","name",e,"action",t,function(e){ge("FileSystem_list").innerHTML=e,request_http("/FileSystem_statu",function(e){ge("FileSystem_statu").innerHTML=e})}),"download"==t){var n="/FileSystem_set?name="+e+"&action="+t;window.open(n,"_blank")}}async function uploadFile(){let e=new FormData;e.append("file",fileupload.files[0]),await fetch("/FileSystem_upload",{method:"POST",body:e}),request_http("/FileSystem_list",function(e){ge("FileSystem_list").innerHTML=e,request_http("/FileSystem_statu",function(e){ge("FileSystem_statu").innerHTML=e})})}
  </script>  
</body>
</html>
)rawliteral";  
#endif


socketQueue::socketQueue() {
  _list = new QueueItemList();
  // if(!_task) _task = new Task();
  // _list->set_task(_task);
  
}
socketQueue::~socketQueue() {
  delete _list;
}
boolean socketQueue::receiveToQueue(const String & msg) {
  String gMsg = msg;
  if (( (_list->get_size() == 0) && (millis()-_last_item) < _timer_item)) {

      _socketQueueSetter->_list->_startHeap = ESP.getFreeHeap();
      // Serial.printf_P(PSTR("\n\n[%d]\n\n\n"), ESP.getFreeHeap());
      
      _list->addString(&gMsg);
      _last_item = millis();
      return false;
  }
  _last_item = millis();

  if (_list->get_size() > 0) {
    _list->addString(&gMsg);
    return false;}   

  return true;         
}
void socketQueue::handle(){
  // if(_task) {if (_task->isEnabled()) _task->execute();}
  if (!_task) return;
  if ( (_list->get_size() == 0) && _executeQuee ) {_executeQuee = false;
    _task_socketCleanupClient->set_enabled(true);}
  if ( (_list->get_size() > 0) && ((millis()-_last_item) > _timer_handle) && !_executeQuee ){
    LOG(APPT_DEBUGREGION_QUEUE, "[socketQueueSetter::handle]\n");
    LOG(APPT_DEBUGREGION_QUEUE, "&c:1&s:\t[T: %d] QUEUE ETD_DELAY -> %d\n", _task->get_id() , _task_delay);
    _executeQuee = true;
    _task->set_callbackOstart([=](){_list->execute_cbTask();});
    _task->set_iteration_max(0);
    _task->set_taskDelay(ETD::ETD_DELAY, true, _task_delay, 1);
    _task->set_taskDelayEnabled(ETD::ETD_DELAY, true);
    _task->set_enabled(true);    
  }
}

socketQueueReply::socketQueueReply(Task * t) {
  _task = t;
  _list->set_task(_task);

  _list->set_callback([](const String & v1){_Webserver.socket_send(v1);});  
  _timer_item   = 250;    
  _task_delay   = 500;  
  _timer_handle = 500;    

  _list->_id = 1;

}
socketQueueReply::~socketQueueReply() {
}
void socketQueueReply::receive(const String & msg) {

  if (!receiveToQueue(msg)) return;

  LOG(APPT_DEBUGREGION_QUEUE, "[socketQueueReply::receive]\n");
  LOG(APPT_DEBUGREGION_QUEUE, "&c:1&s:\t[T: %d] W/ QUEUE ETD_DELAY -> 180\n", _task->get_id());

  _task->set_callbackOstart([=](){
    _Webserver.socket_send(msg);
    _task_socketCleanupClient->set_taskDelay(ETD::ETD_DELAY, true, 1, 2);
    _task_socketCleanupClient->set_taskDelayEnabled(ETD::ETD_DELAY, true);
    _task_socketCleanupClient->set_taskDelayEnabled(ETD::ETD_TIMER, false);
    _task_socketCleanupClient->set_enabled(true);   
  });
  _task->set_iteration_max(0);
  _task->set_taskDelay(ETD::ETD_DELAY, true, 180, 1);
  _task->set_taskDelayEnabled(ETD::ETD_DELAY, true);
  _task->set_enabled(true);               
}


socketQueueSetter::socketQueueSetter(Task * t) {
  _task = t;
  _list->set_task(_task);

  _list->set_callback([](const String & v1){
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, v1);
    String reply;
    _AP_Api.parsingRequest(doc, reply, ""); 
    _socketQueueReply->receive(reply);   
  });
  _timer_item   = 250;    
  _task_delay   = 250;  
  _timer_handle = 200;  

}
socketQueueSetter::~socketQueueSetter() {
}
void socketQueueSetter::receive( DynamicJsonDocument & doc) {
  String s;
  serializeJson(doc, s);
    
  if (!receiveToQueue(s)) return;

  LOG(APPT_DEBUGREGION_QUEUE, "[socketQueueSetter::receive]\n");
  LOG(APPT_DEBUGREGION_QUEUE, "&c:1&s:\t[T: %d] W/ QUEUE ETD_DELAY -> 50\n", _task->get_id());

  String reply;
  _AP_Api.parsingRequest(doc, reply, "");
  _task->set_callbackOstart([=](){_socketQueueReply->receive(reply); });
  _task->set_iteration_max(0);
  _task->set_taskDelay(ETD::ETD_DELAY, true, 50, 1);
  _task->set_taskDelayEnabled(ETD::ETD_DELAY, true);
  _task->set_enabled(true);  

  
}




Webserver_request::Webserver_request(){
  _repFlag._cType = WSTE_APPJSON;
  _repFlag._rType = WSRM_FROMDEF;
}
Webserver_request::~Webserver_request(){}

void Webserver_request::set_request(const char * const & v1, WebRequestMethod v2, ENUM_WSTP v3) {
  _name             = v1;
  _method           = v2;
  _repFlag._cType   = v3;
}
void Webserver_request::set_request(const char * const & v1, WebRequestMethod v2, ENUM_WSTP v3, _webServerRequest_f v4) {
  set_request(v1, v2, v3);
  _callback = v4;
}
void Webserver_request::set_callback(_webServerRequest_f f) {_callback = std::move(f);}
void Webserver_request::set_rType(ENUM_WSRM v1)             {_repFlag._rType = v1;}

void Webserver_request::get_rType(ENUM_WSRM & v1)           {v1 = _repFlag._rType;}
void Webserver_request::get_name(const char * & v1)         {v1 = _name;}
void Webserver_request::get_method(WebRequestMethod & v1)   {v1 =_method;}
void Webserver_request::get_contentType(const char * & v1)  {v1 = WSTP_ARR[_repFlag._cType];}


Webserver::Webserver() {
  #if defined(DEBUG)
  _socketTrace  = true;
  _httpTrace    = true;  
  #else
  _socketTrace  = false;
  _httpTrace    = false;  
  #endif
}




Webserver::~Webserver() {
}

uint8_t Webserver::request_new(const char * const & v1, WebRequestMethod v2, ENUM_WSTP v3) {
  uint8_t cp = _requestArrayCnt;
  _requestArray[cp].set_request(v1, v2, v3);
  _requestArrayCnt++;
  return cp;
}
uint8_t Webserver::request_new(const char * const & v1, WebRequestMethod v2, ENUM_WSTP v3, _webServerRequest_f v4) {
  uint8_t cp = _requestArrayCnt;
  _requestArray[cp].set_request(v1, v2, v3, v4);
  _requestArrayCnt++;
  return cp;
}
Webserver_request * Webserver::get_requestArray(uint8_t v1) {return &_requestArray[v1];}
void Webserver::request_array(uint8_t v1) {
  _requestArray = new Webserver_request[v1];
  _requestArrayCntMax = v1;
}



void Webserver::set_socketIsConnected(boolean v1) {
  _socketIsConnected = v1;
      

}
void Webserver::set_socketClient(AsyncWebSocketClient * v1) {_socketClient = v1;}
void Webserver::set_socketServer(AsyncWebSocket * v1)       {_socketServer = v1;}
void Webserver::set_socketCallback(callback_function_t f)   {_socketCallback = std::move(f);}
// void Webserver::set_socketCallbackData(const String & v1)   {_socketCallbackData = v1;}
boolean Webserver::socketIsConnected() {return _socketIsConnected;}
void Webserver::socketHandle(const String & v1) {
  if (_socketCallback) _socketCallback(v1);
}
void socket_event(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
   LOG(APPT_DEBUGREGION_WEBSERVER, "ws[%s][%u] connect\n", server->url(), client->id());
    _Webserver.set_socketClient(client);
    _Webserver.set_socketServer(server);
    _Webserver.set_socketIsConnected(true);
    // client->printf_P(PSTR("Hello Client %u :)"), client->id());
    // client->ping();
    delay(0);

  } else if(type == WS_EVT_DISCONNECT){
   LOG(APPT_DEBUGREGION_WEBSERVER, "ws[%s][%u] disconnect\n", server->url(), client->id());
    _Webserver.set_socketIsConnected(false);
     delay(0);

  } else if(type == WS_EVT_ERROR){
   LOG(APPT_DEBUGREGION_WEBSERVER, "ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG){
   LOG(APPT_DEBUGREGION_WEBSERVER, "ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < info->len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[12];
        for(size_t i=0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t)data[i] );
          msg += buff ;
        }
      }
      LOG(APPT_DEBUGREGION_WEBSERVER, "ws <<<[%s][%u] %s-message[%llu]\n\t%s\n", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len, msg.c_str());

      if(info->opcode == WS_TEXT)  {

        _task_socketCleanupClient->set_enabled(false);

        delay(0);

        // _Webserver.set_socketCallbackData(msg);
        _task_socketCallback->set_callbackOstart(std::bind(&Webserver::socketHandle, _Webserver, msg));
        _task_socketCallback->set_taskDelay(ETD::ETD_DELAY, true, 5000, 0);
        _task_socketCallback->set_taskDelayEnabled(ETD::ETD_DELAY, true);
        _task_socketCallback->set_iteration_max(0);
        _task_socketCallback->set_enabled(true);         
      }
        // client->text("I got your text message");
      // else
        // client->binary("I got your binary message");
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if(info->index == 0){
        if(info->num == 0)
         LOG(APPT_DEBUGREGION_WEBSERVER, "ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
       LOG(APPT_DEBUGREGION_WEBSERVER, "ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[12];
        for(size_t i=0; i < len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      LOG(APPT_DEBUGREGION_WEBSERVER, "ws[%s][%u] frame[%u] %s[%llu - %llu]: %s\n", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len, msg.c_str());

      if((info->index + len) == info->len){
       LOG(APPT_DEBUGREGION_WEBSERVER, "ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if(info->final){
         LOG(APPT_DEBUGREGION_WEBSERVER, "ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          // if(info->message_opcode == WS_TEXT)
            // client->text("I got your text message");
          // else
            // client->binary("I got your binary message");
        }
      }
    }
  }
}

void Webserver::socket_send(const String & message) {
  if ((_socketIsConnected) && (_socketClient!=NULL)) {

    delay(0);

    _socketClient->text(message);

    if (_socketTrace) {  
      #ifdef DEBUG
        LOG(APPT_DEBUGREGION_WEBSERVER, "ws >>>[%s][%u] message[%d]\n\t%s\n", _socketServer->url(), _socketClient->id(), message.length(), message.c_str());
      #endif 
    }
    
  }
  else
  {
    if (_socketTrace) {
      if (!_socketIsConnected)  {LOG(APPT_DEBUGREGION_WEBSERVER, "[ERROR] no socket connected\n");}
      if (_socketClient==NULL)  {LOG(APPT_DEBUGREGION_WEBSERVER, "[ERROR] no socket client available\n");}
    }
  }
}
void Webserver::http_send(AsyncWebServerRequest * request, const int & code, ENUM_WSTP ct, const String & data) {

  request->send(code, FPSTR(WSTP_ARR[ct]), data); 

  #ifdef DEBUG
    LOG(APPT_DEBUGREGION_WEBSERVER, "http >>>[%s][%s] %s-message[%d]\n\t%s\n", 
      request->host().c_str(), request->url().c_str(), WSTP_ARR[ct], data.length(), data.c_str());    
  #endif   
  
}

void Webserver::httpHandle(AsyncWebServerRequest * request, const String & v1){
  if (request) {
    const char * requestName;
    WebRequestMethod method;
    for(int i=0; i<_requestArrayCnt; i++){
      _requestArray[i].get_name(requestName);       
      _requestArray[i].get_method(method);       
      if (request->url() == requestName && request->method() == method){
        _requestArray[i]._callback(request, i, v1);
        break;              
      }
    }
  }  
}

#ifdef AP_WS_FS
String humanReadableSize(const size_t bytes);

String FileSystem_recursiveList() {
  String returnText = "";
  returnText += "<table><tr><th align='left'>Folder</th><th align='left'>Name</th><th align='left'>Size</th><th></th><th></th></tr>";
  DynamicJsonDocument doc(10000);
  JsonObject root = doc.to<JsonObject>();
  al_tools::SPIFFS_printFiles("/", root);
  serializeJsonPretty(doc, Serial);Serial.println();
  JsonArray arr = doc[F("folders")].as<JsonArray>();
  for (size_t i = 0; i < arr.size(); i++) {
    String path = arr[i].as<String>();
    JsonArray oPath;
    if (path == "/")  oPath = doc[path][F("items")].as<JsonArray>();
    else              oPath = doc[F("/")][path][F("items")].as<JsonArray>();
    for (size_t j = 0; j < oPath.size(); j++) {
      String file = oPath[j][F("file")].as<String>();
      size_t size = oPath[j][F("size")].as<size_t>();
      returnText += "<tr align='left'><td>" + path+ "</td><td>" + file+ "</td><td>" + humanReadableSize(size) + "</td>";
      String fullpath = (path=="/")?path+file:"/"+path+"/"+file;
      returnText += "<td><button onclick=\"downloadDeleteButton(\'" + fullpath + "\', \'download\')\">Download</button>";
      returnText += "<td><button onclick=\"downloadDeleteButton(\'" + fullpath + "\', \'delete\')\">Delete</button></tr>";
    }
  }
  returnText += "</table>";
  return returnText;
}

// source: https://github.com/CelliesProjects/minimalUploadAuthESP32
String humanReadableSize(const size_t bytes) {
  if (bytes < 1024) return String(bytes) + " B";
  else if (bytes < (1024 * 1024)) return String(bytes / 1024.0) + " KB";
  else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0) + " MB";
  else return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}

String FileSystem_processor(const String& var) {
  #if defined(ESP8266)
    FSInfo fs_info;
    FILESYSTEM.info(fs_info); 
  #endif   
  if (var == "FILELIST") {
    return FileSystem_recursiveList();
  }
  if (var == "FREESPIFFS") {
    #if defined(ESP8266)
      return humanReadableSize((fs_info.totalBytes - fs_info.usedBytes));  
    #elif defined(ESP32)
      return humanReadableSize((FILESYSTEM.totalBytes() - FILESYSTEM.usedBytes()));
    #endif      
  }
  if (var == "USEDSPIFFS") {
    #if defined(ESP8266)
      return humanReadableSize(fs_info.usedBytes);
    #elif defined(ESP32)
      return humanReadableSize(FILESYSTEM.usedBytes());
    #endif
    
  }
  if (var == "TOTALSPIFFS") {
    #if defined(ESP8266)
      return humanReadableSize(fs_info.totalBytes);
    #elif defined(ESP32)
      return humanReadableSize(FILESYSTEM.totalBytes());
    #endif    
  }
  if (var == "FOPATH") {
    return _Webserver.uploadFOpath;
  }
  return String();
}

// https://github.com/smford/esp32-asyncwebserver-fileupload-example
void FileSystem_upload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  LOG(APPT_DEBUGREGION_WEBSERVER, "\n\tclient: %s - url: %s\n",  request->client()->remoteIP().toString().c_str(), request->url().c_str());
  if (!index) {
    request->_tempFile = FILESYSTEM.open(_Webserver.uploadFOpath + filename, "w");
    LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\tUpload Start: %s\n", String(filename).c_str());
    event.send("Upload in progress", "ota"); 
  }
  if (len) {
    request->_tempFile.write(data, len);
    LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\tWriting file: %s", String(filename).c_str());
    LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s: - index: %s", String(index).c_str());
    LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s: - len: %s\n", String(len).c_str());
  }
  if (final) {
    request->_tempFile.close();
    LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\tUpload Complete: %s", String(filename).c_str());
    LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s: - size: %s\n", String(String(index) + len).c_str());
    event.send("Upload finish", "ota");
    request->redirect("/FileSystem");
  }
}
#endif

void Webserver::setup(){
  if(!_task_httpCallback)   _task_httpCallback              = new Task();
  if(!_task_socketCallback) _task_socketCallback            = new Task();
  if(!_task_socketCleanupClient) _task_socketCleanupClient  = new Task();
  // if(!_task_httpCallback)         _task_httpCallback              = scheduler->get_task(4);
  // if(!_task_socketCallback)       _task_socketCallback            = scheduler->get_task(5);
  // if(!_task_socketCleanupClient)  _task_socketCleanupClient       = scheduler->get_task(6);

  web_socket.onEvent(socket_event);
  web_server.addHandler(&web_socket);

  event.onConnect([](AsyncEventSourceClient *client){
    char buffer[100];
    String time;
    al_tools::on_time_d(time);
    sprintf_P(buffer, PSTR("client connected at : %s"), time.c_str());
    client->send(buffer,NULL,millis(),1000);
  });
  web_server.addHandler(&event);

  web_server.serveStatic("/", FILESYSTEM, "/").setDefaultFile("www/index.html");


  const char        * requestName;
  WebRequestMethod  method;
  const char        * contentType;
  ENUM_WSRM         rType;
  for(int i=0; i<_requestArrayCnt; i++){

    _requestArray[i].get_method(method);
    _requestArray[i].get_name(requestName);
    _requestArray[i].get_contentType(contentType);
    _requestArray[i].get_rType(rType);

    LOG(APPT_DEBUGREGION_WEBSERVER, "[%d][New request]\n\t[method: %d][name: %s]\n\t[content-type: %s][reponse-type: %d]\n",
     i, method, requestName, contentType, rType);

    if (method== HTTP_POST) { 
      web_server.on(requestName, HTTP_POST, [](AsyncWebServerRequest * request){}, NULL, [=](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {  
        if (_requestArray[i]._callback) {
          // _httpCallbackRequest  = request;
          String sData = "";
          for (size_t i = 0; i < len; i++) {sData += (char) data[i];}
          sData.replace("\r\n", "");
          #ifdef DEBUG
            LOG(APPT_DEBUGREGION_WEBSERVER, "http <<<[%s][%s][%s] %s-message[%d]\n\t%s\n", 
            request->host().c_str(), request->url().c_str(), WRMTP_ARR[request->method()-1], request->contentType().c_str(), sData.length(), sData.c_str());    
          #endif                 
          _task_httpCallback->set_callbackOstart(std::bind(&Webserver::httpHandle, this, request, sData));
          _task_httpCallback->set_iteration_max(0);
          _task_httpCallback->set_taskDelay(ETD::ETD_DELAY, true, 5000, 0);
          _task_httpCallback->set_taskDelayEnabled(ETD::ETD_DELAY, true);
          _task_httpCallback->set_enabled(true); 
        }
      });
    }
    else if (method== HTTP_GET) {
      web_server.on(requestName, HTTP_GET, [=](AsyncWebServerRequest *request){
        if (_requestArray[i]._callback) {
          #ifdef DEBUG
            LOG(APPT_DEBUGREGION_WEBSERVER, "http <<<[%s][%s][%s] %s\n", 
              request->host().c_str(), request->url().c_str(), WRMTP_ARR[request->method()-1], request->contentType().c_str());                   
          #endif               
          // _httpCallbackRequest  = request;
          _task_httpCallback->set_callbackOstart(std::bind(&Webserver::httpHandle, this, request, ""));
          _task_httpCallback->set_iteration_max(0);
          _task_httpCallback->set_taskDelay(ETD::ETD_DELAY, true, 5000, 0);
          _task_httpCallback->set_taskDelayEnabled(ETD::ETD_DELAY, true);
          _task_httpCallback->set_enabled(true);        
        }
      });       
    }
  }

  web_server.onNotFound([](AsyncWebServerRequest *request) {
    not_found_server(request);
  });

  web_server.onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index)
      LOG(APPT_DEBUGREGION_WEBSERVER, "UploadStart: %s\n", filename.c_str());
    LOG(APPT_DEBUGREGION_WEBSERVER, "%s", (const char*)data);
    if(final)
      LOG(APPT_DEBUGREGION_WEBSERVER, "UploadEnd: %s (%u)\n", filename.c_str(), index+len);
  });
  web_server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if(!index)
      LOG(APPT_DEBUGREGION_WEBSERVER, "BodyStart: %u\n", total);
    LOG(APPT_DEBUGREGION_WEBSERVER, "%s", (const char*)data);
    if(index + len == total)
      LOG(APPT_DEBUGREGION_WEBSERVER, "BodyEnd: %u\n", total);
  });  

#ifdef AP_WS_FS
  web_server.on("/FileSystem", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", FileSystemIndex_html, FileSystem_processor);
  });
  web_server.on("/FileSystem_statu", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", FileSystemStatu_html, FileSystem_processor);
  }); 
  web_server.on("/FileSystem_list", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", FileSystem_recursiveList());
  }); 
  web_server.on("/FileSystem_set", HTTP_GET, [](AsyncWebServerRequest * request) {
    LOG(APPT_DEBUGREGION_WEBSERVER, "\n\tclient: %s - url: %s\n",  request->client()->remoteIP().toString().c_str(), request->url().c_str());
    if (request->hasParam("name") && request->hasParam("action")) {
      const char *fileName = request->getParam("name")->value().c_str();
      const char *fileAction = request->getParam("action")->value().c_str();

      LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\tfile: %s\n", fileName);
      LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\taction: %s\n", fileAction);
      
      if (!FILESYSTEM.exists(fileName)) {
        LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\tERROR: file does not exist\n");
        request->send(400, "text/plain", "ERROR: file does not exist");
      } else {
        LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\tfile exists\n");;
        if (strcmp(fileAction, "download") == 0) {
          LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\tdownloaded\n");
          request->send(FILESYSTEM, fileName, "application/octet-stream");
        } else if (strcmp(fileAction, "delete") == 0) {
          LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\tdeleted\n");
          FILESYSTEM.remove(fileName);
          event.send("File deleted", "ota");  
          request->send(200, "text/plain", FileSystem_recursiveList());
        } else {
          LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\tERROR: invalid action param supplied\n");
          request->send(400, "text/plain", "ERROR: invalid action param supplied");
        }
      }
    } else {
      request->send(400, "text/plain", "ERROR: name and action params required");
    }
  });
  web_server.on("/FileSystem_upload", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200);
      }, FileSystem_upload);
  web_server.on("/FileSystem_upload", HTTP_GET, [&](AsyncWebServerRequest * request) {
    LOG(APPT_DEBUGREGION_WEBSERVER, "http://%s%s\n", request->host().c_str(), request->url().c_str());
    int params = request->params();
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if (p->isFile())      {LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\t_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());} 
      else if(p->isPost())  {LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\t_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());} 
      else                  {LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\t_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());}
      if (p->name() == "path") {
        uploadFOpath = p->value();
      }
    }
    char buffer[100];
    sprintf_P(buffer, PSTR("filesystem download path set to : %s"), uploadFOpath.c_str());
    event.send(buffer, "ota");  
    request->send(200, "text/plain", uploadFOpath);
  });  
#endif

  _task_socketCleanupClient->set_callback([](){/*Serial.println("-");*/ web_socket.cleanupClients();});
  _task_socketCleanupClient->set_taskDelay(ETD::ETD_TIMER, true, 500, 1);
  _task_socketCleanupClient->set_taskDelay(ETD::ETD_DELAY, true, 1, 2);
  _task_socketCleanupClient->set_taskDelayEnabled(ETD::ETD_DELAY, true);
  _task_socketCleanupClient->set_taskDelayEnabled(ETD::ETD_TIMER, false);
  _task_socketCleanupClient->set_iteration_max(-1);
  _task_socketCleanupClient->set_enabled(true);   
} 


void Webserver::begin() {
  web_server.begin();
}
void Webserver::handle()  {
  
  if(_task_socketCleanupClient) {if (_task_socketCleanupClient->isEnabled()) _task_socketCleanupClient->execute();}
  if(_task_httpCallback)    {if (_task_httpCallback->isEnabled())   _task_httpCallback->execute();}
  if(_task_socketCallback)  {if (_task_socketCallback->isEnabled()) _task_socketCallback->execute();}
}


void not_found_server(AsyncWebServerRequest * request){
    if (_Webserver._httpTrace){
      String result = "";
      if (request->method() == HTTP_GET)          {result = "GET";}
      else if(request->method() == HTTP_POST)     {result = "POST";}
      else if(request->method() == HTTP_DELETE)   {result = "DELETE";}
      else if(request->method() == HTTP_PUT)      {result = "PUT";}
      else if(request->method() == HTTP_PATCH)    {result = "PATCH";}
      else if(request->method() == HTTP_HEAD)     {result = "HEAD";}
      else if(request->method() == HTTP_OPTIONS)  {result = "OPTIONS";}
      else                                        {result = "UNKNOWN";}
      LOG(APPT_DEBUGREGION_WEBSERVER, "%s http://%s%s\n", result.c_str(), request->host().c_str(), request->url().c_str());

      if(request->contentLength()){
        LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\t_CONTENT_TYPE: %s\n", request->contentType().c_str());
        LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\t_CONTENT_LENGTH: %u\n", request->contentLength());
      }

      int headers = request->headers();
      int i;
      for(i=0;i<headers;i++){
        AsyncWebHeader * h = request->getHeader(i);
        LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\t_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
      }

      int params = request->params();
      for(i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if (p->isFile())      {LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\t_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());} 
        else if(p->isPost())  {LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\t_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());} 
        else                  {LOG(APPT_DEBUGREGION_WEBSERVER, "&c:1&s:\t_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());}
      }
    }

    char err[100];
    sprintf_P(err, PSTR("Error: 'http://%s%s' was not found\n"), request->host().c_str(), request->url().c_str());
    request->send(404, "text/html", String(err));
}

      // request->send(200, contentType, String(ESP.getFreeHeap()));
