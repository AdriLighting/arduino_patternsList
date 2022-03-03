#ifndef WEBSERVERREQUEST_H
#define WEBSERVERREQUEST_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include "constants.h"

class WebserverRequest {
public:

	WebserverRequest();
	~WebserverRequest();
	void parsingRequest(DynamicJsonDocument & doc, String & r, const String & upd);
};
class webserverRequest_reponse
{
	RA _action;
	String _value = "";
public:

	webserverRequest_reponse(){};
	~webserverRequest_reponse(){};
	void set_ra(RA action);
	void set_value(const String & v1);
	void make_reponse(DynamicJsonDocument & doc);
};
extern WebserverRequest _WebserverRequest;
#endif