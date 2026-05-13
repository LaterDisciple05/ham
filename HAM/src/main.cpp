#include <Arduino.h>

#include "relay_manager.h"
#include "switch_manager.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "webserver_manager.h"

void setup() {

    Serial.begin(115200);

    initRelay();

    initSwitch();

    initWiFi();

    initWebServer();

    initMQTT();

    Serial.println("System Ready");
}

void loop() {

    checkWiFi();

    handleWebServer();

    mqttLoop();

    if(switchTriggered) {

        switchTriggered = false;

        toggleRelay();

        publishRelayStatus();
    }
}