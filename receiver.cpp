#include "receiver.h"
#include "config.h"

static const uint8_t rxPins[RX_CHANNEL_COUNT] = {
    PIN_RX_ROLL, PIN_RX_PITCH, PIN_RX_THROTTLE, PIN_RX_AUX
};

static volatile uint32_t riseTime[RX_CHANNEL_COUNT];
static volatile uint16_t pulseWidth[RX_CHANNEL_COUNT];
static volatile uint32_t lastPulseMs = 0;

template <int CH>
void IRAM_ATTR isrChannel() {
    if (digitalRead(rxPins[CH])) {
        riseTime[CH] = micros();
    } else {
        uint32_t w = micros() - riseTime[CH];
        if (w >= 800 && w <= 2200) {
            pulseWidth[CH] = (uint16_t)w;
            lastPulseMs = millis();
        }
    }
}

void rxInit() {
    for (int i = 0; i < RX_CHANNEL_COUNT; i++) pinMode(rxPins[i], INPUT);
    attachInterrupt(digitalPinToInterrupt(rxPins[RX_ROLL]), isrChannel<RX_ROLL>, CHANGE);
    attachInterrupt(digitalPinToInterrupt(rxPins[RX_PITCH]), isrChannel<RX_PITCH>, CHANGE);
    attachInterrupt(digitalPinToInterrupt(rxPins[RX_THROTTLE]), isrChannel<RX_THROTTLE>, CHANGE);
    attachInterrupt(digitalPinToInterrupt(rxPins[RX_AUX]), isrChannel<RX_AUX>, CHANGE);
}

uint16_t rxGet(RxChannel ch) {
    noInterrupts();
    uint16_t v = pulseWidth[ch];
    interrupts();
    return v;
}

bool rxHealthy() {
    noInterrupts();
    uint32_t last = lastPulseMs;
    interrupts();
    return (millis() - last) < FAILSAFE_TIMEOUT_MS;
}