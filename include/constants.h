#ifndef CONSTANTS_H
#define CONSTANTS_H
  #include <Arduino.h>

  #define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

  enum apPatternloppMod_t { 
    mpl_next,
    mpl_off
  };
  typedef enum : uint8_t {ST_BASE=0,ST_END, ST_IDX, ST_AB, ST_AB2, ST_MIC} apListSortType_t; 
  // typedef enum : uint8_t {PM_LB=0,PM_LBNAME,PM_PL,PM_LOOP, PM_LL, PM_LLI} PRINT_MOD; 
  typedef struct  APLISTLOOPFLAG_T{ 
    int       _pos          = 0;
    uint8_t   _posMin       = 0;
    uint16_t  _posMax       = 0; 
  } apListLoopFlag_t;
  typedef struct  APPLAYLISTFLAG_T{ 
      uint8_t cnt         ;
      boolean isSet       ;
      boolean isPlaying   ;
      uint8_t pos         ;
  } apPlaylistFlag_t;
  typedef struct  APBASICLISTFLAG_T{ 
      uint8_t cnt;
      uint8_t pos;
  } apBasiclistFlag_t;
  
  static const char APPT_DEBUGREGION_APAPI      [] PROGMEM = "apapi";
  static const char APPT_DEBUGREGION_PLAYLIST   [] PROGMEM = "playlist";
  static const char APPT_DEBUGREGION_PROGRAM    [] PROGMEM = "Program";
  static const char APPT_DEBUGREGION_WEBSERVER  [] PROGMEM = "webserver";
  static const char APPT_DEBUGREGION_QUEUE      [] PROGMEM = "queue";
  static const char APPT_DEBUGREGION_TASK       [] PROGMEM = "task";
  static const char APPT_DEBUGREGION_BASICLIST  [] PROGMEM = "basicList";


  static const char APPT_001   [] PROGMEM = "gv";
  static const char APPT_002   [] PROGMEM = "n";
  static const char APPT_003   [] PROGMEM = "v";
  static const char APPT_004   [] PROGMEM = "op";
  static const char APPT_005   [] PROGMEM = "set";
  static const char APPT_006   [] PROGMEM = "get";


  static const char FOPATH_PLAYLIST   [] PROGMEM = "/playlist/";
  static const char FNPREFIX_PLAYLIST [] PROGMEM = "playlist_";
  static const char FNEXT_PLAYLIST    [] PROGMEM = ".json";

  static const char APPT_SETTER_001[] PROGMEM = "APSET_ITEM";
  static const char APPT_SETTER_002[] PROGMEM = "APSET_ITEM_NEXT";
  static const char APPT_SETTER_003[] PROGMEM = "APSET_ITEM_PREV";
  static const char APPT_SETTER_004[] PROGMEM = "APSET_ITEM_RND";
  static const char APPT_SETTER_005[] PROGMEM = "APSET_PLAY_START";
  static const char APPT_SETTER_006[] PROGMEM = "APSET_PLAY_STOP";
  static const char APPT_SETTER_007[] PROGMEM = "APSET_PLAY_PAUSE";
  static const char APPT_SETTER_008[] PROGMEM = "APSET_PLAY_TOGGLE";
  static const char APPT_SETTER_009[] PROGMEM = "APSET_PLAY_DELAY";
  static const char APPT_SETTER_010[] PROGMEM = "APSET_PLAY_DELAYMIN";
  static const char APPT_SETTER_011[] PROGMEM = "APSET_PLAY_DELAYMINON";
  static const char APPT_SETTER_012[] PROGMEM = "APSET_PLAY_DELAYMINOFF";
  static const char APPT_SETTER_013[] PROGMEM = "APSET_PLAY_RND";
  static const char APPT_SETTER_014[] PROGMEM = "APSET_PLAY_PL";
  static const char APPT_SETTER_015[] PROGMEM = "APSET_PLAY_LB";
  static const char APPT_SETTER_016[] PROGMEM = "APSET_PLAY_LT";
  static const char APPT_SETTER_017[] PROGMEM = "APSET_LSET_PL";
  static const char APPT_SETTER_018[] PROGMEM = "APSET_LSET_PLNEXT";
  static const char APPT_SETTER_019[] PROGMEM = "APSET_LSET_PLPREV";
  static const char APPT_SETTER_020[] PROGMEM = "APSET_LSET_PLRND";
  static const char APPT_SETTER_021[] PROGMEM = "APSET_LSET_LB";
  static const char APPT_SETTER_022[] PROGMEM = "APSET_LSET_LBNEXT";
  static const char APPT_SETTER_023[] PROGMEM = "APSET_LSET_LBPREV";
  static const char APPT_SETTER_024[] PROGMEM = "APSET_LSET_LBRND";
  // static const char APPT_SETTER_025[] PROGMEM = "APSET_LGET_PL";
  static const char APPT_SETTER_026[] PROGMEM = "APSET_PLI_NEW";
  static const char APPT_SETTER_027[] PROGMEM = "APSET_PLI_REM";
  static const char APPT_SETTER_028[] PROGMEM = "APSET_PLI_REP";
  static const char APPT_SETTER_029[] PROGMEM = "APSET_PL_TOFS";
  // static const char APPT_SETTER_021[] PROGMEM = "APSET_LGET_PL";
  // static const char APPT_SETTER_022[] PROGMEM = "APSET_LGET_LB";
  static const char* const APPT_SETTER_ARRAY[] PROGMEM = {
  APPT_SETTER_001, APPT_SETTER_002, APPT_SETTER_003, APPT_SETTER_004, APPT_SETTER_005, 
  APPT_SETTER_006, APPT_SETTER_007, APPT_SETTER_008, APPT_SETTER_009, APPT_SETTER_010, 
  APPT_SETTER_011, APPT_SETTER_012, APPT_SETTER_013, APPT_SETTER_014, APPT_SETTER_015,
  APPT_SETTER_016, APPT_SETTER_017, APPT_SETTER_018, APPT_SETTER_019, APPT_SETTER_020, 
  APPT_SETTER_021, APPT_SETTER_022, APPT_SETTER_023, APPT_SETTER_024,
  APPT_SETTER_026, APPT_SETTER_027, APPT_SETTER_028, APPT_SETTER_029
  };
  typedef enum APSET_T {
    APSET_ITEM,      
    APSET_ITEM_NEXT,
    APSET_ITEM_PREV,
    APSET_ITEM_RND,     
    APSET_PLAY_START,
    APSET_PLAY_STOP,
    APSET_PLAY_PAUSE,    
    APSET_PLAY_TOGGLE,     
    APSET_PLAY_DELAY,
    APSET_PLAY_DELAYMIN,
    APSET_PLAY_DELAYMINON, 
    APSET_PLAY_DELAYMINOFF,
    APSET_PLAY_RND,       
    APSET_PLAY_PL,       
    APSET_PLAY_LB,       
    APSET_PLAY_LT,
    APSET_LSET_PL,
    APSET_LSET_PLNEXT,
    APSET_LSET_PLPREV,       
    APSET_LSET_PLRND,       
    APSET_LSET_LB,     
    APSET_LSET_LBNEXT,     
    APSET_LSET_LBPREV,     
    APSET_LSET_LBRND,
    APSET_PLI_NEW,
    APSET_PLI_REM,
    APSET_PLI_REP,
    APSET_PL_TOFS
  } apSetter_t;
  extern apSetter_t RAARR[];
  
  static const char APPT_REP_001[] PROGMEM = "pl";     // APSET_ITEM->loop, APSET_PLAY_PL->loop, list->list, list_pls->list
  static const char APPT_REP_002[] PROGMEM = "plt";    // APSET_ITEM->loop, APSET_PLAY_PL->loop, 
  static const char APPT_REP_003[] PROGMEM = "lb";     // APSET_ITEM->loop, APSET_PLAY_LB->loop, list->list, list_lbs->list, js_init->list, listc->list
  static const char APPT_REP_004[] PROGMEM = "lbt";    // APSET_ITEM->loop, APSET_PLAY_LB->loop
  static const char APPT_REP_005[] PROGMEM = "statu";  // APSET_PLAY->loop
  static const char APPT_REP_006[] PROGMEM = "plc";    // js_init->list, list_plsc->list, listc->list    
  static const char APPT_REP_007[] PROGMEM = "pld";    // APSET_LGET_PL->pld

  static const char APPT_REQ_001[] PROGMEM = "list_alllb";
  static const char APPT_REQ_002[] PROGMEM = "list_lb";
  static const char APPT_REQ_003[] PROGMEM = "list_allpl";
  static const char APPT_REQ_004[] PROGMEM = "list_pl";
  static const char APPT_REQ_005[] PROGMEM = "loop";
  static const char APPT_REQ_006[] PROGMEM = "list";         // DEMO WEBSERVER
  static const char APPT_REQ_007[] PROGMEM = "list_lbs";
  static const char APPT_REQ_008[] PROGMEM = "list_pls";
  static const char APPT_REQ_009[] PROGMEM = "list_ra";
  static const char APPT_REQ_010[] PROGMEM = "list_req";
  static const char APPT_REQ_011[] PROGMEM = "list_lbpl";    // DEMO WEBSERVER
  static const char APPT_REQ_012[] PROGMEM = "js_init";      // DEMO WEBSERVER
  static const char APPT_REQ_013[] PROGMEM = "list_plsc";

  static const char APPT_REQ_014[] PROGMEM = "listc";        // DEMO WEBSERVER

  static const char APPT_REQ_015[] PROGMEM = "loop_select";  // "get":[{"loop_select":["statu", "pl"]}]      
  static const char APPT_REQ_016[] PROGMEM = "list_pld";     // "get":[{"gv":{"n":"list_pld","v":"0"}}]

  static const char* const APPT_REQ_ARRAY[] PROGMEM = {
  APPT_REQ_001, APPT_REQ_002, APPT_REQ_003, APPT_REQ_004, APPT_REQ_005,
  APPT_REQ_006, APPT_REQ_007, APPT_REQ_008, APPT_REQ_009, APPT_REQ_010,
  APPT_REQ_011, APPT_REQ_012, APPT_REQ_013, APPT_REQ_014, APPT_REQ_015, 
  APPT_REQ_016
  };  
  // typedef enum _requette_type {
  //     APPT_REQ_LB,      
  //     APPT_REQ_ALLLB     
  // } RT;
  // extern RT RTARR[];

#endif // CONSTANTS_H


  /*

APPT_SETTER_001
APPT_SETTER_002
APPT_SETTER_003
APPT_SETTER_004
APPT_SETTER_005
APPT_SETTER_006
APPT_SETTER_007
APPT_SETTER_008
APPT_SETTER_009
APPT_SETTER_010
APPT_SETTER_011
APPT_SETTER_012
APPT_SETTER_013
APPT_SETTER_014
APPT_SETTER_015
APPT_SETTER_016
APPT_SETTER_017
APPT_SETTER_018
APPT_SETTER_019
APPT_SETTER_020
APPT_SETTER_021
APPT_SETTER_022
APPT_SETTER_023
APPT_SETTER_024
APPT_SETTER_025
APPT_SETTER_026
APPT_SETTER_027
APPT_SETTER_028
APPT_SETTER_029
APPT_SETTER_030
APPT_SETTER_031
APPT_SETTER_032
APPT_SETTER_033
APPT_SETTER_034
APPT_SETTER_035
APPT_SETTER_036
APPT_SETTER_037
APPT_SETTER_038
APPT_SETTER_039

APPT_REP_001
APPT_REP_002
APPT_REP_003
APPT_REP_004
APPT_REP_005
APPT_REP_006
APPT_REP_007
APPT_REP_008
APPT_REP_009
APPT_REP_010
APPT_REP_011
APPT_REP_012
APPT_REP_013
APPT_REP_014
APPT_REP_015
APPT_REP_016
APPT_REP_017
APPT_REP_018
APPT_REP_019
APPT_REP_020
APPT_REP_021
APPT_REP_022
APPT_REP_023
APPT_REP_024
APPT_REP_025
APPT_REP_026
APPT_REP_027
APPT_REP_028
APPT_REP_029
APPT_REP_030
APPT_REP_031
APPT_REP_032
APPT_REP_033
APPT_REP_034
APPT_REP_035
APPT_REP_036
APPT_REP_037
APPT_REP_038
APPT_REP_039
*/