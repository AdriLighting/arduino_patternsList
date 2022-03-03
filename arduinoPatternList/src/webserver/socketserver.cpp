#ifdef WEBSOCKETSERVEROK
#include "webserver/socketserver.h"

    using namespace std::placeholders;

    #if defined(ESP8266)

    #elif defined(ESP32)

    void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
      const uint8_t* src = (const uint8_t*) mem;
      DBG_OUTPUT_PORT.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
      for(uint32_t i = 0; i < len; i++) {
        if(i % cols == 0) {
          DBG_OUTPUT_PORT.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
        }
        DBG_OUTPUT_PORT.printf("%02X ", *src);
        src++;
      }
      DBG_OUTPUT_PORT.printf("\n");
    }

    #else
    #endif

socketserver::socketserver(int port, const String & espUi) : _socket(port, "", "arduino")
{
}
socketserver::socketserver(int port) : _socket(port, "", "arduino")
{
}

void socketserver::setup() {
  _socket.begin();
  _socket.onEvent(std::bind(&socketserver::webSocketEvent, this, _1, _2, _3, _4));
}

void socketserver::loop() {
  _socket.loop();
}

void socketserver::webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      #ifdef DEBUG
        Serial.printf_P(PSTR("[socketserver::webSocketEvent -> WStype_DISCONNECTED]\n\t[%u] Disconnected!\n"), num);     
      #endif
      _isConnected = false;
    break;
    case WStype_CONNECTED:
    {
      #ifdef DEBUG
        IPAddress ip = _socket.remoteIP(num);
        Serial.printf_P(PSTR("[socketserver::webSocketEvent -> WStype_CONNECTED]\n\t[%u] Connected from %d.%d.%d.%d url: %\n"), num, ip[0], ip[1], ip[2], ip[3], payload);    
      #endif 
      _num = num;
      _isConnected = true;
      if (_whenIsConnected != NULL) _whenIsConnected();
    }
    break;
    case WStype_TEXT:
      char buff[512];
      sprintf(buff, "%s", payload);
      _num = num;
      #ifdef DEBUG
        Serial.printf_P(PSTR("[socketserver::webSocketEvent -> WStype_TEXT]\n\t[%u] %s\n"),num, buff);    
      #endif 
      if (_parse != NULL) _parse(String(buff));
    break;
    case WStype_BIN:
      hexdump(payload, length);
    break;
    default: break;
  }

}
void socketserver::broadcastTXT(String msg) {
  if (!_isConnected) return;
  _socket.broadcastTXT(msg);
}

void socketserver::sendTXT(uint8_t num, String msg) {
  if (!_isConnected) return;
  #ifdef DEBUG
   Serial.printf_P(PSTR("[socketserver::sendTXT]\n\t%s\n"), msg.c_str());   
  #endif 
  _socket.sendTXT(num, msg);
}

boolean socketserver::isConnected() {
 return _isConnected;
}    
#endif


