#ifdef DEBUGSERIAL
#ifndef MENU_SERIAL_H
	#define MENU_SERIAL_H

	#include <Arduino.h>

	typedef void (*at_srFunc)(const String &cmd, const String &value);  
	class serialReadItem
	{

	public:
			const char*     _name;
			const char*		_key;
			at_srFunc 		_function;	

		serialReadItem(){}
		void item_add(
			const char* name,
			const char*	key,
			at_srFunc 	f		);

		~serialReadItem(){};
		
	};
	class SerialRead
	{
			int _cmd_1_cnt = 0;
			int _cmd_2_cnt = 0;
			serialReadItem * _cmd_1_Array = nullptr; // simpl letter 	- split
			serialReadItem * _cmd_2_Array = nullptr; // ! 			- split
			at_srFunc 	_cmd_3 		= NULL;		
			char* 		_cmd_3_sep  = (char *)"";		
			String		_cmd_3_desc = "";		
			at_srFunc 	_cmd_4 		= NULL;		
			char*		_cmd_4_sep  = (char *)"";		
			String 		_cmd_4_desc = "";		
	public:

		SerialRead();
		~SerialRead(){};

		int splitText(const String & A_readString, const char* sep, String & cmd, String & value) ;
		void cmd_array(int pos, int cnt);

		void cmd_item_add (int pos, const char* name, const char* key, at_srFunc f);
		void cmd_3(char* sep, const String & desc, at_srFunc f);
		void cmd_4(char* sep, const String & desc, at_srFunc f);

		void read(String a);
		void loop();
		void menu();
	};
	SerialRead * SerialReadPtr_get();
	
#endif // MENU_SERIAL_H
#endif // MENU_SERIAL_H
