#ifndef WEBSERVER_H_
#define WEBSERVER_H_

#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

class Webserver {
  boolean _socket_trace = true;

  AsyncEventSourceClient  * _event_client      = NULL;
  boolean                 _event_is_connected  = false;

  AsyncWebSocketClient    * _websocket_client  = NULL;
  boolean                 _socket_is_connected = false;


public:
  const char  * _url  = "";
  uint32_t    _id     = 0;

	Webserver();
	~Webserver();

  void socket_send(const String & message);

  void set_socketIsConnected(boolean v1);
  void set_socketClient(AsyncWebSocketClient * v1);

  void begin();
  void setup();
  void loop();	
};
extern Webserver _Webserver;
#endif