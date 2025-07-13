#include <LittleFS.h>
#include "ap-server.h"
#include "constants.h"

ESP8266WebServer ap_server(WEB_PORT);

void handleNotFoundAp();
void handleAuth();

void setupApServer() {
    ap_server.on("/", handleAuth);
    ap_server.onNotFound(handleNotFoundAp);
}

void handleNotFoundAp() {
    ap_server.sendHeader("Location", "/", true);
    ap_server.send(302, "text/plain", "");
}

void handleAuth(){ 
    const String authFile = "/auth.html";

    if(LittleFS.exists(authFile)){
        File file = LittleFS.open(authFile, "r");
        ap_server.streamFile(file, "text/html");
        file.close();
    } else {
        ap_server.send(404, "text/plain", "Not Found");
    }
}