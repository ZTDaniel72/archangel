#include "config.h"
#include "receiver.h"
#include "outputs.h"

void setup() {
    Serial.begin(115200);
    rxInit();
    outputsInit();
    Serial.println("online, ready for start");
}

static uint16_t applyRate(uint16_t pulse, float rate) {
    int centered = (int)pulse - PULSE_MID;
    return (uint16_t)(PULSE_MID + centered * rate);
}

void loop() {
    static uint32_t lastRun = 0;
    if (millis() - lastRun < (1000 / LOOP_HZ)) return;
    lastRun = millis();

    if (!rxHealthy()) {
        outputsFailsafe();
        return;
    }

    uint16_t roll = rxGet(RX_ROLL);
    uint16_t pitch = rxGet(RX_PITCH);
    uint16_t thr = rxGet(RX_THROTTLE);
    uint16_t aux = rxGet(RX_AUX);

    float rate = (aux > PULSE_MID) ? RATE_SPORT : RATE_GENTLE;
    roll = applyRate(roll, rate);
    pitch = applyRate(pitch, rate);
    uint16_t ailL = roll;
    uint16_t ailR = (uint16_t)(2 * PULSE_MID - roll);

    outputsWrite(ailL, ailR, pitch, thr);
}