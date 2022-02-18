#include "main.h"
#include <arduino_patternsList.h>

playlist_management * _playlist = nullptr;
programme_loop          * _programme = nullptr;
void _sort( const char arr[] ){
      pattern_list * _ptr =  patternList_instance();
      _ptr->_ListName.sort([](listItem *&a, listItem *&b){ String tmp=FPSTR(&arr[(uint8_t)a->_id]); return strcmp_P(tmp.c_str(), (LPALLNAMES[(uint8_t)b->_id]));});
      for(int i=0; i<_ptr->_ListName.size(); i++){ Serial.printf_P(PSTR("[%-3d][pos: %3d][name: %S]\n"), i, _ptr->_ListName[i]->_id, _ptr->_ListName[i]->_name.c_str());   }  

}
void setup()
{
  Serial.begin(115200);

  for(unsigned long const serialBeginTime = millis(); !Serial && (millis() - serialBeginTime > 5000); ) { }
  delay(300);

  pInitHeap = ESP.getFreeHeap();

  Serial.println(F("\n##################################################\n"));  

  Serial.setDebugOutput(false);

  LittleFS.begin();

  boolean fsFormat = true;
  if (fsFormat) LittleFS.format();




/*
  la playlist se sert des items fournit par la liste principal
    la playlist doit correspondre a la liste pricipal (nom de liste ou id)

  par default les playlist seront creer
      compilation parametre
          -D PLAYLIST_MAXEFF  = nb maximum d'items            (default = 10)
          -D PLAYLIST_MAX     = nb maximum de playlist    (default = 3)
*/

    _playlist   = new playlist_management(); 
    _programme  = new programme_loop();   

    String        patternListName;
    uint8_t       patternListCnt;
    pattern_list  * _ptr;

    

/*
  initialization des items(String) de la liste principal 
*/
    patternListCnt = ARRAY_SIZE(LPALLNAMES);
    _programme->patternList_initialize(patternListCnt); // definir la taille de la liste
    _programme->patternList_getName(patternListName);
    _ptr =  patternList_instance();
    for (uint8_t i = 0; i < patternListCnt; i++){_programme->patternList_item_add(FPSTR(LPALLNAMES[i]));} // ajout des items
    for (uint8_t i = 0; i < patternListCnt; i++){_programme->patternList_item_new(FPSTR(LPALLNAMES[i]), i);} // ajout des items
    Serial.println("");  
    Serial.print("patternListCnt: ");  
    Serial.println(patternListCnt);  
      for(int i=0; i<_ptr->_ListName.size(); i++){ Serial.printf_P(PSTR("[%-3d][pos: %3d][name: %S]\n"), i, _ptr->_ListName[i]->_id, _ptr->_ListName[i]->_name.c_str());   }  
    Serial.println("--------");
    Serial.println("ALPHA");
    _ptr->_ListName.sort([](listItem *&a, listItem *&b){ String tmp=FPSTR(LPALLNAMES[(uint8_t)a->_id]); return strcmp_P(tmp.c_str(), (LPALLNAMES[(uint8_t)b->_id]));});
      for(int i=0; i<_ptr->_ListName.size(); i++){ Serial.printf_P(PSTR("[%-3d][pos: %3d][name: %S]\n"), i, _ptr->_ListName[i]->_id, _ptr->_ListName[i]->_name.c_str());   }  
    Serial.println("--------");
    Serial.println("END");
    _ptr->_ListName.sort([](listItem *&a, listItem *&b){ return b->_id - a->_id;}); 
      for(int i=0; i<_ptr->_ListName.size(); i++){ Serial.printf_P(PSTR("[%-3d][pos: %3d][name: %S]\n"), i, _ptr->_ListName[i]->_id, _ptr->_ListName[i]->_name.c_str());   }  
    Serial.println("--------");
    Serial.println("BASE");
    _ptr->_ListName.sort([](listItem *&a, listItem *&b){ return (((a->_id&0xFF) - (b->_id&0xFF))<<8) + (((a->_id&0xFF00) - (b->_id&0xFF00))>>8);});
      for(int i=0; i<_ptr->_ListName.size(); i++){ Serial.printf_P(PSTR("[%-3d][pos: %3d][name: %S]\n"), i, _ptr->_ListName[i]->_id, _ptr->_ListName[i]->_name.c_str());   }  
    Serial.println("--------");

    _programme->patternList_print(); // debug patterns items


    patternListCnt = ARRAY_SIZE(LPALLNAMES_CAT);
    _programme->patternList_initialize(patternListCnt, "cat"); // definir la taille de la liste
    _programme->patternList_getName(patternListName);
    _ptr =  patternList_instance();

    for (uint8_t i = 0; i < patternListCnt; i++){_programme->patternList_item_add(FPSTR(LPALLNAMES_CAT[i]));} // ajout des items
    for (uint8_t i = 0; i < patternListCnt; i++){_programme->patternList_item_new(FPSTR(LPALLNAMES_CAT[i]), i);} // ajout des items
    Serial.println("");  
    Serial.print("patternListCnt: ");  
    Serial.println(patternListCnt);  
      for(int i=0; i<_ptr->_ListName.size(); i++){ Serial.printf_P(PSTR("[%-3d][pos: %3d][name: %S]\n"), i, _ptr->_ListName[i]->_id, _ptr->_ListName[i]->_name.c_str());   }  
    Serial.println("--------");
    Serial.println("ALPHA");
    _ptr->_ListName.sort([](listItem *&a, listItem *&b){ String tmp=FPSTR(LPALLNAMES_CAT[(uint8_t)a->_id]); return strcmp_P(tmp.c_str(), (LPALLNAMES_CAT[(uint8_t)b->_id]));});
      for(int i=0; i<_ptr->_ListName.size(); i++){ Serial.printf_P(PSTR("[%-3d][pos: %3d][name: %S]\n"), i, _ptr->_ListName[i]->_id, _ptr->_ListName[i]->_name.c_str());   }  
    Serial.println("--------");
    Serial.println("END");
    _ptr->_ListName.sort([](listItem *&a, listItem *&b){ return b->_id - a->_id;}); 
      for(int i=0; i<_ptr->_ListName.size(); i++){ Serial.printf_P(PSTR("[%-3d][pos: %3d][name: %S]\n"), i, _ptr->_ListName[i]->_id, _ptr->_ListName[i]->_name.c_str());   }  
    Serial.println("--------");
    Serial.println("BASE");
    _ptr->_ListName.sort([](listItem *&a, listItem *&b){ return (((a->_id&0xFF) - (b->_id&0xFF))<<8) + (((a->_id&0xFF00) - (b->_id&0xFF00))>>8);});
      for(int i=0; i<_ptr->_ListName.size(); i++){ Serial.printf_P(PSTR("[%-3d][pos: %3d][name: %S]\n"), i, _ptr->_ListName[i]->_id, _ptr->_ListName[i]->_name.c_str());   }  
    Serial.println("--------");

    _programme->patternList_print(); // debug patterns items

    uint8_t plPos = 0;

    if (fsFormat){
      _playlist->list_new("df",   plPos);
      _playlist->list_new("cat",  plPos);
    }   else {
      _playlist->list_fromSpiff();
    }

    _playlist->list_print(); // debug playlists 

    if (_playlist->list_pos(1, patternListName)){
      _playlist->print();
      _playlist->item_restore();
      _playlist->item_print(); // debug playlist items      
      _playlist->item_toArray(255, "item", FPSTR(LPALLNAMES_CAT[0]));
      _playlist->item_toArray(255, "item", FPSTR(LPALLNAMES_CAT[1]));
      _playlist->item_toArray(255, "item", FPSTR(LPALLNAMES_CAT[2]));
    }


    SPIFFS_printFiles("/"); // debug fs
    SPIFFS_fileRead("/playlist/compoNames.json"); // debug fs
    SPIFFS_fileRead("/playlist/playlist/compoName_0.json"); // debug fs
    SPIFFS_fileRead("/playlist/playlist/compoName_1.json"); // debug fs

    _playlist->list_statu_set(false);      // activation de la lecture de la playListe
    _playlist->item_pos_set(0);           // selection de litem de la playlist selectionnée
    _playlist->item_print();              // debug playlist items

    _programme->patternLoop_pos_set(0);   // selection de litem de la liste pricipal/playListe(si activer)
    _programme->delayCurrent_set(10);     // delay de rotation
    _programme->play_set(true);           // activation de la rotation
    _programme->playRnd_set(false);       // activation de la lecture aléatoire

}

void fs_restore_defaultPlaylist(boolean remove){
/*
    if (remove) {
        // deleteRecursive(LittleFS, FPSTR(FOPATH_PLAYLISTEFF));
        deleteRecursive(LittleFS, FPSTR(FOPATH_PLAYLIST));
        _playlist->list_removeFromSpiff() ;
    }

    _playlist->list_fromSpiff() ;
    _playlist->list_print();
    
    // if(!LittleFS.exists(FPSTR(FOPATH_PLAYLIST))) {
    if(remove) {
        // LittleFS.mkdir(FPSTR(FOPATH_PLAYLISTEFF));  
        String      plName;
        // String       listName;
        uint8_t     pMax;
        String      pName;

        _programme->patternLoop_posMax_get(pMax);

        for(int i = 0; i < PLAYLIST_MAX; ++i) {
            _playlist->list_pos(i);
            _playlist->item_restore();
            _playlist->list_name(plName);
            for(int j = 0; j < (PLAYLIST_MAXEFF/2); ++j) {

                uint8_t rnd = random(0, pMax-1);
                _programme->patternList_item_getName(pName, rnd);

                // listName = (String)FPSTR(FOPATH_PLAYLISTEFF) + plName + (String)FPSTR(FSLSH) + String(j);
                _playlist->item_toArray(255, pName, pName + " VALUE");

                // yield();     
            }

            // yield();     
        }

        yield();    
    }
*/
}

boolean patterns_loop(uint16_t & ret){
    static mod_pattern_loop pat_check   = programmeLoop_instance()->statuDefault();
    static String           pat_name    = "";
    static boolean          playlist;

    programmeLoop_instance()->loop(pat_check, pat_name, playlist, false);      

    if (pat_check == programmeLoop_instance()->statuNext()) {

        pat_check =  programmeLoop_instance()->statuDefault();

        // FOR DEBUG
        String heap;
        String time;
        on_timeD(time);
        heapStatsPatternLoop_print(heap);
        
        if (playlist) {
          String plCfg;
          playlist_instance()->item_patternConfig(plCfg);
          Serial.printf_P(PSTR("\n[patterns_loop]\n\t%s\n\t%s\n\t%-15s%s\n"), pat_name.c_str(), plCfg.c_str(), time.c_str(), heap.c_str());
        } else {
          Serial.printf_P(PSTR("\n[patterns_loop]\n\t%s\n\t%-15s%s\n"), pat_name.c_str(), time.c_str(), heap.c_str());
        }
        // if ... Something {
        //  return true;
        // } 

    }   
    return false;
}
void loop()
{
    uint16_t pos;
    if (patterns_loop(pos)) {
            // ... Do something
    }   
}





void seconds2timeD(unsigned long s, char * time) {
   int seconds    = (s /   (1000)         ) % 60   ;
   int minutes    = (s /   (1000*60)      ) % 60   ;
   int hours      = (s /   (1000*3600)    ) % 24   ;
   sprintf(time,"%02d:%02d:%02d", hours , minutes, seconds);
}
void on_timeD(String & result) {
   char t[100];
   seconds2timeD(millis(), t);
   result = String(t);
}

void deleteRecursive(fs::FS &fs, const String &path) {
    // if (fs.remove(path)) {
    //     fs.open(path.substring(0, path.lastIndexOf('/')) + "/", "w");
    //     return;
    // }
    // Dir dir = fs.openDir(path);
    // while (dir.next()) {
    //     FShelper::deleteRecursive(fs, path + '/' + dir.fileName());
    // }
    // fs.rmdir(path);


    File file = fs.open(path, "r");
    bool isDir = file.isDirectory();
    file.close();

    if (!isDir) {
        fs.remove(path);
        return;
    }

    Dir dir = fs.openDir(path);

    while (dir.next()) {
        deleteRecursive(fs, path + '/' + dir.fileName());
        yield();
    }

    fs.rmdir(path);

}