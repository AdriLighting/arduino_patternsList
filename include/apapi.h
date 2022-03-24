#ifndef APAPI_H
#define APAPI_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <LinkedList.h>
#include "constants.h"
#include "tools.h"

void AP_ApiItemList_setup();

typedef std::function<void(DynamicJsonDocument & reply)> _apapiItem_f;

class AP_ApiItem
{

public:
	const char 		* _id = "";
	_apapiItem_f 	_generateJson;

	AP_ApiItem(const char * const v1, _apapiItem_f v2);
	~AP_ApiItem();
};
class AP_ApiItemList
{
	LList<AP_ApiItem *> _list;
	
public:
	AP_ApiItemList();
	~AP_ApiItemList();
	void add (const char * const v1, _apapiItem_f v2);
	void parsingRequest(const String &, DynamicJsonDocument & );
};
extern AP_ApiItemList _apapiItemList;



class AP_Api {
public:

	AP_Api();
	~AP_Api();
	/**
	 * @brief				function to use to process a user request formulated with a query in json format
	 *
	 * @param[out]	doc   query
	 * @param[out]	r			reply json in String format
	 * @param[in]  	upd   parmaeter for send a reply query
	 */
	void parsingRequest(DynamicJsonDocument & doc, String & r, const String & upd);
};
class AP_ApiReply {
	RA 			_action;
	String 	_value = "";
public:
	AP_ApiReply(){};
	~AP_ApiReply(){};
	void set_ra(RA action);
	void set_value(const String & v1);
	void reply_generate(DynamicJsonDocument & doc);
};
extern AP_Api _AP_Api;



typedef enum _WSRM {WSURM_GETTER, WSURM_SETTER} WSURM;
typedef std::function<void(const String & v1, DynamicJsonDocument & doc)> _wsur_cb_f;

class AP_userApiItem {
	WSURM				_mod		= WSURM_GETTER;
	const char 	* _id		= "";
	_wsur_cb_f 	_reply	= nullptr;
public:
	AP_userApiItem();
	~AP_userApiItem(){};
	void set_mod(WSURM);
	void set_id(const char 	* const &);
	void set_replyCallback(_wsur_cb_f);
	void get_id(String &);
	void get_mod(WSURM &);
	boolean isEnabled();
	boolean cb(const String &, DynamicJsonDocument &);
};
class AP_userApi {
	AP_userApiItem	* _array			= nullptr;
  uint8_t					_arrayCnt			= 0;
  uint8_t					_arrayCntMax	= 0;	
public:
	AP_userApi();
	~AP_userApi(){};
	void initialize(uint8_t cmax);
	void set_request(uint8_t, const char 	* const &, _wsur_cb_f);
	void getter(const String &, DynamicJsonDocument &);
	void set_json_id(JsonArray &);
};
extern AP_userApi _AP_userApi;


#endif