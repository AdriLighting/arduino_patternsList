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
#include <arduinoPatternList.h>

#include <functional>



#ifndef DEBUG
  #ifdef DEBUG_WEBSERVER
    #define DEBUG
  #endif
#endif
 #ifdef DEBUG
  #define LOG(func, ...) Serial.func(__VA_ARGS__)
#else
  #define LOG(func, ...) ;
#endif



AsyncWebServer    web_server(80);
AsyncWebSocket    web_socket("/ws");
AsyncEventSource  event("/events");
Webserver         _Webserver;
Task              * _task_socketCallback = nullptr;

extern socketQueueReply * _socketQueueReply;

void not_found_server(AsyncWebServerRequest * request);




socketQueue::socketQueue() {
  _list = new QueueItemList();
  if(!_task) _task = new Task();
  _list->set_task(_task);
  
}
socketQueue::~socketQueue() {
  delete _list;
}
boolean socketQueue::receiveToQueue(const String & msg) {
  String gMsg = msg;
  if (( (_list->get_size() == 0) && (millis()-_last_item) < _timer_item)) {
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
  if(_task) {if (_task->isEnabled()) _task->execute();}
  if ( (_list->get_size() == 0) && _executeQuee ) _executeQuee = false;
  if ( (_list->get_size() > 0) && ((millis()-_last_item) > _timer_handle) && !_executeQuee ){
    _executeQuee = true;
    _task->set_callbackOstart([=](){_list->execute_cbTask();});
    _task->set_iteration_max(0);
    _task->set_taskDelay(ETD::ETD_DELAY, true, _task_delay, 1);
    _task->set_taskDelayEnabled(ETD::ETD_DELAY, true);
    _task->set_enabled(true);    
  }
}

socketQueueReply::socketQueueReply() {
  _list->set_callback([](const String & v1){_Webserver.socket_send(v1);});  
  _timer_item   = 250;    
  _task_delay   = 500;  
  _timer_handle = 500;    
}
socketQueueReply::~socketQueueReply() {
}
void socketQueueReply::receive(const String & msg) {

  if (!receiveToQueue(msg)) return;

  _task->set_callbackOstart([=](){_Webserver.socket_send(msg);});
  _task->set_iteration_max(0);
  _task->set_taskDelay(ETD::ETD_DELAY, true, 180, 1);
  _task->set_taskDelayEnabled(ETD::ETD_DELAY, true);
  _task->set_enabled(true);               
}


socketQueueSetter::socketQueueSetter() {
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
void Webserver::socketHandle()                              {if (_socketCallback) _socketCallback(_socketCallbackData);}

void _task_socketHandle()                                   {_Webserver.socketHandle();}
void socket_event(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
   LOG(printf_P, PSTR("ws[%s][%u] connect\n"), server->url(), client->id());
    _Webserver.set_socketClient(client);
    _Webserver.set_socketServer(server);
    _Webserver.set_socketIsConnected(true);
    // client->printf_P(PSTR("Hello Client %u :)"), client->id());
    client->ping();
    delay(2);

  } else if(type == WS_EVT_DISCONNECT){
   LOG(printf_P, PSTR("ws[%s][%u] disconnect\n"), server->url(), client->id());
    _Webserver.set_socketIsConnected(false);
     delay(2);

  } else if(type == WS_EVT_ERROR){
   LOG(printf_P, PSTR("ws[%s][%u] error(%u): %s\n"), server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG){
   LOG(printf_P, PSTR("ws[%s][%u] pong[%u]: %s\n"), server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      #ifdef DEBUG
        char time[12];
        _timeStamp(micros(), time);  
      #endif      
      LOG(printf_P, PSTR("ws <<<[%s][%u][%s] %s-message[%llu]: "), server->url(), client->id(), time, (info->opcode == WS_TEXT)?"text":"binary", info->len);

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
      LOG(printf, "%s\n",msg.c_str());

      if(info->opcode == WS_TEXT)  {

        delay(2);

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
         LOG(printf_P, PSTR("ws[%s][%u] %s-message start\n"), server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
       LOG(printf_P, PSTR("ws[%s][%u] frame[%u] start[%llu]\n"), server->url(), client->id(), info->num, info->len);
      }

     LOG(printf_P, PSTR("ws[%s][%u] frame[%u] %s[%llu - %llu]: "), server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

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
      LOG(printf, "%s\n",msg.c_str());

      if((info->index + len) == info->len){
       LOG(printf_P, PSTR("ws[%s][%u] frame[%u] end[%llu]\n"), server->url(), client->id(), info->num, info->len);
        if(info->final){
         LOG(printf_P, PSTR("ws[%s][%u] %s-message end\n"), server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
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

    delay(1);

    _socketClient->text(message);

    if (_socketTrace) {  
      #ifdef DEBUG
        char time[12];
        _timeStamp(micros(), time);  
        LOG(printf_P, PSTR("ws >>>[%s][%u][%s] message[%d]: %s\n"), _socketServer->url(), _socketClient->id(), time, message.length(), message.c_str());
      #endif 
    }
    
  }
  else
  {
    if (_socketTrace) {
      if (!_socketIsConnected)  {LOG(printf, "[Webserver::socket_send] Socket msg not sent, no socket connected\n");}
      if (_socketClient==NULL)  {LOG(printf, "[Webserver::socket_send] Socket msg not sent, no socket client available\n");}
    }
  }
}
void Webserver::http_send(AsyncWebServerRequest * request, const int & code, ENUM_WSTP ct, const String & data) {

  request->send(code, FPSTR(WSTP_ARR[ct]), data); 

  #ifdef DEBUG
    char time[12];
    _timeStamp(micros(), time);  
    LOG(printf_P, PSTR("http >>>[%s][%s][%s] %s-message[%d]: %s\n"), 
      request->host().c_str(), request->url().c_str(), time, WSTP_ARR[ct], data.length(), data.c_str());    
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
  if(!_task_httpCallback)   _task_httpCallback      = new Task();
  if(!_task_socketCallback) _task_socketCallback = new Task();

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

    LOG(printf_P, PSTR("[%d][New request]\n\t[method: %d][name: %s]\n\t[content-type: %s][reponse-type: %d]\n"),
     i, method, requestName, contentType, rType);

    if (method== HTTP_POST) { 
      web_server.on(requestName, HTTP_POST, [](AsyncWebServerRequest * request){}, NULL, [=](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {  
        if (_requestArray[i]._callback) {
          _httpCallbackRequest  = request;
          _httpCallbackData     = "";
          for (size_t i = 0; i < len; i++) {_httpCallbackData += (char) data[i];}
          _httpCallbackData.replace("\r\n", "");
          #ifdef DEBUG
            char time[12];
            _timeStamp(micros(), time);  
            LOG(printf_P, PSTR("http <<<[%s][%s][%s][%s] %s-message[%d]: %s\n"), 
            request->host().c_str(), request->url().c_str(), WRMTP_ARR[request->method()-1], time, request->contentType().c_str(), _httpCallbackData.length(), _httpCallbackData.c_str());    
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
            char time[12];
            _timeStamp(micros(), time);
            LOG(printf_P, PSTR("http <<<[%s][%s][%s][%s] %s\n"), 
              request->host().c_str(), request->url().c_str(), WRMTP_ARR[request->method()-1], time, request->contentType().c_str());                   
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
      LOG(printf, "UploadStart: %s\n", filename.c_str());
    LOG(printf, "%s", (const char*)data);
    if(final)
      LOG(printf, "UploadEnd: %s (%u)\n", filename.c_str(), index+len);
  });
  web_server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if(!index)
      LOG(printf, "BodyStart: %u\n", total);
    LOG(printf, "%s", (const char*)data);
    if(index + len == total)
      LOG(printf, "BodyEnd: %u\n", total);
  });  
} 

void Webserver::begin() {
  web_server.begin();
}
void Webserver::handle()  {
  web_socket.cleanupClients();
  if(_task_httpCallback)    {if (_task_httpCallback->isEnabled())   _task_httpCallback->execute();}
  if(_task_socketCallback)  {if (_task_socketCallback->isEnabled()) _task_socketCallback->execute();}
}


void not_found_server(AsyncWebServerRequest * request){
    if (_Webserver._httpTrace){
      LOG(printf, "NOT_FOUND: ");
      if (request->method() == HTTP_GET)          {LOG(printf, "GET");}
      else if(request->method() == HTTP_POST)     {LOG(printf, "POST");}
      else if(request->method() == HTTP_DELETE)   {LOG(printf, "DELETE");}
      else if(request->method() == HTTP_PUT)      {LOG(printf, "PUT");}
      else if(request->method() == HTTP_PATCH)    {LOG(printf, "PATCH");}
      else if(request->method() == HTTP_HEAD)     {LOG(printf, "HEAD");}
      else if(request->method() == HTTP_OPTIONS)  {LOG(printf, "OPTIONS");}
      else                                        {LOG(printf, "UNKNOWN");}

      LOG(printf_P, PSTR(" http://%s%s\n"), request->host().c_str(), request->url().c_str());

      if(request->contentLength()){
        LOG(printf_P, PSTR("_CONTENT_TYPE: %s\n"), request->contentType().c_str());
        LOG(printf_P, PSTR("_CONTENT_LENGTH: %u\n"), request->contentLength());
      }

      int headers = request->headers();
      int i;
      for(i=0;i<headers;i++){
        AsyncWebHeader * h = request->getHeader(i);
        LOG(printf_P, PSTR("_HEADER[%s]: %s\n"), h->name().c_str(), h->value().c_str());
      }

      int params = request->params();
      for(i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if (p->isFile())      {LOG(printf_P, PSTR("_FILE[%s]: %s, size: %u\n"), p->name().c_str(), p->value().c_str(), p->size());} 
        else if(p->isPost())  {LOG(printf_P, PSTR("_POST[%s]: %s\n"), p->name().c_str(), p->value().c_str());} 
        else                  {LOG(printf_P, PSTR("_GET[%s]: %s\n"), p->name().c_str(), p->value().c_str());}
      }
    }

    char err[100];
    sprintf_P(err, PSTR("Error: 'http://%s%s' was not found\n"), request->host().c_str(), request->url().c_str());
    request->send(404, "text/html", String(err));
}

      // request->send(200, contentType, String(ESP.getFreeHeap()));
