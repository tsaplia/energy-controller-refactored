#include "wifi-tools.h"
#include <ArduinoOTA.h>
#include "constants.h"
#include "globals.h"

DNSServer dnsServer;
bool APConnected = false;

/* Start an Access Point */
bool startWiFiAP() {
    WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0));
    bool result = WiFi.softAP(AP_SSID, AP_PASSWORD);
    APConnected = result;
    if (result) {
        logger.info("AP started with IP: " + WiFi.softAPIP().toString());
        dnsServer.start(DNS_PORT, "*", AP_IP);
    } else {
        logger.error("Failed to start AP");
    }
    return result;
}

/* Try to connect to a WiFi network with new credentials */
bool changeWiFiSTA(String& ssid, String& password) {
    startWiFiSTA(ssid, password);

    if (WiFi.status() == WL_CONNECTED) {
        logger.info("WiFi credentials updated");
        configs.ssid = ssid;
        configs.password = password;
        configs.save();
        return true;
    }
    logger.warning("Failed to connect to " + ssid);
    logger.info("Reverting to previous credentials");
    WiFi.begin(configs.ssid, configs.password);
    return false;
}

/* Connect to a WiFi network */
void startWiFiSTA(String& ssid, String& password) {
    WiFi.begin(ssid, password);
    logger.info("Connecting to " + ssid);
    for(int tries = 0; WiFi.status() != WL_CONNECTED && tries < WIFI_CONNECT_TRIES; tries++){
        delay(WIFI_CONNECT_DELAY);
        logger.debug("Attempt " + String(tries + 1));
    }
    if (WiFi.status() == WL_CONNECTED) {
        logger.info("WiFi connected on IP: " + WiFi.localIP().toString());
    }
}

/* Disconnect from an Access Point */
bool disconnectAP() {
    logger.info("Disconnecting Access Point");
    dnsServer.stop();
    bool result = WiFi.softAPdisconnect(true);
    if(!result) logger.warning("Failed to disconnect from Access Point");
    APConnected = false;
    return result;
}

/* Setup Arduino OTA  for WiFi firmware updates */
void setupOTA() {
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPort(OTA_PORT);

    ArduinoOTA.onEnd([]() {
        logger.info("OTA End");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        logger.debug("OTA Progress: " + String((progress / (total / 100))) + "%");
    });
    ArduinoOTA.onError([](ota_error_t error) {
        String msg = "OTA Error[" + String(error) + "]: ";
        if (error == OTA_AUTH_ERROR) msg += "Auth Failed";
        else if (error == OTA_BEGIN_ERROR) msg += "Begin Failed";
        else if (error == OTA_CONNECT_ERROR) msg += "Connect Failed";
        else if (error == OTA_RECEIVE_ERROR) msg += "Receive Failed";
        else if (error == OTA_END_ERROR) msg += "End Failed";
        logger.error(msg);
    });
    ArduinoOTA.onStart([]() {
        logger.info("OTA Start");
    });
}
