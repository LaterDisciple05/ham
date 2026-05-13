#include <Arduino.h>
#include <WiFi.h>

#include "wifi_manager.h"
#include "config.h"

void initWiFi() {

    Serial.println("Connecting WiFi");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while(WiFi.status() != WL_CONNECTED) {

        delay(500);

        Serial.print(".");
    }

    Serial.println();

    Serial.println("WiFi Connected");

    Serial.println(WiFi.localIP());
}

void checkWiFi() {

    if(WiFi.status() != WL_CONNECTED) {

        Serial.println("WiFi Lost");

        WiFi.disconnect();

        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
}