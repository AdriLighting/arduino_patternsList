#ifndef WEBSERVER_H_
#define WEBSERVER_H_

#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "TaskScheduler.h"

static const char WSTP_001 [] PROGMEM = "text/json";
static const char WSTP_002 [] PROGMEM = "application/json";
static const char WSTP_003 [] PROGMEM = "text/plain";
static const char WSTP_004 [] PROGMEM = "FS INIT ERROR";
static const char WSTP_005 [] PROGMEM = "FileNotFound";
static const char* const WSTP_ARR[] PROGMEM = {
  WSTP_001,           WSTP_002,           WSTP_003, 
  WSTP_004,           WSTP_005
};
typedef enum : uint8_t {
  WSTE_TXTJSON=0,    WSTE_APPJSON,      WSTE_TXTPLAIN,
  WSTE_FSINITERROR,  WSTE_FILENOTFONUD
} ENUM_WSTP;
typedef enum : uint8_t {
  WSRM_FROMDEF=0,    WSRM_FROMCALLBACK
} ENUM_WSRM;

typedef std::function<void(AsyncWebServerRequest *request, uint8_t pos)> _webServerRequest_f;

typedef struct  WSREP_FLAG{ 
    ENUM_WSTP _cType;
    ENUM_WSRM _rType ;
} WSREP_flag;

class Webserver_request
{
  
  const char            * _name = "";
  WebRequestMethod      _method;
  WSREP_flag            _repFlag;
public:
  _webServerRequest_f   _callback = nullptr;

  Webserver_request( );
  ~Webserver_request();
  void set_callback(_webServerRequest_f v1);
  void set_request(const char * const & , WebRequestMethod, ENUM_WSTP);
  void set_request(const char * const & , WebRequestMethod, ENUM_WSTP, _webServerRequest_f);
  void set_rType(ENUM_WSRM);

  void get_name(const char * &);
  void get_method(WebRequestMethod &);
  void get_contentType(const char * &);
  void get_rType(ENUM_WSRM &);
};

class Webserver {
  Webserver_request       * _requestArray       = nullptr;
  uint8_t                 _requestArrayCnt      = 0;
  uint8_t                 _requestArrayCntMax   = 0;

  boolean                 _httpCallback             = false;
  boolean                 _httpCallbackPos          = false;
  AsyncWebServerRequest   * _httpCallbackRequest    = NULL;
  
  AsyncWebSocketClient    * _socketClient   = NULL;
  AsyncWebSocket          * _socketServer   = NULL;
  boolean                 _socketTrace      = true;
  boolean                 _socketIsConnected = false;

  typedef std::function<void(const String & v1)> callback_function_t;
  callback_function_t     _socketCallback     = nullptr;
  String                  _socketReceivedMsg  = "";
  boolean                 _socketReceived     = false;

  Task * _task_httpCallback = nullptr;
public:
  boolean                 _httpTrace = true;


  Webserver();
  ~Webserver();

  void request_array(uint8_t v1);
  Webserver_request * get_requestArray(uint8_t p);
  uint8_t request_new(const char * const & , WebRequestMethod, ENUM_WSTP);
  uint8_t request_new(const char * const & , WebRequestMethod, ENUM_WSTP, _webServerRequest_f);

  void socket_send(const String & message);

  void set_socketCallback(callback_function_t v1);
  void set_socketIsConnected(boolean v1);
  void set_socketClient(AsyncWebSocketClient * v1);
  void set_socketServer(AsyncWebSocket * v1);
  void set_socketReceivedMsg(const String & v1);
  void set_socketReceived(boolean v1);

  void set_httpCallback(boolean v1);


  void begin();
  void setup();
  void handle();  
  void socketHandle();  
  void httpHandle();  
};
extern Webserver _Webserver;
#endif