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

  uint16_t roll  = rxGet(RX_ROLL);
  uint16_t pitch = rxGet(RX_PITCH);
  uint16_t thr   = rxGet(RX_THROTTLE);
  uint16_t aux   = rxGet(RX_AUX);
  uint16_t rudd  = rxGet(RX_RUDD);

  uint16_t ailL, ailR, ruddL, ruddR;

  if (rxHealthy()) {
    float rate = (aux > PULSE_MID) ? RATE_SPORT : RATE_GENTLE;
    roll  = applyRate(roll,  rate);
    pitch = applyRate(pitch, rate);
    rudd  = applyRate(rudd,  rate);

    // V-tail mixer: both surfaces carry pitch; rudder deflects them
    // in opposite directions.
    int ruddDefl = (int)rudd - PULSE_MID;
    ruddL = (uint16_t)((int)pitch + ruddDefl);
    ruddR = (uint16_t)((int)pitch - ruddDefl);

    ailL = roll;
    ailR = (uint16_t)(2 * PULSE_MID - roll);

    outputsWrite(ailL, ailR, ruddL, ruddR, thr);
  } else {
    ailL = ailR = ruddL = ruddR = FAILSAFE_SURFACE;
    thr = FAILSAFE_THROTTLE;
    outputsFailsafe();
  }

  static uint32_t lastPrint = 0;
  if (millis() - lastPrint > 100) {
    lastPrint = millis();
    Serial.printf("ABN,%u,%u,%u,%u,%u,%u,%u,%u,%u,%d\n",
                  roll, pitch, thr, rudd, aux,
                  ailL, ailR, ruddL, ruddR, (int)rxHealthy());
  }
}
