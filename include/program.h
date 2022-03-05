#ifndef PROGRAM_H
#define PROGRAM_H

  #include <Arduino.h>
  #include <ArduinoJson.h>

  #include "LList/LList.h"
  #include "constants.h"
  #include "tools.h"
  #include "playlist.h"
  #include "patterns.h"
  #include "lloop.h"



  class ListSortItems {
  public:
    String    _name;
    uint16_t  _id;
    ListSortItems(String n, uint16_t id) : _name(n), _id(id) {};
  };
  class LBnames
  {

    const char * const * _arr;  
  public:
    const char  * _name = "";
    uint8_t     _cnt; 

    LBnames()  {};
    void setup(const char * name, uint8_t items, const char * const * arr);
    void print(uint8_t p);
    void json(JsonArray & );

  };
  class Program : public Listbase, public ListLoop {
    typedef std::function<void(const String & v1, const uint16_t & v2, boolean upd)> callback_function_t;
    // typedef std::function<void(const String & v1)> callback_error;

    LBnames             * _LBnames    = nullptr;  
    LB_flag             _lbStatu;

    Playlist_list       * _Playlists  = nullptr;

    HeapStatu           _HeapStatu;

    callback_function_t _callback       = nullptr;
    // callback_error      _callbackError  = nullptr;
 
    void get_itemBase(String &);
    void get_itemPosBase(uint16_t &);
    void get_itemPos(uint16_t &);

    void set_itemNext();
    void set_itemPrev();
    void set_itemRnd();
    void set_itemPos(const uint16_t & p);
    void lb_print();

    void pl_print();
    void pl_print(uint8_t);

    void item_callback(boolean upd = true);



  public:
    Program (uint8_t nbLb );
    ~Program ();


    PL_flag         _plStatu;

    void initialize(const uint16_t & , const char* const* arr, const char  * const &, SORT_TYPE t = ST_BASE);
    void initialize_playlist(uint8_t , const uint8_t * const &, const char ** const &);
    void initialize_lb(uint8_t p, const char * name, uint8_t items, const char * const * arr);


    void    pl_item_new(uint8_t, uint8_t);
    void    pl_item_new(uint8_t);
    void    pl_item_new(DynamicJsonDocument & doc, DynamicJsonDocument & rep);
    void    pl_item_remove(uint8_t, uint8_t);
    void    pl_item_remove(DynamicJsonDocument & doc, DynamicJsonDocument & rep);
    void    pl_item_toArray(uint8_t, uint8_t, const String &, const String &, const String &);
    boolean pl_item_removeitemIdByArrayPos(uint8_t pP, uint8_t aP);

    boolean pl_set_listPos(uint8_t pos, const char  * currentList);


    #ifdef FSOK
      void pl_fs();  
      void pl_fs(uint8_t pP, DynamicJsonDocument & doc);  
      void pl_fs_restore(uint8_t pP);  
      void pl_fs_restore();  
    #endif
    void pl_json(JsonObject & doc, boolean pI = true, boolean lRef = false);
    void pl_currentJson(uint8_t p, JsonObject & doc, boolean pI = true);
    void pl_currentJson(JsonObject & doc, boolean pI = true);


    void lb_json(JsonObject & doc, boolean pI = true) ;
    void lb_json(JsonObject & doc, uint8_t p, boolean pI = true) ;

    void loop(mod_pattern_loop &, String &s);
    void handle();

    void set_json_v1(JsonObject & doc);
    void set_json_statu(JsonObject &);
    void set_json_pl(JsonObject &);
    void set_json_plt(JsonObject &);
    void set_json_lb(JsonObject &);
    void set_json_lbt(JsonObject &);

    void set_callback(callback_function_t);

    void print(PRINT_MOD mod) ;

    void remote_action(RA action, ...);

    // boolean get_pl_itemIdByArrayPos(uint8_t aP); 
  };

  Program * ProgramPtrGet();
#endif // PROGRAM_H   