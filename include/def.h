#ifndef DEF_H
  #define DEF_H
  #ifndef AP_DEFAULT
    // #ifndef DEBUG_KEYBOARD
    //   #define DEBUG_KEYBOARD
    // #endif
    #ifndef FSOK
      #define FSOK
      #ifndef USE_LITTLEFS
        #define USE_LITTLEFS
      #endif
    #endif
    #ifdef FSOK
      #ifndef FILESYSTEM 
        #if defined USE_LITTLEFS
          #if defined(ESP8266)
            #include <LittleFS.h> 
            #define FILESYSTEM LittleFS
          #elif defined(ESP32)
            #include <FS.h>
            #include <LITTLEFS.h>
            #define FILESYSTEM LITTLEFS
          #endif
        #elif defined USE_SPIFFS
          #include <FS.h>
          #define FILESYSTEM SPIFFS
        #endif
      #endif
    #endif
  #endif
#endif // DEF_H