// #include "playlist_pattern.h"

#include <ArduinoJson.h>
#include <LittleFS.h>

#include "playlist_pattern.h"


namespace {


}
// String playlist_management_folder = "/effect_list/playlist/";
// #define COMPOSTIONS_FILENAME String("/effect_list/compoNames")

// bool compositions_debug = true; 
// #define DEBUG

playlist_item 		playlist_itemArray[PLAYLIST_MAXEFF];
playlist_list		* playlist_listArray[PLAYLIST_MAX];
playlist_management * playlist_manage;


namespace  {
	String literal_item(const String & name, const String & value) {
	   String x="<"+name+">"+value+"</"+name+">";
	   return x;
	}
	String literal_value(const String & name, const String & xml){
	   String open,close;
	   String value = "";
	   int start, end;
	   open="<"+name+">";
	   close="</"+name+">";
	   start=xml.indexOf(open);
	   if (start!=-1) {
	      start+=name.length()+2;
	      end=xml.indexOf(close);
	      value=xml.substring(start,end);
	   }
	   return value;
	}
	bool deserializeFile(DynamicJsonDocument& doc, const char* filepath){
	    if (!filepath || !*filepath)
	        return false;

	    File jfile = LittleFS.open(filepath, "r");
	    DeserializationError error;
	    if (jfile){
	        error = deserializeJson(doc, jfile);
	        jfile.close();
	    } else {
	        return false;
	    }

	    if (error) {
	        // ADRI_LOG(-1,2,2,"\nFile: failed to load json file: %s, deserializeJson error: %d", filepath, error.code());
	        // fsprintf("\t");
	        // Serial.println(error.code());
	        return false;
	    }
	    // ADRI_LOG(-1,2,2,"\nFile: %s ", filepath);
	    return true;
	}		
} // name


void playlist_statu(boolean & ret)		{playlist_instance()->list_statu(ret);}
void playlist_play() 					{playlist_instance()->list_statu_set(true);}
void playlist_stop() 					{playlist_instance()->list_statu_set(false);}

playlist_management * playlist_instance(){
	return playlist_manage;
}
playlist_management::playlist_management(){
	playlist_manage = this;
	_item_loop_timer = new adri_timer(10000,true);	
}


void playlist_management::print(){
	Serial.printf_P(PSTR("\n[playlist_management::print]\n\tname: %s\n\tpos: %d\n\titemMax: %d\n\titemPos: %d\n\titemStatu: %d\n"), 
		_list_name.c_str(),
		_list_pos,
		_item_max,
		_item_pos,
		_list_statu
	);	
}

void playlist_management::jsonKey_object(JsonObject & object){
        object[F("item_max")] 		= "0";
        object[F("item_pos")] 		= "1";
        object[F("list_statu")] 	= "2";
        object[F("list_name")] 		= "3";
        object[F("list_pos")] 		= "4";
}
void playlist_management::json_object(JsonObject & object){
        object[F("0")] = String(_item_max);
        object[F("1")] = String(_item_pos);
        object[F("2")] = String(_list_statu);
        object[F("3")] = _list_name;
        object[F("4")] = String(_list_pos);
}
void playlist_management::json_objectName(JsonObject & object){
        object[F("item_max")] 		= String(_item_max);
        object[F("item_pos")] 		= String(_item_pos);
        object[F("list_statu")] 	= String(_list_statu);
        object[F("list_name")] 		= _list_name;
        object[F("list_pos")] 		= String(_list_pos);
}

void playlist_management::item_jsonObject(JsonObject & object){
	for (int i = 0; i < _item_max; ++i)
	{
        object[String(i)] = playlist_itemArray[i]._pattern_name;

	}
}
void playlist_management::json_list(const String & path) {
    File file = LittleFS.open(path, "w");
    if (file) {
		DynamicJsonDocument json(3100);
		JsonObject root = json.to<JsonObject>();
		root[F("size")] = _item_max;
		JsonObject object = root.createNestedObject("listplaylist");
		playlist_instance()->item_jsonObject(object);
		serializeJson(json, file);
		file.close();
    }
}

void playlist_management::item_print(){
	Serial.print(F("\n[playlist_management::item_print]\n"));
	for (int i = 0; i < _item_max; ++i)
	{
		playlist_itemArray[i].print(i);
	}

}

void playlist_management::emptyLists_check(){
	int currentPos = _list_pos;
	for(int i = 0; i < PLAYLIST_MAX; ++i) {
		list_pos(i);
		item_restore();
		(_item_max > 0 ) ? Playlistflag[i].isEmpty = true : Playlistflag[i].isEmpty = false; 
	}	
	Serial.print(F("\n[playlist_management::emptyLists_check]\n"));
	for(int i = 0; i < PLAYLIST_MAX; ++i) {
		list_pos(i);
		// fsprintf( "[%d][%-15s = %5d]\n", i, _list_name.c_str(), Playlistflag[i].isEmpty);
	}
	list_pos(currentPos);
	item_restore();
}
// void playlist_management::list_print(JsonObject & root){
// 	Serial.print(F("\n[playlist_management::list_print]\n"));
// 	for (int i = 0; i < PLAYLIST_MAX; ++i) {
// 		if (!Playlistflag[i].isEmpty) {
// 			JsonObject object = root.createNestedObject(String(i));
// 			object[F("arrayPos")] 	= i;
// 			object[F("pos")] 		= playlist_listArray[i]._pos;
// 			object[F("name")] 		= playlist_listArray[i]._pos;
// 			object[F("lbl")] 		= playlist_listArray[i]._pos;
// 		}
// 	}

// }

void playlist_management::item_toTxt() {

	// String xml;
	String path = (String)FPSTR(FOPATH_PLAYLIST) + _list_name + ".json" ;

	#ifdef DEBUG
		Serial.printf_P(PSTR("\n[playlist_management::item_toTxt][START]\n"));	
		Serial.printf_P(PSTR("\t[writing : %s] result = "), path.c_str());	
	#endif

	File f=LittleFS.open(path,"w");
	if (!f) {
		
	#ifdef DEBUG
		Serial.printf_P(PSTR("FAIL\n"));
		Serial.printf_P(PSTR("[playlist_management::item_toTxt][FAIL]\n"));
	#endif

		return;
	}

  	DynamicJsonDocument doc(2048);
	JsonArray arr = doc.createNestedArray(F("items"));	
	for (int i=0; i < _item_max; i++) {
		JsonObject var = arr.createNestedObject();
		var[F("fn")]	= String(playlist_itemArray[i]._filename);
		var[F("pos")]	= String(playlist_itemArray[i]._pos);
		var[F("n")]    	= String(playlist_itemArray[i]._pattern_name);
		var[F("c")]		= String(playlist_itemArray[i]._pattern_config);
	}
	serializeJson(doc, f);
	f.close();

	#ifdef DEBUG
		Serial.printf_P(PSTR("SUCCES\n"));
		Serial.printf_P(PSTR("[playlist_management::item_toTxt][DO?E]\n"));	
	#endif
}
void playlist_management::item_toArray(int s_pos, const String & s_patterName, const String & s_pattern_config) {
	#ifdef DEBUG
		Serial.printf_P(PSTR("\n[playlist_management::item_toArray][START]\n"));	
	#endif

	bool newSav = false;
	if (s_pos > _item_max) {
		if ((_item_max) >= PLAYLIST_MAXEFF) {
			#ifdef DEBUG
				Serial.printf_P(PSTR("\t[LIMITE MAXIMUM ATTEINTE]\n")); 
				Serial.printf_P(PSTR("[playlist_management::item_toArray][FAIL]\n"));
			#endif
			return;
		}  	
		if (_item_max < 0) _item_max = 0;

		s_pos = _item_max;
		_item_max++;
		newSav=true;
		#ifdef DEBUG
			Serial.printf_P(PSTR("\t[new sav]\n")); 
		#endif
	}

	String val = "compo_" +  String(s_pos);

	int newNbr = s_pos;
	if (newSav){
		 bool find = true;
		 while (find) {
				for (int i=0; i < _item_max; i++) {
					 if (playlist_itemArray[i]._pos == newNbr) {
							newNbr = random(0,255);
							#ifdef DEBUG
								Serial.printf_P(PSTR("\t[new position : %d]\n"), newNbr);
							#endif
					 }  
					 else find = false;
				}
		 }
	}

	if (newSav) playlist_itemArray[s_pos]._filename 		= val;
	if (newSav) playlist_itemArray[s_pos]._pos 				= newNbr;
	if (newSav) playlist_itemArray[s_pos]._pattern_name 	= s_patterName;
				playlist_itemArray[s_pos]._pattern_config 	= s_pattern_config;
	
	item_toTxt(); 

	#ifdef DEBUG
		Serial.printf_P(PSTR("[playlist_management::item_toArray][DONE]\n"));
	#endif
}
boolean playlist_management::item_restore() {
	#ifdef DEBUG
		Serial.printf_P(PSTR("\n[playlist_management::item_restore][START]\n"));
	#endif

	_item_pos 		= 0;	
	_item_Oldpos 	= _item_pos;
	_item_max 		= 0;
	for (int i=0; i< PLAYLIST_MAXEFF; i++) {
		playlist_itemArray[i]._filename 		= "";
		playlist_itemArray[i]._pattern_config 	= "";
	}

	String path = (String)FPSTR(FOPATH_PLAYLIST) + _list_name + ".json";
	DynamicJsonDocument doc(2048);
	char buffer[80];
	sprintf(buffer, "%s", path.c_str());	
	if (!deserializeFile(doc, buffer)) {
		#ifdef DEBUG
			Serial.printf_P(PSTR("\t[Error reading : %s]\n"), path.c_str()); 				
			Serial.printf_P(PSTR("[playlist_management::item_restore][FAIL]\n"));	
		#endif
		return false;		
	}

	JsonArray arr = doc[F("items")].as<JsonArray>();
	for (size_t i = 0; i < arr.size(); i++) {
		JsonObject item = arr[i];
		if (i >= PLAYLIST_MAXEFF) {
			#ifdef DEBUG
			 	Serial.printf_P(PSTR("\t[LIMITE MAXIMUM ATTEINTE]\n"));	
			 #endif 	
			break;
		} else {

			playlist_itemArray[i]._filename 		= item[F("fn")].as<String>();
			playlist_itemArray[i]._pos           	= item[F("pos")].as<int>();
			playlist_itemArray[i]._pattern_name 	= item[F("n")].as<String>();
			playlist_itemArray[i]._pattern_config	= item[F("c")].as<String>();     

			_item_max++;   
		}  		
	}


	#ifdef DEBUG
		Serial.printf_P(PSTR("\t[item_restore nbr of items : %d]\n"), _item_max); 
		item_print();				
	#endif	

	_item_loop_timer->set_duration(millis());

	#ifdef DEBUG
		Serial.printf_P(PSTR("[playlist_management::item_restore][DONE]\n"));
	#endif
	return true;
}
void playlist_management::item_remove(int remove) {
	#ifdef DEBUG
		Serial.printf_P(PSTR("\n[playlist_management::item_remove][START]\n"));
	#endif

	String path = (String)FPSTR(FOPATH_PLAYLIST) + _list_name + ".json";

	File f=LittleFS.open(path,"w");
	if (!f) {
		#ifdef DEBUG
			Serial.printf_P(PSTR("\t[Error reading : %s]\n"), path.c_str()); 				
			Serial.printf_P(PSTR("[playlist_management::item_remove][FAIL]\n"));	
		#endif
		return;
	}
	int count =0;

  	DynamicJsonDocument doc(2048);
	JsonArray arr = doc.createNestedArray(F("items"));	
	for (int i=0; i < PLAYLIST_MAXEFF; i++) {
		if ((playlist_itemArray[i]._filename != "") && (playlist_itemArray[i]._filename != playlist_itemArray[remove]._filename )) {
			if (count == _item_max - 1) break;
			String val = "compo_" +  String(count);
			// xml+=literal_item("filename",          String(val));
			// xml+=literal_item("position",          String(playlist_itemArray[i]._pos));
			// xml+=literal_item("pattern_name",      String(playlist_itemArray[i]._pattern_name));
			// xml+=literal_item("pattern_config",    String(playlist_itemArray[i]._pattern_config));
			JsonObject var = arr.createNestedObject();
			var[F("fn")]	= String(val);
			var[F("pos")]	= String(playlist_itemArray[i]._pos);
			var[F("n")]    	= String(playlist_itemArray[i]._pattern_name);
			var[F("c")]		= String(playlist_itemArray[i]._pattern_config);			
			count++;
		} 
	}
	serializeJson(doc, f);
	f.close();

	item_restore();

	#ifdef DEBUG
		Serial.printf_P(PSTR("[playlist_management::item_remove][DONE]\n"));
	#endif

}

void playlist_management::list_name(String & ret) 			{ret 		= _list_name;}

void playlist_management::item_patternName(int pos, String & ret) 	{ret = playlist_itemArray[pos]._pattern_name;}
void playlist_management::item_patternName(String & ret) 			{ret = playlist_itemArray[_item_pos]._pattern_name;}

void playlist_management::item_patternConfig(int pos, String & ret) {ret = playlist_itemArray[pos]._pattern_config;}
void playlist_management::item_patternConfig(String & ret) 			{ret = playlist_itemArray[_item_pos]._pattern_config;}

void playlist_management::item_Oldpos(int & ret){ret = _item_Oldpos;}
void playlist_management::item_pos(int & ret)	{ret = _item_pos;}
void playlist_management::item_pos_set(int ret)	{_item_Oldpos = _item_pos; _item_pos = ret;_item_loop_timer->set_duration(millis());}
boolean playlist_management::item_pos_set(String ret){
	int pos = -1;
	for(int i = 0; i < _item_max; ++i) {if (ret == playlist_itemArray[i]._pattern_name ) {pos = i; break;}}	
	if (pos == -1) return false ;
	_item_Oldpos 	= _item_pos;
	_item_pos 		= pos;
	_item_loop_timer->set_duration(millis());
	return true;
}
void playlist_management::item_max(int & ret) {ret = _item_max;}
void playlist_management::remainingTime(uint32_t & v){_item_loop_timer->get_remainingTime(v);}
void playlist_management::item_loop(pattern_loop * ptr, mod_pattern_loop & mod, String & v1, boolean delayInMin){

	if ( !ptr->isPlaying() ) {
		return;
	} 
	uint32_t delay;
	ptr->delayCurrent_get(delay);
	if (_item_loop_timer->loop(delay*(delayInMin ? 60000 : 1000))) {
		String name;
		// #ifdef PATHEAPSTATS
		// 	heap_pass++;

		// 	heap_lastPos = _pos;
		// #endif

		item_next();

		// #ifdef PATHEAPSTATS
		// 	heap_newPos = _pos;
		// #endif

		item_patternName(name);

		// // fsprintf("\n[pattern_loop::loop] : next item set to : [%d] - %s\n", _pos, name.c_str());


		mod		= mpl_next;
		v1 		= name;		
	}
}
void playlist_management::item_loop_next(pattern_loop * ptr){
	if (ptr->isRndPlaying()) 	item_rnd();
	else 						item_next();
}
void playlist_management::item_next(){
	_item_Oldpos = _item_pos;
	_item_pos++;
	if (_item_pos >= _item_max) _item_pos = 0;
}
void playlist_management::item_prev(){
	_item_Oldpos = _item_pos;
	_item_pos--;
	if (_item_pos < 0) _item_pos = _item_max-1;
}
void playlist_management::item_rnd(){
	_item_Oldpos = _item_pos;
	_item_pos = random(0, _item_max);
	if (_item_pos >= _item_max) _item_pos = 0;
}
void playlist_management::item_rnd(String & ret){
	_item_Oldpos = _item_pos;
	_item_pos = random(0, _item_max);
	if (_item_pos >= _item_max) _item_pos = 0;
	item_patternName(ret);
	_item_loop_timer->set_duration(millis());
}

void playlist_management::list_statu_set(boolean ret) 	{_list_statu = ret;_item_loop_timer->set_duration(millis());}
void playlist_management::list_statu(boolean & ret) 	{ret = _list_statu;}
boolean playlist_management::list_statu() 				{return _list_statu;}
void playlist_management::list_pos_get(int & ret) 		{ret = _list_pos;}

void playlist_management::list_print(){
	Serial.print(F("\n[playlist_management::list_print][START]\n"));
	for (int i = 0; i < PLAYLIST_MAX; ++i) playlist_listArray[i]->print(i);
	Serial.print(F("[playlist_management::list_print][DONE]\n"));	
}

void playlist_management::list_dir(boolean dir) {
	if (dir) {
		_list_pos++;
		if (_list_pos>=PLAYLIST_MAX)_list_pos=0;
	} else {
		_list_pos--;
		if (_list_pos<0)_list_pos=PLAYLIST_MAX-1;
	}
	_list_name 	= playlist_listArray[_list_pos]->_name;
}

void playlist_management::list_pos(int pos){
	_list_pos 	= pos;
	_list_name 	= playlist_listArray[pos]->_name;
}
void playlist_management::list_lbl(const String & lbl){
 	playlist_listArray[_list_pos]->_lbl = lbl;
}
void playlist_management::list_initialize() {
	#ifdef DEBUG
		Serial.printf_P(PSTR("\n[playlist_management::list_initialize][START]\n"));
	#endif
	// String path = (String)FPSTR(FNPATH_LISTPLAYLLIST) + ".txt";
	// File f = LittleFS.open(path,"r");
	// if (!f){ 
	// 	f = LittleFS.open(path,"w");
	// 	if (f){
	// 		#ifdef DEBUG
	// 			Serial.printf_P(PSTR("\t[writing default filename : %s]\n"), path.c_str());
	// 		#endif
	// 		String line;
	// 		for (int i=0; i < PLAYLIST_MAX; i++) {
	// 			String cName 	= literal_item("name", 	"compoName_" + String(i));
	// 			String clbl 	= literal_item("lbl", 	"compoName_" + String(i));
	// 			String cPos 	= literal_item("pos", 	String(i));
	// 			line += cName ;
	// 			line += cPos ;
	// 			line += clbl + "\n";
	// 		} 
	// 		f.print(line);
	// 		f.close();
	// 	} else {
	// 		#ifdef DEBUG
	// 			Serial.printf_P(PSTR("\t[ERROR writing default filename : %s]\n"), path.c_str());	
	// 		#endif
	// 	}
	// } else {
	// 	#ifdef DEBUG
	// 		Serial.printf_P(PSTR("\t[filename is already creted : %s]\n"), path.c_str());	
	// 	#endif
	// }

	String path = (String)FPSTR(FNPATH_LISTPLAYLLIST)  + ".json";
	File f = LittleFS.open(path,"r");
	if (!f){ 
		f = LittleFS.open(path,"w");
		if (f){
			Serial.printf_P(PSTR("\t[writing default filename : %s]\n"), path.c_str());
			DynamicJsonDocument json(2048);
			JsonObject root = json.to<JsonObject>();
			JsonObject object_2;
			JsonObject object = root.createNestedObject("componames");
			object[F("cnt")] = PLAYLIST_MAX;
			for (int i=0; i < PLAYLIST_MAX; i++) {
				object_2 = object.createNestedObject("compo_" + String(i));
				object_2[F("name")] 	= "compoName_" + String(i);
				object_2[F("lbl")] 		= "compoName_" + String(i);
				object_2[F("arrayPos")] = i;
			} 
			serializeJson(json, f);
			f.close();			
		} else {
			#ifdef DEBUG
				Serial.printf_P(PSTR("\t[ERROR writing default filename : %s]\n"), path.c_str());	
			#endif
		}
	} else {
		#ifdef DEBUG
			Serial.printf_P(PSTR("\t[filename is already creted : %s]\n"), path.c_str());	
		#endif
	}

	#ifdef DEBUG
		Serial.printf_P(PSTR("[playlist_management::list_initialize][DONE]\n"));
	#endif	   
}

boolean playlist_management::listAjson_toSpiff(){ 
	#ifdef DEBUG
		Serial.printf_P(PSTR("\n[playlist_management::listAjson_toSpiff][START]\n"));
	#endif
	String path = (String)FPSTR(FNPATH_LISTPLAYLLIST) + ".json";

		File f = LittleFS.open(path,"w");
		if (f){
			#ifdef DEBUG
				Serial.printf_P(PSTR("\t[writing filename : %s]\n"), path.c_str());
			#endif
			DynamicJsonDocument json(2048);
			JsonObject root = json.to<JsonObject>();
			JsonObject object_2;
			JsonObject object = root.createNestedObject("componames");
			object[F("cnt")] = PLAYLIST_MAX;
			for (int i=0; i < PLAYLIST_MAX; i++) {
				object_2 = object.createNestedObject("compo_" + String(i));
				object_2[F("name")] 	= playlist_listArray[i]->_name;
				object_2[F("lbl")] 		= playlist_listArray[i]->_lbl;
				object_2[F("arrayPos")] = playlist_listArray[i]->_pos;
			} 
			serializeJson(json, f);
			f.close();
			return true;			
		} else {
			#ifdef DEBUG
				Serial.printf_P(PSTR("\t[ERROR writing default filename : %s]\n"), path.c_str());	
			#endif
		}

	#ifdef DEBUG
		Serial.printf_P(PSTR("[playlist_management::listAjson_toSpiff][DONE]\n"));
	#endif
	return false;	
}

void playlist_management::list_removeFromSpiff(){
	LittleFS.remove((String)FPSTR(FNPATH_LISTPLAYLLIST) + ".txt");
}
boolean playlist_management::list_fromSpiff(){
	#ifdef DEBUG
		Serial.printf_P(PSTR("\n[playlist_management::list_fromSpiff][START]\n"));
	#endif

	list_initialize();

	String path = (String)FPSTR(FNPATH_LISTPLAYLLIST) + ".json";
	DynamicJsonDocument doc(2048);
	char buffer[80];
	sprintf(buffer, "%s", path.c_str());	
	if (!deserializeFile(doc, buffer)) {
		#ifdef DEBUG
			Serial.printf_P(PSTR("\t[Error reading : %s]\n"), path.c_str()); 				
		#endif
		return false;		
	}

	uint8_t cnt = doc[F("componames")][F("cnt")].as<uint8_t>();
	for (int i=0; i < cnt; i++) {
		playlist_listArray[i] = new playlist_list();
		String key = "compo_" + String(i);
		playlist_listArray[i]->_name 	= doc[F("componames")][key][F("name")].as<String>();
		playlist_listArray[i]->_lbl 	= doc[F("componames")][key][F("lbl")].as<String>();
		playlist_listArray[i]->_pos 	= doc[F("componames")][key][F("arrayPos")].as<int>();

	} 			

	#ifdef DEBUG
		Serial.printf_P(PSTR("[playlist_management::list_fromSpiff][DONE]\n"));
	#endif		
	return true;
}

boolean playlist_management::list_toSpiff(){ 

	return listAjson_toSpiff();

}




void playlist_management::delay_reset(){_item_loop_timer->set_duration(millis());}


playlist_item::playlist_item(){
}

void playlist_item::print(int cnt){
	Serial.printf("\t[%3d] [pos: %5d] [name: %-15s] [pattern: %-15s] [param: %-15s]\n", 
		cnt,
		_pos,
		_filename.c_str(),
		_pattern_name.c_str(),
		_pattern_config.c_str()
	);
}
// void playlist_patternItem_initialize(playlist_patternItem ** array){
// 	for (int i = 0; i < PLAYLIST_MAXEFF; ++i) array[i] = new playlist_patternItem();
// }
// 






playlist_list::playlist_list(){}

void playlist_list::setup(){

}
void playlist_list::print(int cnt){
	Serial.printf_P(PSTR("[%3d] [pos: %5s] [name: %-15s] [lbl: %-15s]\n"), 
		cnt,
		_pos.c_str(),
		_name.c_str(),
		_lbl.c_str()
	);
}
void playlist_list::printItems(){
	// for (int i = 0; i < PLAYLIST_MAXEFF; ++i) _item[i]->print(i);
}
