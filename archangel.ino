#include "config.h"
#include "receiver.h"
#include "outputs.h"
#include "imu.h"
#include "led.h"
#include "webui.h"

void setup() {
    Serial.begin(115200);
    rxInit();
    outputsInit();
    ledInit();
    ledSet(0,0,1);
    Serial.println("stay still, calibrating IMU...");
    imuInit();
    webuiInit();
    Serial.println("wifi up: connect to ARCHANGEL, open 192.168.4.1");
    Serial.println("online, ready for start");
}

static uint16_t applyRate(uint16_t pulse, float rate) {
    int centered = (int)pulse - PULSE_MID;
    return (uint16_t)(PULSE_MID + centered * rate);
}

static float pidStep(float target, float measured, float rate, float &integ) {
  float err = target - measured;
  integ += err * (1.0f / LOOP_HZ);
  integ = constrain(integ, -PID_ILIMIT, PID_ILIMIT);
  return PID_KP * err + PID_KI * integ - PID_KD * rate;
}

void loop() {
  webuiUpdate();
  static uint32_t lastRun = 0;
  if (millis() - lastRun < (1000 / LOOP_HZ)) return;
  lastRun = millis();
  imuUpdate();

  uint16_t roll  = rxGet(RX_ROLL);
  uint16_t pitch = rxGet(RX_PITCH);
  uint16_t thr   = rxGet(RX_THROTTLE);
  uint16_t aux   = rxGet(RX_AUX);
  uint16_t rudd  = rxGet(RX_RUDD);

  uint16_t ailL, ailR, ruddL, ruddR;

  static float rollInteg = 0, pitchInteg = 0;

  if (rxHealthy()) {
    
    rudd = applyRate(rudd, RATE_GENTLE);
    float rollTarget = ((int)roll - PULSE_MID) / 500.0f * PID_MAXANGLE;
    float pitchTarget = ((int)pitch - PULSE_MID) / 500.0f * PID_MAXANGLE;
    int rollOut = (int)pidStep(rollTarget, imuRoll(), imuRollRate(), rollInteg);
    int pitchOut = (int)pidStep(pitchTarget, imuPitch(), imuPitchRate(), pitchInteg);
    roll = (uint16_t)constrain(PULSE_MID + rollOut, PULSE_MIN, PULSE_MAX);
    pitch = (uint16_t)constrain(PULSE_MID + pitchOut, PULSE_MIN, PULSE_MAX);
    

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
  webuiSet(imuRoll(), imuPitch(), imuAlt(), thr, rxHealthy());
  if (!rxHealthy()) ledSet(0,1,0);
  else ledSet(1,0,0);

  static uint32_t lastPrint = 0;
  if (millis() - lastPrint > 100) {
    lastPrint = millis();
    Serial.printf("ABN,%u,%u,%u,%u,%u,%u,%u,%u,%u,%d,%.1f,%.1f, %.1f\n",
            roll, pitch, thr, rudd, aux,
            ailL, ailR, ruddL, ruddR, (int)rxHealthy(), imuRoll(), imuPitch(), imuAlt());
  }
}
