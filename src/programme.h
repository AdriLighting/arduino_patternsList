#ifndef PROGRAMME_PATTERN_H
	#define PROGRAMME_PATTERN_H

	#include <Arduino.h>
	#include <ArduinoJson.h>

	#include "patterns.h"
	#include "playlist_pattern.h"



	class programme_loop {
		pattern_loop 			* _patterns;
		pattern_list 			* _patterns_list;
		playlist_management		* _playlist;
	public:
		programme_loop();
		~programme_loop(){delete this;};

		
		void loop(mod_pattern_loop & mod, String & v1, boolean & playlist, boolean delayInMin = false);
		void remainingTime(uint32_t & v);
		void aytoDelayReset();
		void effectSet(boolean dir, String & name);
		void effectCurrentGet(String & ret);
		void item_getName(String & value, uint8_t pos);
		
		mod_pattern_loop statuDefault();
		mod_pattern_loop statuNext();
		boolean isPlaying 		();
		boolean isRndPlaying 	();	
		void delayCurrent_get	(uint32_t & value);
		void delayManual_get	(boolean & value);
		void delayCurrent_set	(int pos);
		void delayManual_set	(boolean pos);

		void play_set		(boolean pos);
		void autoPlay		();
		void playRnd_set	(boolean pos);
		void autoPlayRnd	();

		void effectRnd(String & ret);

		void patternList_initialize(uint8_t maxCnt);
		void patternList_item_add(const String &value);
		void listReSort(int pos, const String & value);	
		void patternList_randomEffect(String & ret);

		void patternList_item_getName(String & value, uint8_t pos);
		boolean patternList_itemGetPosByName(const String &search, int & result);

		boolean pattern_list_changeEffect(const String &search, int & result);
		boolean pattern_list_changeEffect(boolean dir, String & ret);
		void pattern_list_setEffect(boolean dir, uint8_t & ret);

		void patternList_print();
		void patternList_jsonObject(JsonObject & root);

		void patternList_cnt(uint8_t & value);


		void patternLoop_pos_set(uint8_t pos);
		void patternLoop_pos_get(uint8_t & pos);

		void patternLoop_delay_reset		();
		void patternLoop_delay_get 			(uint32_t & value);
		void patternLoop_delay_set 			(int pos);

		void patternLoop_posMin_get(uint8_t & value);
		void patternLoop_posMax_get(uint8_t & value);




	};

	programme_loop * programmeLoop_instance();
#endif


/*

		unsigned long 		_timer_cyclePattern_lastChange;
		unsigned long 		_timer_cyclePatternFunc_lastChange;

		unsigned long   	_timer_cyclePalette_lastChange;

		unsigned long   	_timer_cycleColor_lastChange;

		unsigned long   	_timer_cycleHue_lastChange;

		unsigned long   	_timer_cycleReverse_lastChange;

*/	