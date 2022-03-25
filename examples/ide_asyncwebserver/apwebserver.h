#ifndef _APWEBSERVER_H
#define _APWEBSERVER_H

#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <arduinoPatternList.h>

#include "TaskScheduler.h"
#include "queue.h"

// typedef enum {
//   HTTP_GET     = 0b00000001,
//   HTTP_POST    = 0b00000010,
//   HTTP_DELETE  = 0b00000100,
//   HTTP_PUT     = 0b00001000,
//   HTTP_PATCH   = 0b00010000,
//   HTTP_HEAD    = 0b00100000,
//   HTTP_OPTIONS = 0b01000000,
//   HTTP_ANY     = 0b01111111,
// } WebRequestMethod;

static const char WRMTP_001 [] PROGMEM = "HTTP_GET";
static const char WRMTP_002 [] PROGMEM = "HTTP_POST";
static const char WRMTP_003 [] PROGMEM = "HTTP_DELETE";
static const char WRMTP_004 [] PROGMEM = "HTTP_PUT";
static const char WRMTP_005 [] PROGMEM = "HTTP_PATCH";
static const char WRMTP_006 [] PROGMEM = "HTTP_HEAD";
static const char WRMTP_007 [] PROGMEM = "HTTP_OPTIONS";
static const char WRMTP_008 [] PROGMEM = "HTTP_ANY";
static const char* const WRMTP_ARR[] PROGMEM = {
  WRMTP_001, WRMTP_002, WRMTP_003, WRMTP_004,
  WRMTP_005, WRMTP_006, WRMTP_007, WRMTP_008
};

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

typedef struct  WSAPPT_REP_FLAG{ 
    ENUM_WSTP _cType;
    ENUM_WSRM _rType ;
} WSAPPT_REP_flag;

typedef std::function<void(AsyncWebServerRequest *request, uint8_t pos, const String & data)> _webServerRequest_f;
typedef std::function<void(const String & v1)> callback_function_t;

class Webserver_request
{
  
  const char            * _name = "";
  WebRequestMethod      _method;
  WSAPPT_REP_flag            _repFlag;
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

  String                  _httpCallbackData       = "";
  AsyncWebServerRequest   * _httpCallbackRequest    = NULL;
  
  AsyncWebSocketClient    * _socketClient     = NULL;
  AsyncWebSocket          * _socketServer     = NULL;
  boolean                 _socketTrace        = true;
  boolean                 _socketIsConnected  = false;
  callback_function_t     _socketCallback     = nullptr;
  String                  _socketCallbackData  = "";

  Task * _task_httpCallback = nullptr;
  
public:
  boolean _httpTrace = true;


  Webserver();
  ~Webserver();

  void request_array(uint8_t v1);
  Webserver_request * get_requestArray(uint8_t p);
  uint8_t request_new(const char * const & , WebRequestMethod, ENUM_WSTP);
  uint8_t request_new(const char * const & , WebRequestMethod, ENUM_WSTP, _webServerRequest_f);

  void socket_send(const String & message);
  void http_send(AsyncWebServerRequest *, const int &, ENUM_WSTP, const String &);

  void set_socketCallback(callback_function_t v1);
  void set_socketIsConnected(boolean v1);
  void set_socketClient(AsyncWebSocketClient * v1);
  void set_socketServer(AsyncWebSocket * v1);
  void set_socketCallbackData(const String & v1);
  void socketHandle();  


  void begin();
  void setup();

  void handle();  
  void httpHandle();

};
extern Webserver _Webserver;


class socketQueue {
protected:  
    boolean         _executeQuee  = false;

    uint32_t        _last_item    = 0;
    uint32_t        _timer_handle = 250;
    uint32_t        _timer_item   = 250;

    Task*           _task         = nullptr;
    uint32_t        _task_delay   = 500;
public:
    QueueItemList   * _list;

    socketQueue();
    ~socketQueue();
    virtual void receive(const String &) {};
    virtual void receive(DynamicJsonDocument &) {};
    boolean receiveToQueue(const String &);
    void handle();
};

class socketQueueReply : public socketQueue {
public:
    socketQueueReply(Task *);
    ~socketQueueReply();
    void receive(const String &) override;
};
class socketQueueSetter : public socketQueue {
public:
    socketQueueSetter(Task *);
    ~socketQueueSetter();
    void receive(DynamicJsonDocument & d) override ;
};

#endif