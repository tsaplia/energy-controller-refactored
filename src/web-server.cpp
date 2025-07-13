#include <LittleFS.h>
#include "web-server.h"
#include "constants.h"

ESP8266WebServer web_server(WEB_PORT);

void handleNotFoundWeb();
bool handleFileRead(String path);
String getContentType(String filename);

void setupWebServer() {
    web_server.onNotFound(handleNotFoundWeb);
}

void handleNotFoundWeb() {
    if(!handleFileRead(web_server.uri())) {
        web_server.send(404, "text/plain", "{\"error\": \"Not Found\"}");
    }
}

/* send the right file to the client returns true if file exists false otherwise */
bool handleFileRead(String path){ 
    if(path.endsWith("/")) path += "index.html"; 
    if(path.indexOf(".") == -1) path += ".html"; 

    String contentType = getContentType(path);
    if(LittleFS.exists(path)){
        File file = LittleFS.open(path, "r");
        web_server.streamFile(file, contentType);
        file.close();

        return true;
    }
    return false;
}

String getContentType(String filename){
    if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".csv")) return "text/csv";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".png")) return "image/png";
    else if (filename.endsWith(".jpg")) return "image/jpeg";
    else if (filename.endsWith(".gif")) return "image/gif";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    return "text/plain";
}
