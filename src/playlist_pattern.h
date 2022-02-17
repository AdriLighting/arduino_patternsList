#ifndef PLAYLIST_PATTERN_H
	#define PLAYLIST_PATTERN_H 

	#define PLAYLIST_MAXEFF	10
	#define PLAYLIST_MAX 	3
	// #define DEBUG


	#include <Arduino.h>
	#include <adri_timer.h>

	#include "patterns.h"

	#ifndef PLDFPATH
	static const char FOPATH_PLAYLIST       [] PROGMEM = "/playlist/playlist/";
	static const char FNPATH_LISTPLAYLLIST	[] PROGMEM = "/playlist/compoNames";
	#endif

// #include <vector>

// using namespace std;

/*
 *
	
	 	name 	= 	/compo_0.tx
	 				/compo_0.txt = sauvegarde des patterns

*/

	struct PLAYLISTFLAG {
		boolean isEmpty = false;
		int  arrayPos = 0;
	} ;


	class playlist_management
	{
		adri_timer * _item_loop_timer;

		int 	_item_max		= 0;
		int 	_item_pos 		= 0;
		int 	_item_Oldpos	= 0;

		bool 	_list_statu		= false; 
		String 	_list_name 		= "compoName_0";
		int 	_list_pos 		= 0;

		PLAYLISTFLAG Playlistflag[PLAYLIST_MAXEFF];
	public:

		playlist_management();
		~playlist_management(){delete this;};

		void emptyLists_check();
		void print();

		void jsonKey_object(JsonObject & root);
		void json_object(JsonObject & root);
		void item_jsonObject(JsonObject & root);
		void json_objectName(JsonObject & object);
		void json_list(const String & path);

		void 	item_print();
		void 	item_toArray(int s_pos, const String & s_patterName, const String & s_pattern_config);
		boolean item_restore();
		void 	item_remove(int remove) ;

		void 	item_patternName(String & ret);
		void 	item_patternName(int pos, String & ret);
		void 	item_patternConfig(int pos, String & ret);
		void 	item_patternConfig(String & ret);

		void 	item_Oldpos(int & ret);
		void 	item_pos(int & ret);
		void 	item_pos_set(int ret);
		boolean item_pos_set(String ret);
		void 	item_max(int & ret);

		void remainingTime(uint32_t & v);

		void item_loop(pattern_loop * ptr, mod_pattern_loop & mod, String & v1, boolean delayInMin);
		void item_loop_next(pattern_loop * ptr);
		void item_rnd();
		void item_rnd(String & ret);
		void item_next();
		void item_prev();

		void  	delay_reset();
		void  	list_dir(boolean dir);
		void  	list_statu(boolean & ret);
		boolean list_statu();
		void  	list_statu_set(boolean ret);
		void  	list_print();
		void  	list_pos(int pos);
		void  	list_pos_get(int & pos);
		void  	list_lbl(const String & pos);
		void  	list_name(String & ret);
		void	list_initialize();
		boolean	list_fromSpiff();
		boolean	list_toSpiff();
		void 	list_removeFromSpiff();	
		boolean	listAjson_toSpiff();

	private:
		void item_toTxt();


	};	

	playlist_management * playlist_instance();

	void playlist_statu(boolean & ret);
	void playlist_play();
	void playlist_stop();



	class playlist_item
	{


	public:
		String 	_filename;
		int 	_pos;
		String 	_pattern_name;
		String 	_pattern_config;

		playlist_item();
		~playlist_item(){delete this;};
		void print(int cnt);
		
	};	



	class playlist_list
	{
	public:
		// playlist_patternItem ** _item = nullptr;
		// String json[PLAYLIST_MAXEFF];
		String _name;
		String _lbl;
		String _pos;
		// playlist_item * _itemArray[PLAYLIST_MAXEFF];
		// int _itemMax = -1;


		playlist_list();
		~playlist_list(){delete this;};

		void setup();
		void print(int cnt);
		void printItems();
	};

	void playlist_pattern_print();
#endif