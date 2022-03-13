#ifndef _MAIN_H
#define _MAIN_H 
    #include <Arduino.h>
    
    // static const char FOPATH_PLAYLISTEFF    [] PROGMEM = "/TEST/eff/";
    // static const char FSLSH                 [] PROGMEM = "/";

    static const char LPNAME_001[] PROGMEM = "Philippe";
    static const char LPNAME_002[] PROGMEM = "Catherine";
    static const char LPNAME_003[] PROGMEM = "Mamie Stella";
    static const char LPNAME_004[] PROGMEM = "Papi Roger";
    static const char LPNAME_005[] PROGMEM = "Mamie Christiane";
    static const char LPNAME_006[] PROGMEM = "Axel";
    static const char LPNAME_007[] PROGMEM = "Eliza";
    static const char LPNAME_008[] PROGMEM = "Zap";
    static const char LPNAME_009[] PROGMEM = "Mystic";
    static const char LPNAME_010[] PROGMEM = "Valentine";
    static const char LPNAME_011[] PROGMEM = "Pépita";
    static const char LPNAME_012[] PROGMEM = "Réglisse(Mon RéRé)";
    static const char LPNAME_013[] PROGMEM = "Crevette(Pot de cole)";
    static const char LPNAME_014[] PROGMEM = "Mimine(MouMoune)";
    static const char* const LPALLNAMES[] PROGMEM = {
    LPNAME_001, LPNAME_002, LPNAME_003, LPNAME_004, LPNAME_005, 
    LPNAME_006, LPNAME_007, LPNAME_008, LPNAME_009, LPNAME_010, 
    LPNAME_011, LPNAME_012, LPNAME_013, LPNAME_014
    };
    static const char* const LPALLNAMES_CAT[] PROGMEM = {
    LPNAME_008, LPNAME_009, LPNAME_010, 
    LPNAME_011, LPNAME_012, LPNAME_013
    };

#endif