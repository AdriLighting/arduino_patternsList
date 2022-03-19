#include "def.h"

#ifdef DEBUGSERIAL
#ifndef MENU_SERIAL_H
	#define MENU_SERIAL_H

	#include <Arduino.h>
	#include <LinkedList.h>

	typedef std::function<void()> sr_cb_v_f;
	typedef std::function<void(const String &, const String &)> sr_cb_ss_f;
	typedef enum sr_menumod { SRMM_SIMPLE, SRMM_KEY  } SR_MM;

	class Sr_item
	{
		const char*		_name 	= "";
		const char*		_key 		= "";
		sr_cb_v_f			_cb_v 	= nullptr;			
		sr_cb_ss_f 		_cb_ss 	= nullptr;		
		SR_MM 				_mod 		= SRMM_SIMPLE;	
	public:
		Sr_item();
		~Sr_item();
		void set(const char*, const char*, sr_cb_v_f, SR_MM v4 = SRMM_SIMPLE);
		void set(const char*, const char*, sr_cb_ss_f, SR_MM v4 = SRMM_SIMPLE);
		void print();
		void get_key(const char * &);
		void get_mod(SR_MM&);
		void get_callback();
		void get_callback(const String &, const String&);
	};
	class Sr_menu
	{
		LList<Sr_item *> _list;
	public:
		Sr_menu();
		~Sr_menu();
		Sr_item * add();
		void add(const char*, const char*, sr_cb_v_f, SR_MM v4 = SRMM_SIMPLE);
		void add(const char*, const char*, sr_cb_ss_f, SR_MM v4 = SRMM_SIMPLE);
		void serialRead();
		void serialReadString(const String &);
	};

#endif // MENU_SERIAL_H
#endif // MENU_SERIAL_H
