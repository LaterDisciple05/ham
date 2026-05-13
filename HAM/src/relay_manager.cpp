#include <Arduino.h>
#include "relay_manager.h"

#include "config.h"

static bool relayState = false;

// =====================================================
// APPLY RELAY STATE
// =====================================================

void applyRelayState() {

    bool gpioState;

    if(RELAY_ACTIVE_HIGH) {

        gpioState = relayState;
    }

    else {

        gpioState = !relayState;
    }

    digitalWrite(RELAY_PIN, gpioState);
}

// =====================================================
// INIT
// =====================================================

void initRelay() {

    pinMode(RELAY_PIN, OUTPUT);

    applyRelayState();
}

// =====================================================
// SET RELAY
// =====================================================

void setRelay(bool state) {

    relayState = state;

    applyRelayState();
}

// =====================================================
// TOGGLE RELAY
// =====================================================

void toggleRelay() {

    relayState = !relayState;

    applyRelayState();
}

// =====================================================
// GET STATE
// =====================================================

bool getRelayState() {

    return relayState;
}