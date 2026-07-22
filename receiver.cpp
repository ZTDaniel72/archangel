#include "receiver.h"
#include "config.h"

// Synchronous pulse measurement using pulseIn().
// Slower than interrupts but rock-solid. Good enough for bench + Milestone 1.

static const uint8_t rxPins[RX_CHANNEL_COUNT] = {
    PIN_RX_ROLL, PIN_RX_PITCH, PIN_RX_THROTTLE, PIN_RX_AUX, PIN_RX_RUDD
};

static uint16_t lastGood[RX_CHANNEL_COUNT] = {0, 0, 0, 0};
static uint32_t lastPulseMs = 0;
static uint32_t lastRawW = 0;   // last width measured, unfiltered (debug)

void rxInit() {
    for (int i = 0; i < RX_CHANNEL_COUNT; i++) {
        pinMode(rxPins[i], INPUT_PULLDOWN);
    }
}

uint16_t rxGet(RxChannel ch) {
    // Wait for one HIGH pulse on this pin; timeout 25 ms (~one PWM frame).
    uint32_t w = pulseIn(rxPins[ch], HIGH, 25000UL);
    lastRawW = w;
    if (w >= 800 && w <= 2200) {
        lastGood[ch] = (uint16_t)w;
        lastPulseMs = millis();
    }
    return lastGood[ch];
}

bool rxHealthy() {
    return (millis() - lastPulseMs) < FAILSAFE_TIMEOUT_MS;
}

// kept so the debug print still compiles; now reports last raw width
uint32_t rxIsrCount() { return lastRawW; }
