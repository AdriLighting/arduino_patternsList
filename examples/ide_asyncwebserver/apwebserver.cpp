#include "apwebserver.h"

#include <LittleFS.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <functional>

#ifndef DEBUG
  #ifdef DEBUG_WEBSERVER
    #define DEBUG
  #endif
#endif
#ifdef DEBUG
  #define LOG(func, ...) APTRACEC(func, __VA_ARGS__)
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
void Webserver::set_socketCallbackData(const String & v1)   {_socketCallbackData = v1;}
void Webserver::socketHandle() {
  if (_socketCallback) _socketCallback(_socketCallbackData);
}
void _task_socketHandle()                                   {_Webserver.socketHandle();}
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

        _Webserver.set_socketCallbackData(msg);
        _task_socketCallback->set_callbackOstart(std::bind(_task_socketHandle));
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


void Webserver::httpHandle(){
  if (_httpCallbackRequest) {
    const char * requestName;
    WebRequestMethod method;
    for(int i=0; i<_requestArrayCnt; i++){
      _requestArray[i].get_name(requestName);       
      _requestArray[i].get_method(method);       
      if (_httpCallbackRequest->url() == requestName && _httpCallbackRequest->method() == method){
        _requestArray[i]._callback(_httpCallbackRequest, i, _httpCallbackData);
        _httpCallbackData = "";
        break;              
      }
    }
  }  
}
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
    client->send("hello!",NULL,millis(),1000);
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
          _httpCallbackRequest  = request;
          _httpCallbackData     = "";
          for (size_t i = 0; i < len; i++) {_httpCallbackData += (char) data[i];}
          _httpCallbackData.replace("\r\n", "");
          #ifdef DEBUG
            LOG(APPT_DEBUGREGION_WEBSERVER, "http <<<[%s][%s][%s] %s-message[%d]\n\t%s\n", 
            request->host().c_str(), request->url().c_str(), WRMTP_ARR[request->method()-1], request->contentType().c_str(), _httpCallbackData.length(), _httpCallbackData.c_str());    
          #endif                 
          _task_httpCallback->set_callbackOstart(std::bind(&Webserver::httpHandle, this));
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
          _httpCallbackRequest  = request;
          _task_httpCallback->set_callbackOstart(std::bind(&Webserver::httpHandle, this));
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
