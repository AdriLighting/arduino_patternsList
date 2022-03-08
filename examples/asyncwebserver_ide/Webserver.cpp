#include "Webserver.h"

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

AsyncWebServer    web_server(80);
AsyncWebSocket    web_socket("/ws");
AsyncEventSource  event("/events");
Webserver 				_Webserver;

extern String _socket_msg_received;

Webserver::Webserver() {

}

Webserver::~Webserver() {

}


void Webserver::set_socketIsConnected(boolean v1) {
	_socket_is_connected = v1;
}
void Webserver::set_socketClient(AsyncWebSocketClient * v1) {
	_websocket_client = v1;
}

void socket_event(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  
  if(type == WS_EVT_CONNECT){
    Serial.printf_P(PSTR("ws[%s][%u] connect\n"), server->url(), client->id());
    _Webserver.set_socketClient(client);
    _Webserver.set_socketIsConnected(true);
    client->printf_P(PSTR("Hello Client %u :)"), client->id());
    client->ping();

  } else if(type == WS_EVT_DISCONNECT){
    Serial.printf_P(PSTR("ws[%s][%u] disconnect\n"), server->url(), client->id());
    _Webserver.set_socketIsConnected(false);

  } else if(type == WS_EVT_ERROR){
    Serial.printf_P(PSTR("ws[%s][%u] error(%u): %s\n"), server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG){
    Serial.printf_P(PSTR("ws[%s][%u] pong[%u]: %s\n"), server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      Serial.printf_P(PSTR("ws[%s][%u] %s-message[%llu]: "), server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);

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
      Serial.printf("%s\n",msg.c_str());

      if(info->opcode == WS_TEXT)  {
				_Webserver._url		   = server->url();
				_Webserver._id 		   = client->id();
      	_socket_msg_received = msg;}
        // client->text("I got your text message");
      // else
        // client->binary("I got your binary message");
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if(info->index == 0){
        if(info->num == 0)
          Serial.printf_P(PSTR("ws[%s][%u] %s-message start\n"), server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
        Serial.printf_P(PSTR("ws[%s][%u] frame[%u] start[%llu]\n"), server->url(), client->id(), info->num, info->len);
      }

      Serial.printf_P(PSTR("ws[%s][%u] frame[%u] %s[%llu - %llu]: "), server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

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
      Serial.printf("%s\n",msg.c_str());

      if((info->index + len) == info->len){
        Serial.printf_P(PSTR("ws[%s][%u] frame[%u] end[%llu]\n"), server->url(), client->id(), info->num, info->len);
        if(info->final){
          Serial.printf_P(PSTR("ws[%s][%u] %s-message end\n"), server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          // if(info->message_opcode == WS_TEXT)
            // client->text("I got your text message");
          // else
            // client->binary("I got your binary message");
        }
      }
    }
  }
}

void Webserver::setup(){

  web_socket.onEvent(socket_event);
  web_server.addHandler(&web_socket);

  event.onConnect([](AsyncEventSourceClient *client){
    client->send("hello!",NULL,millis(),1000);
  });
  web_server.addHandler(&event);

  web_server.serveStatic("/", LittleFS, "/").setDefaultFile("www/index.html");

  web_server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  web_server.onNotFound([](AsyncWebServerRequest *request){
    Serial.printf("NOT_FOUND: ");
    if(request->method() == HTTP_GET)
      Serial.printf("GET");
    else if(request->method() == HTTP_POST)
      Serial.printf("POST");
    else if(request->method() == HTTP_DELETE)
      Serial.printf("DELETE");
    else if(request->method() == HTTP_PUT)
      Serial.printf("PUT");
    else if(request->method() == HTTP_PATCH)
      Serial.printf("PATCH");
    else if(request->method() == HTTP_HEAD)
      Serial.printf("HEAD");
    else if(request->method() == HTTP_OPTIONS)
      Serial.printf("OPTIONS");
    else
      Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if(request->contentLength()){
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
      AsyncWebHeader* h = request->getHeader(i);
      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for(i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if(p->isPost()){
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    request->send(404);
  });

  web_server.onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index)
      Serial.printf("UploadStart: %s\n", filename.c_str());
    Serial.printf("%s", (const char*)data);
    if(final)
      Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index+len);
  });
  web_server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if(!index)
      Serial.printf("BodyStart: %u\n", total);
    Serial.printf("%s", (const char*)data);
    if(index + len == total)
      Serial.printf("BodyEnd: %u\n", total);
  });  
} 
void Webserver::begin()	{web_server.begin();}
void Webserver::loop() 	{web_socket.cleanupClients();}

void Webserver::socket_send(const String & message) {
	if ((_socket_is_connected) && (_websocket_client!=NULL)) {
		if (_socket_trace) Serial.printf_P(PSTR("ws[%s][%u] send-message[%d]:\n%s\n"), _url, _id, message.length(), message.c_str());
		_websocket_client->text(message);
	}
	else
	{
		if (_socket_trace) {
			if (!_socket_is_connected)   {Serial.printf("Socket msg not sent, no socket connected\n");}
			if (_websocket_client==NULL) {Serial.printf("Socket msg not sent, no socket client available\n");}
		}
	}
}