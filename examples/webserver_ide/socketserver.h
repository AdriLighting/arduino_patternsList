#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H


	#if defined(ESP8266)
		#include <Arduino.h>
		#include <ESP8266WiFi.h>
		#include <WiFiClient.h>
		#include <ESP8266WebServer.h>
		#include <ESP8266mDNS.h>
		#include <SPI.h>
		#ifdef FSOK
		  #if defined USE_SPIFFS
		    #include <FS.h>
		  #elif defined USE_LITTLEFS
		    #include <LittleFS.h>	
		  #endif
		#endif
		#include <WebSocketsServer.h>	

	#elif defined(ESP32)
		#include <Arduino.h>
		#include <WiFi.h>
		#include <WiFiClient.h>
		#include <WebServer.h>
		#include <ESPmDNS.h>
		#include <SPIFFS.h>
		#include <Update.h>

		#include <WiFi.h>
		#include <WiFiClientSecure.h>
		#include <WebSocketsServer.h>

	#else
	#endif

    // #include <adri_logger.h>
	// #include <adri_tools_v2.h>

	#define DBG_OUTPUT_PORT Serial


	typedef void (*socketserver_func)();  
	typedef void (*socketserver_strFunc)(String s);  
	class socketserver
	{
	public:
		socketserver_strFunc	_parse  			= NULL;	
		socketserver_func 		_whenIsConnected  	= NULL;	
		WebSocketsServer		_socket;		
		boolean 				_isConnected 		= false;
		uint8_t 				_num				= 0;

		socketserver(int port);
		socketserver(int port, const String & espUi);

		void parse(const String &);
		void setup();
		void loop();
		boolean isConnected();
		void broadcastTXT( String );
		void sendTXT(uint8_t, String);
		void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
	
	};


	
#endif // SERVER_H	
