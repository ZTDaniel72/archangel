#include "led.h"
#include "config.h"

void ledInit() {
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  ledSet(0,0,0);
}

void ledSet(uint8_t r, uint8_t g, uint8_t b) {
#ifdef LED_COMMON_ANODE
 digitalWrite(PIN_LED_R, r ? LOW : HIGH);
 digitalWrite(PIN_LED_G, g ? LOW : HIGH);
 digitalWrite(PIN_LED_B, b ? LOW : HIGH);
#else
  digitalWrite(PIN_LED_R, r ? HIGH : LOW);
  digitalWrite(PIN_LED_G, g ? HIGH : LOW);
  digitalWrite(PIN_LED_B, b ? HIGH : LOW);
#endif
}