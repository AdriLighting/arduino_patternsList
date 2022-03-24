#ifndef CONSTANTS_H
#define CONSTANTS_H
  #include <Arduino.h>

  #define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

  enum mod_pattern_loop { 
    mpl_next,
    mpl_off
  };
  typedef enum : uint8_t {ST_BASE=0,ST_END, ST_IDX, ST_AB, ST_AB2, ST_MIC} SORT_TYPE; 
  typedef enum : uint8_t {PM_LB=0,PM_LBNAME,PM_PL,PM_LOOP, PM_LL, PM_LLI} PRINT_MOD; 
  typedef struct  LL_FLAG{ 
    int       _pos          = 0;
    uint8_t   _posMin       = 0;
    uint16_t  _posMax       = 0; 
  } LL_flag;
  typedef struct  PL_FLAG{ 
      uint8_t cnt         ;
      boolean isSet       ;
      boolean isPlaying   ;
      uint8_t pos         ;
  } PL_flag;
  typedef struct  LB_FLAG{ 
      uint8_t cnt;
      uint8_t pos;
  } LB_flag;
  
  static const char DPTT_APAPI      [] PROGMEM = "apapi";
  static const char DPTT_PLAYLIST   [] PROGMEM = "playlist";
  static const char DPTT_PROGRAM    [] PROGMEM = "Program";
  static const char DPTT_WEBSERVER  [] PROGMEM = "webserver";
  static const char DPTT_QUEUE      [] PROGMEM = "queue";
  static const char DPTT_TASK       [] PROGMEM = "task";


  static const char APPTT_001   [] PROGMEM = "gv";
  static const char APPTT_002   [] PROGMEM = "n";
  static const char APPTT_003   [] PROGMEM = "v";
  static const char APPTT_004   [] PROGMEM = "op";
  static const char APPTT_005   [] PROGMEM = "set";
  static const char APPTT_006   [] PROGMEM = "get";


  static const char FOPATH_PLAYLIST   [] PROGMEM = "/playlist/";
  static const char FNPREFIX_PLAYLIST [] PROGMEM = "playlist_";
  static const char FNEXT_PLAYLIST    [] PROGMEM = ".json";

  static const char RANAME_001[] PROGMEM = "RA_ITEM";
  static const char RANAME_002[] PROGMEM = "RA_ITEM_NEXT";
  static const char RANAME_003[] PROGMEM = "RA_ITEM_PREV";
  static const char RANAME_004[] PROGMEM = "RA_ITEM_RND";
  static const char RANAME_005[] PROGMEM = "RA_PLAY_START";
  static const char RANAME_006[] PROGMEM = "RA_PLAY_STOP";
  static const char RANAME_007[] PROGMEM = "RA_PLAY_PAUSE";
  static const char RANAME_008[] PROGMEM = "RA_PLAY_TOGGLE";
  static const char RANAME_009[] PROGMEM = "RA_PLAY_DELAY";
  static const char RANAME_010[] PROGMEM = "RA_PLAY_DELAYMIN";
  static const char RANAME_011[] PROGMEM = "RA_PLAY_DELAYMINON";
  static const char RANAME_012[] PROGMEM = "RA_PLAY_DELAYMINOFF";
  static const char RANAME_013[] PROGMEM = "RA_PLAY_RND";
  static const char RANAME_014[] PROGMEM = "RA_PLAY_PL";
  static const char RANAME_015[] PROGMEM = "RA_PLAY_LB";
  static const char RANAME_016[] PROGMEM = "RA_PLAY_LT";
  static const char RANAME_017[] PROGMEM = "RA_LSET_PL";
  static const char RANAME_018[] PROGMEM = "RA_LSET_PLNEXT";
  static const char RANAME_019[] PROGMEM = "RA_LSET_PLPREV";
  static const char RANAME_020[] PROGMEM = "RA_LSET_PLRND";
  static const char RANAME_021[] PROGMEM = "RA_LSET_LB";
  static const char RANAME_022[] PROGMEM = "RA_LSET_LBNEXT";
  static const char RANAME_023[] PROGMEM = "RA_LSET_LBPREV";
  static const char RANAME_024[] PROGMEM = "RA_LSET_LBRND";
  // static const char RANAME_025[] PROGMEM = "RA_LGET_PL";
  static const char RANAME_026[] PROGMEM = "RA_PLI_NEW";
  static const char RANAME_027[] PROGMEM = "RA_PLI_REM";
  static const char RANAME_028[] PROGMEM = "RA_PLI_REP";
  static const char RANAME_029[] PROGMEM = "RA_PL_TOFS";
  // static const char RANAME_021[] PROGMEM = "RA_LGET_PL";
  // static const char RANAME_022[] PROGMEM = "RA_LGET_LB";
  static const char* const RAALLNAMES[] PROGMEM = {
  RANAME_001, RANAME_002, RANAME_003, RANAME_004, RANAME_005, 
  RANAME_006, RANAME_007, RANAME_008, RANAME_009, RANAME_010, 
  RANAME_011, RANAME_012, RANAME_013, RANAME_014, RANAME_015,
  RANAME_016, RANAME_017, RANAME_018, RANAME_019, RANAME_020, 
  RANAME_021, RANAME_022, RANAME_023, RANAME_024,
  RANAME_026, RANAME_027, RANAME_028, RANAME_029
  };
  typedef enum _remote_action {
    RA_ITEM,      
    RA_ITEM_NEXT,
    RA_ITEM_PREV,
    RA_ITEM_RND,     
    RA_PLAY_START,
    RA_PLAY_STOP,
    RA_PLAY_PAUSE,    
    RA_PLAY_TOGGLE,     
    RA_PLAY_DELAY,
    RA_PLAY_DELAYMIN,
    RA_PLAY_DELAYMINON, 
    RA_PLAY_DELAYMINOFF,
    RA_PLAY_RND,       
    RA_PLAY_PL,       
    RA_PLAY_LB,       
    RA_PLAY_LT,
    RA_LSET_PL,
    RA_LSET_PLNEXT,
    RA_LSET_PLPREV,       
    RA_LSET_PLRND,       
    RA_LSET_LB,     
    RA_LSET_LBNEXT,     
    RA_LSET_LBPREV,     
    RA_LSET_LBRND,
    RA_PLI_NEW,
    RA_PLI_REM,
    RA_PLI_REP,
    RA_PL_TOFS
  } RA;
  extern RA RAARR[];
  
  static const char REP_001[] PROGMEM = "pl";     // RA_ITEM->loop, RA_PLAY_PL->loop, list->list, list_pls->list
  static const char REP_002[] PROGMEM = "plt";    // RA_ITEM->loop, RA_PLAY_PL->loop, 
  static const char REP_003[] PROGMEM = "lb";     // RA_ITEM->loop, RA_PLAY_LB->loop, list->list, list_lbs->list, js_init->list, listc->list
  static const char REP_004[] PROGMEM = "lbt";    // RA_ITEM->loop, RA_PLAY_LB->loop
  static const char REP_005[] PROGMEM = "statu";  // RA_PLAY->loop
  static const char REP_006[] PROGMEM = "plc";    // js_init->list, list_plsc->list, listc->list    
  static const char REP_007[] PROGMEM = "pld";    // RA_LGET_PL->pld

  static const char REQ_001[] PROGMEM = "list_alllb";
  static const char REQ_002[] PROGMEM = "list_lb";
  static const char REQ_003[] PROGMEM = "list_allpl";
  static const char REQ_004[] PROGMEM = "list_pl";
  static const char REQ_005[] PROGMEM = "loop";
  static const char REQ_006[] PROGMEM = "list";         // DEMO WEBSERVER
  static const char REQ_007[] PROGMEM = "list_lbs";
  static const char REQ_008[] PROGMEM = "list_pls";
  static const char REQ_009[] PROGMEM = "list_ra";
  static const char REQ_010[] PROGMEM = "list_req";
  static const char REQ_011[] PROGMEM = "list_lbpl";    // DEMO WEBSERVER
  static const char REQ_012[] PROGMEM = "js_init";      // DEMO WEBSERVER
  static const char REQ_013[] PROGMEM = "list_plsc";

  static const char REQ_014[] PROGMEM = "listc";        // DEMO WEBSERVER

  static const char REQ_015[] PROGMEM = "loop_select";  // "get":[{"loop_select":["statu", "pl"]}]      
  static const char REQ_016[] PROGMEM = "list_pld";     // "get":[{"gv":{"n":"list_pld","v":"0"}}]

  static const char* const REQALL[] PROGMEM = {
  REQ_001, REQ_002, REQ_003, REQ_004, REQ_005,
  REQ_006, REQ_007, REQ_008, REQ_009, REQ_010,
  REQ_011, REQ_012, REQ_013, REQ_014, REQ_015, 
  REQ_016
  };  
  // typedef enum _requette_type {
  //     REQ_LB,      
  //     REQ_ALLLB     
  // } RT;
  // extern RT RTARR[];

#endif // CONSTANTS_H


  /*

RANAME_001
RANAME_002
RANAME_003
RANAME_004
RANAME_005
RANAME_006
RANAME_007
RANAME_008
RANAME_009
RANAME_010
RANAME_011
RANAME_012
RANAME_013
RANAME_014
RANAME_015
RANAME_016
RANAME_017
RANAME_018
RANAME_019
RANAME_020
RANAME_021
RANAME_022
RANAME_023
RANAME_024
RANAME_025
RANAME_026
RANAME_027
RANAME_028
RANAME_029
RANAME_030
RANAME_031
RANAME_032
RANAME_033
RANAME_034
RANAME_035
RANAME_036
RANAME_037
RANAME_038
RANAME_039

REP_001
REP_002
REP_003
REP_004
REP_005
REP_006
REP_007
REP_008
REP_009
REP_010
REP_011
REP_012
REP_013
REP_014
REP_015
REP_016
REP_017
REP_018
REP_019
REP_020
REP_021
REP_022
REP_023
REP_024
REP_025
REP_026
REP_027
REP_028
REP_029
REP_030
REP_031
REP_032
REP_033
REP_034
REP_035
REP_036
REP_037
REP_038
REP_039
*/