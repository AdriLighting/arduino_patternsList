#ifndef PATTERNS
#define PATTERNS

  #include "Arduino.h"
  #include <ArduinoJson.h>

  #include "constants.h"


  class Listbase  {
      // enum STATU
      // {
      //   NOINIT,   
      // };    
  private:
    const char  * _name   = "df";
    // const char  **  _list   = nullptr;
    String      * _list   = nullptr;
    uint16_t    _cnt      = 0;
    uint16_t    _cntMax   = 0;
    // STATU       _STATU      = 


  public:
    Listbase();
    ~Listbase();

    void print();

    void initialize(const uint16_t & maxCnt, const char  * const & n);

    void list_delete();
    void item_add(const String & n);

    void get_name   (const char * & v1);
    void get_cnt    (uint16_t & v1);
    void get_cntMax (uint16_t & v1);

    void    get_itemRnd         (String & value, uint16_t & p);
    void    get_itemNameByPos   (const uint16_t & pos, String & value);
    boolean get_itemPosByName   (const String & search, uint16_t & result);
    boolean get_item            (const String & search);

    void jsonObject(JsonObject & root);

        
    // void loop(mod_pattern_loop & mod, String & v1, boolean delayInMin);
  };
  Listbase * ListbasePtrGet();





#endif