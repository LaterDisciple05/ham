#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#include "mqtt_manager.h"
#include "relay_manager.h"
#include "config.h"

WiFiClientSecure espClient;

PubSubClient mqttClient(espClient);

// =====================================================
// CALLBACK
// =====================================================

void mqttCallback(
    char* topic,
    byte* payload,
    unsigned int length
) {

    String message;

    for(int i = 0; i < length; i++) {

        message += (char)payload[i];
    }

    Serial.print("Message: ");

    Serial.println(message);

    if(message == "ON") {

        setRelay(true);

        publishRelayStatus();
    }

    else if(message == "OFF") {

        setRelay(false);

        publishRelayStatus();
    }
}

// =====================================================
// MQTT CONNECT
// =====================================================

void reconnectMQTT() {

    while(!mqttClient.connected()) {

        Serial.println("Connecting MQTT...");

        String clientId = "ESP32Client-";

        clientId += String(random(0xffff), HEX);

        if(
            mqttClient.connect(
                clientId.c_str(),
                MQTT_USERNAME,
                MQTT_PASSWORD
            )
        ) {

            Serial.println("MQTT Connected");

            mqttClient.subscribe(
                "home/room1/light1/set"
            );
        }

        else {

            Serial.print("Failed: ");

            Serial.println(
                mqttClient.state()
            );

            delay(2000);
        }
    }
}

// =====================================================
// INIT
// =====================================================

void initMQTT() {

    espClient.setInsecure();

    mqttClient.setServer(
        MQTT_SERVER,
        8883
    );

    mqttClient.setCallback(
        mqttCallback
    );
}

// =====================================================
// LOOP
// =====================================================

void mqttLoop() {

    if(!mqttClient.connected()) {

        reconnectMQTT();
    }

    mqttClient.loop();
}

// =====================================================
// STATUS
// =====================================================

void publishRelayStatus() {

    if(getRelayState()) {

        mqttClient.publish(
            "MQTT_TOPIC_STATUS",
            "ON"
        );
    }

    else {

        mqttClient.publish(
            "MQTT_TOPIC_STATUS",
            "OFF"
        );
    }
}