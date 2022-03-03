#ifndef PLAYLIST_CPP
#define PLAYLIST_CPP
  #include <Arduino.h>
  #include <ArduinoJson.h>
  class Playlist_item {
    uint8_t _id = 0;
    String  _lbl          = "null";
    String  _itemBase     = "null";
    String  _itemBaseCfg  = "null";
  public:
    Playlist_item(){};
    ~Playlist_item(){};

    void print();

    void set_id(uint8_t);
    void set_lbl(const String &);
    void set_itemBase(const String &);
    void set_itemBaseCfg(const String &);

    void get_id(uint8_t &);
    void get_lbl(String &);
    void get_itemBase(String &);
    void get_itemBaseCfg(String &);
  };  
  class Playlist_list {
    uint8_t     _pos        = 0;
    String      _lbl        = "";
    uint16_t    _items_max  = 0;
    uint16_t    _items_cnt  = 0;  
    const char  * _listRef  = "";  
  public:
    // uint8_t     _selected   = 0;
    // String      _listRef  = "";
    Playlist_item     * _Playlist_itemArray = nullptr;

    Playlist_list( ) {}
    ~Playlist_list() {delete[] _Playlist_itemArray;};

     void print() ;

    void item_print();
    void item_json(JsonObject & doc, boolean pI = true);

    void item_toArray(uint8_t, const String &, const String &, const String &);
    void item_remove(uint8_t remove); 
    void item_df();

    void get_listRef(const char * &);
    void get_pos(uint8_t &);
    void get_lbl(String &);
    void get_items_max(uint16_t &);
    void get_items_cnt(uint16_t &);

    void set_listRef(const char  * const &);
    void set_pos(uint8_t);
    void set_lbl(const String &);
    void set_items_max(uint8_t);
    void set_items_cnt(uint8_t);  

    boolean get_itemIdByArrayPos(uint8_t aP, uint8_t & r); 
    boolean get_itemBaseByArrayPos(uint8_t aP, String & r); 
  };

#endif // PLAYLIST_CPP