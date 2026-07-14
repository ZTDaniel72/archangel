#include "outputs.h"
#include "config.h"
#include "ESP32Servo.h"

static Servo sAilL, sAilR, sElev, sEsc;

static uint16_t clampPulse(uint16_t v) {
    if (v < PULSE_MIN) return PULSE_MIN;
    if (v > PULSE_MAX) return PULSE_MAX;
    return v;
}

void outputsInit(){
    sAilL.attach(PIN_SERVO_AIL_L, PULSE_MIN, PULSE_MAX);
    sAilR.attach(PIN_SERVO_AIL_R, PULSE_MIN, PULSE_MAX);
    sElev.attach(PIN_SERVO_ELEV, PULSE_MIN, PULSE_MAX);
    sEsc.attach(PIN_ESC, PULSE_MIN, PULSE_MAX);
    outputsFailsafe();
}

void outputsWrite(uint16_t ailL, uint16_t ailR, uint16_t elev, uint16_t throttle) {
    sAilL.writeMicroseconds(clampPulse(ailL));
    sAilR.writeMicroseconds(clampPulse(ailR));
    sElev.writeMicroseconds(clampPulse(elev));
    sEsc.writeMicroseconds(clampPulse(throttle));
}

void outputsFailsafe() {
    outputsWrite(FAILSAFE_SURFACE, FAILSAFE_SURFACE, FAILSAFE_SURFACE, FAILSAFE_THROTTLE);
}