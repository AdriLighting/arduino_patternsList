#include "apwebserver.h"

#include <arduinoPatternList.h>

#include "system.h"

#ifdef INCLUDE_FALLBACK_INDEX_HTM
  #include "extras/index_htm.h"
#endif

#ifdef WEBSERVERFLASH
  #include "html.h"  
#endif

#ifdef FSOK
  #if defined USE_SPIFFS
    #include <FS.h>
    const char* fsName = "SPIFFS";
    FS* fileSystem = &SPIFFS;
    SPIFFSConfig fileSystemConfig = SPIFFSConfig();
  #elif defined USE_LITTLEFS
    #include <LittleFS.h>
    const char* fsName = "LittleFS";
    FS* fileSystem = &LittleFS;
    LittleFSConfig fileSystemConfig = LittleFSConfig();
  #elif defined USE_SDFS
    #include <SDFS.h>
    const char* fsName = "SDFS";
    FS* fileSystem = &SDFS;
    SDFSConfig fileSystemConfig = SDFSConfig();
    // fileSystemConfig.setCSPin(chipSelectPin);
  #else
  // #error Please select a filesystem first by uncommenting one of the "#define USE_xxx" lines at the beginning of the sketch.
  #endif
#endif


#define DBG_OUTPUT_PORT Serial

String unsupportedFiles = String();

File uploadFile;

static const char TEXT_JSON         [] PROGMEM = "text/json";
static const char APPLICATION_JSON  [] PROGMEM = "application/json";
static const char TEXT_PLAIN        [] PROGMEM = "text/plain";
static const char FS_INIT_ERROR     [] PROGMEM = "FS INIT ERROR";
static const char FILE_NOT_FOUND    [] PROGMEM = "FileNotFound";

serverSystem _system;


espwebServer::espwebServer(int port) : server(port) {

}

void espwebServer::setup(boolean setHandleRoot){
  if (setHandleRoot){ 
#ifdef WEBSERVERFLASH
    server.on("/",       std::bind(&espwebServer::handleRootFlash, this));
#else
    server.on("/",       std::bind(&espwebServer::handleRoot, this));
#endif
  }

  server.on("/request_reset", HTTP_GET, [this]() {
    server.send(200, FPSTR(TEXT_PLAIN), "ESP restart in 1sc\n");
    delay(1000);
    ESP.restart();
  }); 
  server.on("/update", HTTP_POST, [this]() {
    server.sendHeader("Connection", "close");
    server.send(200, FPSTR(TEXT_PLAIN), (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, [this]() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.setDebugOutput(true);
      WiFiUDP::stopAll();
      Serial.printf_P(PSTR("Update: %s\n"), upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf_P(PSTR("Update Success: %u\nRebooting...\n"), upload.totalSize);
      } else {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
    yield();
  }); 

#ifdef WEBSERVERFLASH
    server.on("/asset/css/bootstrap.css",           HTTP_GET, std::bind(&espwebServer::handleBootstrap,    this));
    server.on("/asset/css/style.css",               HTTP_GET, std::bind(&espwebServer::handleStyle,        this));
    server.on("/asset/js/bootstrap.bundle.min.js",  HTTP_GET, std::bind(&espwebServer::handleBootstrapJs,  this));
    server.on("/asset/js/jquery.min.js",            HTTP_GET, std::bind(&espwebServer::handleJquerry,      this));  
#endif

    server.on("/api",     HTTP_POST,    std::bind(&espwebServer::handleJson,        this));
    server.on("/statu",   HTTP_GET,     std::bind(&espwebServer::handleStatu,       this));
    server.on("/statu",   HTTP_GET,     std::bind(&espwebServer::handleStatu,       this));
    server.on("/status",  HTTP_GET,     std::bind(&espwebServer::handleStatus,      this));

#ifdef FSOK
    server.on("/list",    HTTP_GET,     std::bind(&espwebServer::handleFileList,    this));
    server.on("/edit",    HTTP_GET,     std::bind(&espwebServer::handleGetEdit,     this));
    server.on("/edit",    HTTP_PUT,     std::bind(&espwebServer::handleFileCreate,  this));
    server.on("/edit",    HTTP_DELETE,  std::bind(&espwebServer::handleFileDelete,  this));
    server.on("/edit",    HTTP_POST,    std::bind(&espwebServer::replyOK, this), 
                                        std::bind(&espwebServer::handleFileUpload,  this));  
#endif

    server.onNotFound( std::bind(&espwebServer::handleNotFound, this));
   
}

#ifdef WEBSERVERFLASH
void espwebServer::handleBootstrap()  {server.send_P(200, "text/css", CSS_BOOSTRAP);}
void espwebServer::handleStyle()      {server.send_P(200, "text/css", CSS_STYLE);}
void espwebServer::handleBootstrapJs(){server.send_P(200, "application/javascript", JS_BOOSTRAP);}
void espwebServer::handleJquerry()    {server.send_P(200, "application/javascript", JS_JQUERY);}
void espwebServer::handleRootFlash(){
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200,"text/html", "");
  server.sendContent(HTML_INDEX);
  server.client().stop();
}   
#endif


void espwebServer::handleJson() {
  if (server.method() == HTTP_POST) {
    String header;
    for (uint8_t i = 0; i < server.headers(); i++) {
      header += server.headerName(i) + ": " + server.header(i) + "\n";
    }
#ifdef DEBUG
    Serial.printf_P(PSTR("[HTTP_POST /json][HEADERS]\n%s"), header.c_str());  
#endif
    String json;
    for (uint8_t i = 0; i < server.args(); i++) {
      json +=  server.arg(i) + "\n";
    }        
    DynamicJsonDocument doc(2048);  
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
#ifdef DEBUG
      Serial.printf_P(PSTR("[HTTP_POST /json][PARSSING ERROR]\n%s"), json.c_str());  
#endif
      replyOK();
    } else {
      // DeviceserverPtrGet()->handleJson(doc);
      String reponse;
      // _WebserverRequest.parsingRequest(doc, reponse, "");
      replyJson(reponse);
    } 
    
  }  
}
void espwebServer::handleStatu(){
  DynamicJsonDocument doc(3150);
  JsonObject root = doc.to<JsonObject>();
  String out = "";
  String pos ;
  JsonObject oOjbect;
  JsonObject oDevice = root.createNestedObject(F("device"));
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "sys" ) { 
      pos = server.arg(i); 
      if (pos == FPSTR(req_sys_1)) {
        oOjbect = oDevice.createNestedObject(F("system"));
        _system.sys_json(oOjbect);
      } else if (pos == FPSTR(req_sys_2)) {
        oOjbect = oDevice.createNestedObject(F("network"));
        _system.sys_networkJson(oOjbect);
      } else if (pos == FPSTR(req_sys_3)) {
        oOjbect = oDevice.createNestedObject(F("firmware"));
        _system.sys_firmWareJson(oOjbect);
      } else {
        oOjbect = oDevice.createNestedObject(F("system"));
        _system.sys_json(oOjbect);
        oOjbect = oDevice.createNestedObject(F("network"));
        _system.sys_networkJson(oOjbect);
        oOjbect = oDevice.createNestedObject(F("firmware"));
        _system.sys_firmWareJson(oOjbect);        
      }   
    }   
  } 
  serializeJson(doc, out);
  doc.clear();

  if (out != "") {
    server.send(200, FPSTR(APPLICATION_JSON), out);    
  } else {
    server.send(404, FPSTR(TEXT_PLAIN), "");
  }
  yield(); 
}

#ifdef FSOK
void espwebServer::handleRoot() {
  // if (captivePortal()) return;


  if (!fsOK) {
    return replyServerError(FPSTR(FS_INIT_ERROR));
  }
  String uri;
  #if defined(ESP8266)
    uri = ESP8266WebServer::urlDecode(server.uri()); // required to read paths with blanks
  #elif defined(ESP32)
    uri = WebServer::urlDecode(server.uri()); // required to read paths with blanks
  #endif
  
  
  if (handleFileRead(uri)) {
    return;
  }
  return replyNotFound(FPSTR(FILE_NOT_FOUND));
}
#else
void espwebServer::handleRoot() {
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, 400,

           "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP8266!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
  </body>\
</html>",

           hr, min % 60, sec % 60
          );
  server.send(200, "text/html", temp);
}
#endif

void espwebServer::filesystem_ok(bool val){
  fsOK = val;
} 

void espwebServer::handleClient() {
  server.handleClient();
}
void espwebServer::begin() {
  server.begin();
} 

void espwebServer::replyOK() {
  server.send(200, FPSTR(TEXT_PLAIN), "");
}

void espwebServer::replyJson(const String & msg) {
  server.send(200, FPSTR(APPLICATION_JSON), msg);
}

void espwebServer::replyOKWithMsg(const String & msg) {
  server.send(200, FPSTR(TEXT_PLAIN), msg);
}

void espwebServer::replyNotFound(const String & msg) {
  server.send(404, FPSTR(TEXT_PLAIN), msg);
}

void espwebServer::replyBadRequest(const String & msg) {
#ifdef DEBUG
  DBG_OUTPUT_PORT.println(msg);  
#endif
  server.send(400, FPSTR(TEXT_PLAIN), msg + "\r\n");
}

void espwebServer::replyServerError(const String & msg) {
#ifdef DEBUG
  DBG_OUTPUT_PORT.println(msg);  
#endif
  server.send(500, FPSTR(TEXT_PLAIN), msg + "\r\n");
}

#ifdef USE_SPIFFS
/*
   Checks filename for character combinations that are not supported by FSBrowser (alhtough valid on SPIFFS).
   Returns an empty String if supported, or detail of error(s) if unsupported
*/
String checkForUnsupportedPath(const String & filename) {
  String error = String();
  if (!filename.startsWith("/")) {
    error += F("!NO_LEADING_SLASH! ");
  }
  if (filename.indexOf("//") != -1) {
    error += F("!DOUBLE_SLASH! ");
  }
  if (filename.endsWith("/")) {
    error += F("!TRAILING_SLASH! ");
  }
  return error;
}
#endif


////////////////////////////////
// Request handlers

/*
   Return the FS type, status and size info
*/
#ifdef FSOK
  void espwebServer::handleStatus() {
    #ifdef DEBUG
      DBG_OUTPUT_PORT.println("handleStatus");  
    #endif
    FSInfo fs_info;
    String json;
    json.reserve(128);

    json = "{\"type\":\"";
    json += fsName;
    json += "\", \"isOk\":";
    if (fsOK) {
      fileSystem->info(fs_info);
      json += F("\"true\", \"totalBytes\":\"");
      json += fs_info.totalBytes;
      json += F("\", \"usedBytes\":\"");
      json += fs_info.usedBytes;
      json += "\"";
    } else {
      json += "\"false\"";
    }
    json += F(",\"unsupportedFiles\":\"");
    json += unsupportedFiles;
    json += "\"}";

    server.send(200, "application/json", json);
  }  
#else
  void espwebServer::handleStatus(){
    DynamicJsonDocument doc(3150);
    JsonObject root = doc.to<JsonObject>();
    String out = "";
    JsonObject oOjbect;
    JsonObject oDevice = root.createNestedObject(F("device"));
    oOjbect = oDevice.createNestedObject(F("system"));
    _system.sys_json(oOjbect);
    oOjbect = oDevice.createNestedObject(F("firmware"));
    _system.sys_firmWareJson(oOjbect);  
    serializeJson(doc, out);
    doc.clear();

    if (out != "") {
      server.send(200, FPSTR(APPLICATION_JSON), out);    
    } else {
      server.send(404, FPSTR(TEXT_PLAIN), "");
    }
    yield(); 
  }
#endif


#ifdef FSOK
/*
   Return the list of files in the directory specified by the "dir" query string parameter.
   Also demonstrates the use of chuncked responses.
*/
void espwebServer::handleFileList() {
  if (!fsOK) {
    return replyServerError(FPSTR(FS_INIT_ERROR));
  }

  if (!server.hasArg("dir")) {
    return replyBadRequest(F("DIR ARG MISSING"));
  }

  String path = server.arg("dir");
  if (path != "/" && !fileSystem->exists(path)) {
    return replyBadRequest("BAD PATH");
  }

  #ifdef DEBUG
    DBG_OUTPUT_PORT.println(String("handleFileList: ") + path);  
  #endif
  Dir dir = fileSystem->openDir(path);
  path.clear();

  // use HTTP/1.1 Chunked response to avoid building a huge temporary string
  if (!server.chunkedResponseModeStart(200, "text/json")) {
    server.send(505, F("text/html"), F("HTTP1.1 required"));
    return;
  }

  // use the same string for every line
  String output;
  output.reserve(64);
  while (dir.next()) {
    #ifdef USE_SPIFFS
      String error = checkForUnsupportedPath(dir.fileName());
      if (error.length() > 0) {
        #ifdef DEBUG
          DBG_OUTPUT_PORT.println(String("Ignoring ") + error + dir.fileName());  
        #endif
        continue;
      }
    #endif
    if (output.length()) {
      // send string from previous iteration
      // as an HTTP chunk
      server.sendContent(output);
      output = ',';
    } else {
      output = '[';
    }

    output += "{\"type\":\"";
    if (dir.isDirectory()) {
      output += "dir";
    } else {
      output += F("file\",\"size\":\"");
      output += dir.fileSize();
    }

    output += F("\",\"name\":\"");
    // Always return names without leading "/"
    if (dir.fileName()[0] == '/') {
      output += &(dir.fileName()[1]);
    } else {
      output += dir.fileName();
    }

    output += "\"}";
  }

  // send last string
  output += "]";
  server.sendContent(output);
  server.chunkedResponseFinalize();
}  



/*
   Read the given file from the filesystem and stream it back to the client
*/
bool espwebServer::handleFileRead(const String & path) {
  String sPath = path;
#ifdef DEBUG
  DBG_OUTPUT_PORT.println(String("handleFileRead: ") + sPath);  
#endif
  if (!fsOK) {
    replyServerError(FPSTR(FS_INIT_ERROR));
    return true;
  }

  if (sPath.endsWith("/")) {
    if (!fileSystem->exists("/index.html")) {
      if (fileSystem->exists("/www/index.html")) sPath += "www/index.html";
    } else sPath += "index.htm";
    // sPath += "www/index.html";
  }

  String contentType;
  if (server.hasArg("download")) {
    contentType = F("application/octet-stream");
  } else {
    contentType = mime::getContentType(sPath);
  }

  if (!fileSystem->exists(sPath)) {
    // File not found, try gzip version
    sPath = sPath + ".gz";
  }
  if (fileSystem->exists(sPath)) {
    File file = fileSystem->open(sPath, "r");
    if (server.streamFile(file, contentType) != file.size()) {
#ifdef DEBUG
      DBG_OUTPUT_PORT.println("Sent less data than expected!");  
#endif
    }
    file.close();
    return true;
  }

  return false;
}


/*
   As some FS (e.g. LittleFS) delete the parent folder when the last child has been removed,
   return the path of the closest parent still existing
*/
String lastExistingParent(const String & path) {
  String sPath = path;
  while (!sPath.isEmpty() && !fileSystem->exists(sPath)) {
    if (sPath.lastIndexOf('/') > 0) {
      sPath = sPath.substring(0, sPath.lastIndexOf('/'));
    } else {
      sPath = String();  // No slash => the top folder does not exist
    }
  }
#ifdef DEBUG
  DBG_OUTPUT_PORT.println(String("Last existing parent: ") + sPath);  
#endif
  return sPath;
}

/*
   Handle the creation/rename of a new file
   Operation      | req.responseText
   ---------------+--------------------------------------------------------------
   Create file    | parent of created file
   Create folder  | parent of created folder
   Rename file    | parent of source file
   Move file      | parent of source file, or remaining ancestor
   Rename folder  | parent of source folder
   Move folder    | parent of source folder, or remaining ancestor
*/
void espwebServer::handleFileCreate() {
  if (!fsOK) {
    return replyServerError(FPSTR(FS_INIT_ERROR));
  }

  String path = server.arg("path");
  if (path.isEmpty()) {
    return replyBadRequest(F("PATH ARG MISSING"));
  }

#ifdef USE_SPIFFS
  if (checkForUnsupportedPath(path).length() > 0) {
    return replyServerError(F("INVALID FILENAME"));
  }
#endif

  if (path == "/") {
    return replyBadRequest("BAD PATH");
  }
  if (fileSystem->exists(path)) {
    return replyBadRequest(F("PATH FILE EXISTS"));
  }

  String src = server.arg("src");
  if (src.isEmpty()) {
    // No source specified: creation
#ifdef DEBUG
    DBG_OUTPUT_PORT.println(String("handleFileCreate: ") + path);  
#endif
    if (path.endsWith("/")) {
      // Create a folder
      path.remove(path.length() - 1);
      if (!fileSystem->mkdir(path)) {
        return replyServerError(F("MKDIR FAILED"));
      }
    } else {
      // Create a file
      File file = fileSystem->open(path, "w");
      if (file) {
        file.write((const char *)0);
        file.close();
      } else {
        return replyServerError(F("CREATE FAILED"));
      }
    }
    if (path.lastIndexOf('/') > -1) {
      path = path.substring(0, path.lastIndexOf('/'));
    }
    replyOKWithMsg(path);
  } else {
    // Source specified: rename
    if (src == "/") {
      return replyBadRequest("BAD SRC");
    }
    if (!fileSystem->exists(src)) {
      return replyBadRequest(F("SRC FILE NOT FOUND"));
    }

#ifdef DEBUG
    DBG_OUTPUT_PORT.println(String("handleFileCreate: ") + path + " from " + src);  
#endif

    if (path.endsWith("/")) {
      path.remove(path.length() - 1);
    }
    if (src.endsWith("/")) {
      src.remove(src.length() - 1);
    }
    if (!fileSystem->rename(src, path)) {
      return replyServerError(F("RENAME FAILED"));
    }
    replyOKWithMsg(lastExistingParent(src));
  }
}


/*
   Delete the file or folder designed by the given path.
   If it's a file, delete it.
   If it's a folder, delete all nested contents first then the folder itself

   IMPORTANT NOTE: using recursion is generally not recommended on embedded devices and can lead to crashes (stack overflow errors).
   This use is just for demonstration purpose, and FSBrowser might crash in case of deeply nested filesystems.
   Please don't do this on a production system.
*/
void deleteRecursive(const String & path) {
  File file = fileSystem->open(path, "r");
  bool isDir = file.isDirectory();
  file.close();

  // If it's a plain file, delete it
  if (!isDir) {
    fileSystem->remove(path);
    return;
  }

  // Otherwise delete its contents first
  Dir dir = fileSystem->openDir(path);

  while (dir.next()) {
    deleteRecursive(path + '/' + dir.fileName());
  }

  // Then delete the folder itself
  fileSystem->rmdir(path);
}


/*
   Handle a file deletion request
   Operation      | req.responseText
   ---------------+--------------------------------------------------------------
   Delete file    | parent of deleted file, or remaining ancestor
   Delete folder  | parent of deleted folder, or remaining ancestor
*/
void espwebServer::handleFileDelete() {
  if (!fsOK) {
    return replyServerError(FPSTR(FS_INIT_ERROR));
  }

  String path = server.arg(0);
  if (path.isEmpty() || path == "/") {
    return replyBadRequest("BAD PATH");
  }

#ifdef DEBUG
  DBG_OUTPUT_PORT.println(String("handleFileDelete: ") + path);  
#endif
  if (!fileSystem->exists(path)) {
    return replyNotFound(FPSTR(FILE_NOT_FOUND));
  }
  deleteRecursive(path);

  replyOKWithMsg(lastExistingParent(path));
}

/*
   Handle a file upload request
*/
void espwebServer::handleFileUpload() {
  if (!fsOK) {
    return replyServerError(FPSTR(FS_INIT_ERROR));
  }
  if (server.uri() != "/edit") {
    return;
  }
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    // Make sure paths always start with "/"
    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }
#ifdef DEBUG
    DBG_OUTPUT_PORT.println(String("handleFileUpload Name: ") + filename);  
#endif
    uploadFile = fileSystem->open(filename, "w");
    if (!uploadFile) {
      return replyServerError(F("CREATE FAILED"));
    }
#ifdef DEBUG
    DBG_OUTPUT_PORT.println(String("Upload: START, filename: ") + filename);  
#endif
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      size_t bytesWritten = uploadFile.write(upload.buf, upload.currentSize);
      if (bytesWritten != upload.currentSize) {
        return replyServerError(F("WRITE FAILED"));
      }
    }
#ifdef DEBUG
    DBG_OUTPUT_PORT.println(String("Upload: WRITE, Bytes: ") + upload.currentSize);  
#endif
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
    }
#ifdef DEBUG
    DBG_OUTPUT_PORT.println(String("Upload: END, Size: ") + upload.totalSize);  
#endif
  }
}


/*
   The "Not Found" handler catches all URI not explicitely declared in code
   First try to find and return the requested file from the filesystem,
   and if it fails, return a 404 page with debug information
*/
void espwebServer::handleNotFound() {
  if (!fsOK) {
    return replyServerError(FPSTR(FS_INIT_ERROR));
  }

  String uri = ESP8266WebServer::urlDecode(server.uri()); // required to read paths with blanks

  if (handleFileRead(uri)) {
    return;
  }

  // Dump debug data
  String message;
  message.reserve(100);
  message = F("Error: File not found\n\nURI: ");
  message += uri;
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += server.args();
  message += '\n';
  for (uint8_t i = 0; i < server.args(); i++) {
    message += F(" NAME:");
    message += server.argName(i);
    message += F("\n VALUE:");
    message += server.arg(i);
    message += '\n';
  }
  message += "path=";
  message += server.arg("path");
  message += '\n';
#ifdef DEBUG
  DBG_OUTPUT_PORT.print(message);  
#endif

  return replyNotFound(message);
}

/*
   This specific handler returns the index.htm (or a gzipped version) from the /edit folder.
   If the file is not present but the flag INCLUDE_FALLBACK_INDEX_HTM has been set, falls back to the version
   embedded in the program code.
   Otherwise, fails with a 404 page with debug information
*/
void espwebServer::handleGetEdit() {
  if (handleFileRead(F("/www/edit/index.htm"))) {
    return;
  }

#ifdef INCLUDE_FALLBACK_INDEX_HTM
  server.sendHeader(F("Content-Encoding"), "gzip");
  server.send(200, "text/html", index_htm_gz, index_htm_gz_len);
#else
  replyNotFound(FPSTR(FILE_NOT_FOUND));
#endif

}  

#else
  void espwebServer::handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }

    server.send(404, "text/plain", message);
  }
#endif



// void loop(void) {
//   server.handleClient();
//   MDNS.update();
// }
