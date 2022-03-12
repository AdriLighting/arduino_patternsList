#include "../include/menu_serial.h"

#ifdef DEBUGSERIAL
#include "../include/tools.h"

void serialReadItem::item_add(   
        const char* name,
        const char* key,
        at_srFunc   f 
    )
{
    _name       = name;
    _key        = key;
    _function   = f;
}


SerialRead * SerialRead_ptr;
SerialRead * SerialReadPtr_get(){
    return SerialRead_ptr;
}
SerialRead::SerialRead(){
    SerialRead_ptr = this;
}
void SerialRead::menu(){
    Serial.printf("\n[SerialRead::menu]\n");
    for (int i = 0; i < _cmd_1_cnt; ++i) {
        Serial.printf_P(PSTR("[%5d][%25s][%-15s]\n"), 
                    i, 
                    _cmd_1_Array[i]._name, 
                    _cmd_1_Array[i]._key
                );
    }
    Serial.printf("\n[!]\n");
    for (int i = 0; i < _cmd_2_cnt; ++i) {
        Serial.printf_P(PSTR("[%5d][%25s][%-15s]\n"), 
                    i, 
                    _cmd_2_Array[i]._name, 
                    _cmd_2_Array[i]._key
                );
    } 
    if (_cmd_3_desc!="") { Serial.printf_P(PSTR("\n[%s][%s]\n"), _cmd_3_sep, _cmd_3_desc.c_str()); } 
    if (_cmd_4_desc!="") { Serial.printf_P(PSTR("\n[%s][%s]\n"), _cmd_4_sep, _cmd_4_desc.c_str()); }                  
}
void SerialRead::cmd_array(int pos, int cnt) {
    switch (pos){
        case 1: 
            _cmd_1_Array = new serialReadItem[cnt];
        break;
        case 2: 
            _cmd_2_Array = new serialReadItem[cnt];
        break;
    }
}


void SerialRead::cmd_item_add(int pos, const char* name, const char* key, at_srFunc f){
    switch (pos){
        case 1: 
            _cmd_1_Array[_cmd_1_cnt].item_add(name, key, f);
            _cmd_1_cnt++;
        break;
        case 2: 
            _cmd_2_Array[_cmd_2_cnt].item_add(name, key, f);
            _cmd_2_cnt++;
        break;
    }
}
void SerialRead::cmd_3(char* sep, const String & desc, at_srFunc f){
    _cmd_3_sep  = sep;
    _cmd_3_desc = desc;
    _cmd_3      = f;
}
void SerialRead::cmd_4(char* sep, const String & desc, at_srFunc f){
    _cmd_4_sep  = sep;
    _cmd_4_desc = desc;
    _cmd_4      = f;    
}

void SerialRead::loop(){

    if(Serial.available()) { 
        boolean next = true;

        String a = Serial.readStringUntil('\n');

        String cmd = "";
        String value = "";  
        static String lastMsg = "";

        if (a.indexOf("^")>=0)a=lastMsg;
        lastMsg = a;

        if (a.indexOf("!")>=0){
            splitText(a, "!", cmd,  value) ; 
            for (int i = 0; i < _cmd_2_cnt; ++i) {
                if (cmd == _cmd_2_Array[i]._key) {
                    _cmd_2_Array[i]._function(cmd, value);
                    break;
                }
            } 
            next = false;       
        }       
        if (!next) return;
        if (_cmd_3_sep != (char *)"") {
            if (a.indexOf(_cmd_3_sep)>=0) {
                 splitText(a, _cmd_3_sep, cmd,  value) ; 
                _cmd_3(cmd, value);
                return;
            }   
        }   
        if (_cmd_4_sep != (char *)"") {
            if (a.indexOf(_cmd_4_sep)>=0) {
                 splitText(a, _cmd_4_sep, cmd,  value) ; 
                _cmd_4(cmd, value);
                return;
            }                            
        } 
        if (next) {
            for (int i = 0; i < _cmd_1_cnt; ++i) {
                if (a.indexOf(_cmd_1_Array[i]._key)>=0 ) {
                    _cmd_1_Array[i]._function("", "");
                    break;
                }
            }                        
        }        
        
    }       
}
void SerialRead::read(String a){

    // if(Serial.available()) { 
        boolean next = true;

        // String a = Serial.readStringUntil('\n');

        String cmd = "";
        String value = "";  
        static String lastMsg = "";

        if (a.indexOf("^")>=0)a=lastMsg;
        lastMsg = a;

        if (a.indexOf("!")>=0){
            splitText(a, "!", cmd,  value) ; 
            for (int i = 0; i < _cmd_2_cnt; ++i) {
                if (cmd == _cmd_2_Array[i]._key) {
                    _cmd_2_Array[i]._function(cmd, value);
                    break;
                }
            } 
            next = false;       
        }       
        if (!next) return;
        if (_cmd_3_sep != (char *)"") {
            if (a.indexOf(_cmd_3_sep)>=0) {
                 splitText(a, _cmd_3_sep, cmd,  value) ; 
                _cmd_3(cmd, value);
                return;
            }   
        }   
        if (_cmd_4_sep != (char *)"") {
            if (a.indexOf(_cmd_4_sep)>=0) {
                 splitText(a, _cmd_4_sep, cmd,  value) ; 
                _cmd_4(cmd, value);
                return;
            }                            
        } 
        if (next) {
            for (int i = 0; i < _cmd_1_cnt; ++i) {
                if (a.indexOf(_cmd_1_Array[i]._key)>=0 ) {
                    _cmd_1_Array[i]._function("", "");
                    break;
                }
            }                        
        }       
        
    // }       
}
int SerialRead::splitText(const String & A_readString, const char* sep, String & cmd, String & value) {

    String  s_command;
    String  s_valueCommand;
    String  s_readString = A_readString;
    char    inputChar[s_readString.length() + 1] ;
            s_readString.toCharArray(inputChar, s_readString.length() + 1);
    char    * Rcommand  = strtok(inputChar, sep);

    while (Rcommand != 0){          
        char* separator  = strchr(Rcommand, '=');
        if (separator != 0) {
            *separator  = 0;            
            ++separator ;
            s_command = String(Rcommand);
            s_valueCommand = String(separator); 
            
            if (s_command!=""){
                value = s_valueCommand;
                cmd = s_command;
                break;
            }
        }
        Rcommand = strtok(0, sep); 
    }

    return 0;
}

#endif
