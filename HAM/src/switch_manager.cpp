#include <Arduino.h>

#include "config.h"

volatile bool switchTriggered = false;

unsigned long lastInterruptTime = 0;

unsigned long ignoreInterruptUntil = 0;

void IRAM_ATTR handleSwitchInterrupt() {

    unsigned long currentTime = millis();

    if(currentTime < ignoreInterruptUntil) {
        return;
    }

    if(currentTime - lastInterruptTime > 300) {

        switchTriggered = true;

        lastInterruptTime = currentTime;
    }
}

void initSwitch() {

    pinMode(SWITCH_PIN, INPUT_PULLDOWN);

    attachInterrupt(
        digitalPinToInterrupt(SWITCH_PIN),
        handleSwitchInterrupt,
        FALLING
    );
}
