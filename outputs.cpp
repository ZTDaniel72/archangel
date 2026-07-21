#include "outputs.h"
#include "config.h"
#include "ESP32Servo.h"

static Servo sAilL, sAilR, sRuddL, sRuddR, sEsc;

static uint16_t clampPulse(uint16_t v) {
    if (v < PULSE_MIN) return PULSE_MIN;
    if (v > PULSE_MAX) return PULSE_MAX;
    return v;
}

void outputsInit() {
    sAilL.attach(PIN_SERVO_AIL_L,   PULSE_MIN, PULSE_MAX);
    sAilR.attach(PIN_SERVO_AIL_R,   PULSE_MIN, PULSE_MAX);
    sRuddL.attach(PIN_SERVO_RUDD_L, PULSE_MIN, PULSE_MAX);
    sRuddR.attach(PIN_SERVO_RUDD_R, PULSE_MIN, PULSE_MAX);
    sEsc.attach(PIN_ESC,            PULSE_MIN, PULSE_MAX);
    outputsFailsafe();
}

void outputsWrite(uint16_t ailL, uint16_t ailR,
                  uint16_t ruddL, uint16_t ruddR, uint16_t throttle) {
    sAilL.writeMicroseconds(clampPulse(ailL));
    sAilR.writeMicroseconds(clampPulse(ailR));
    sRuddL.writeMicroseconds(clampPulse(ruddL));
    sRuddR.writeMicroseconds(clampPulse(ruddR));
    sEsc.writeMicroseconds(clampPulse(throttle));
}

void outputsFailsafe() {
    outputsWrite(FAILSAFE_SURFACE, FAILSAFE_SURFACE,
                 FAILSAFE_SURFACE, FAILSAFE_SURFACE, FAILSAFE_THROTTLE);
}