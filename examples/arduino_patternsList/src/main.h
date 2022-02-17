#ifndef MAIN_H
#define MAIN_H 
	#include <Arduino.h>
	#include <littlefs_helper.h>

	#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

	
	// static const char FOPATH_PLAYLISTEFF    [] PROGMEM = "/TEST/eff/";
	// static const char FSLSH                 [] PROGMEM = "/";

	static const char LPNAME_001[] PROGMEM = "Philippe";
	static const char LPNAME_002[] PROGMEM = "Catherine";
	static const char LPNAME_003[] PROGMEM = "Axel";
	static const char LPNAME_004[] PROGMEM = "Eliza";
	static const char LPNAME_005[] PROGMEM = "Mimine";
	static const char* const LPALLNAMES[] PROGMEM = {
	LPNAME_001, LPNAME_002, LPNAME_003, LPNAME_004, LPNAME_005
	};

	void fs_restore_defaultPlaylist(boolean remove);
	void on_timeD(String & result);
	void deleteRecursive(fs::FS &fs, const String &path);

#endif