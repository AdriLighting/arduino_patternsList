#ifndef PATTERNS
#define PATTERNS

    #include "Arduino.h"
    #include <ArduinoJson.h>

    #include <adri_timer.h>

    #include "config.h"

    extern uint32_t pInitHeap;


    void heapStatsPatternLoop_print(String & ret);

    void heapStats_print();



    class pattern_list
    {
        
        String      * _listName     = nullptr;
        int         _initialize_cnt = 0;
        int         _maxcnt         = 0;
    public:
        pattern_list(uint8_t maxCnt);
        ~pattern_list(){delete[] _listName;};
        uint8_t item_add(const String &value);
        void    item_getRandomName(String & value);
        void    item_getName(String & value, uint8_t pos);
        int     item_getPosByName(const String &search);
        boolean item_getPosByName(const String &search, int & result);
        void    print();
        void    heapStats_print();
        void    jsonObject(JsonObject & root);
        void    list_cnt(uint8_t & value);
        void    listReSort(int pos, const String & value);
    };

    pattern_list *  patternList_instance();
    int             patternList_itemGetPosByName(pattern_list * ptr, const String &search);
    void            patternList_setPtr(pattern_list * ptr);






    enum mod_pattern_loop { 
        mpl_next,
        mpl_off
    };

    class pattern_loop {

        adri_timer      * _timer;

        uint32_t        _delay          = 35;
        uint32_t        _delayCurrent   = 35;

        boolean         _delayManual    = false;
        
        boolean         _play           = true;
        boolean         _random         = false;

        int             _pos            = 0;
        uint8_t         _posMin         = 0;
        uint8_t         _posMax         = 0;

        pattern_list    * _patterns_list;

    public:


        pattern_loop(pattern_list * ptr);
        ~pattern_loop(){};

        // 1 play effect + set next effect
        // 2 play effect
        // 3 stop play effect
        void loop(mod_pattern_loop & mod, String & v1, boolean delayInMin);
        void remainingTime(uint32_t & v);
        
        void loop_next();
        void next();
        void prev();
        void rnd();
        void rnd(String & value);
        void pattern_set(boolean dir = true);
        boolean pattern_setPosByPos(const String & search );

        void pos_set    (uint8_t value);
        void posMin_set (uint8_t value);
        void posMax_set (uint8_t value);

        void play_set (boolean value);
        void playRnd_set (boolean value);

        void delay_reset        ();
        void delay_set          (uint32_t   value);
        void delayCurrent_set   (uint32_t   value);
        void delayManual_set    (boolean    value);

        void pos_get    (uint8_t & value);
        void posMin_get (uint8_t & value);
        void posMax_get (uint8_t & value);  

        void delay_get          (uint32_t   & value);
        void delayCurrent_get   (uint32_t   & value);
        void delayManual_get    (boolean    & value);

        boolean isPlaying();
        boolean isRndPlaying();

        mod_pattern_loop statuDefault();
        mod_pattern_loop statuNext();

    };
    pattern_loop    * patternLoop_instance() ;
    void            patternLoop_setPtr(pattern_loop * ptr);

#endif