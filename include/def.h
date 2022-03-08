#ifndef DEF_H
  #define DEF_H
  #ifndef AP_DEFAULT
    // #ifndef DEBUGSERIAL
    //   #define DEBUGSERIAL
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
          #include <LittleFS.h> 
          #define FILESYSTEM LittleFS
        #elif defined USE_SPIFFS
          #include <FS.h>
          #define FILESYSTEM SPIFFS
        #endif
      #endif
    #endif
  #endif
#endif // DEF_H