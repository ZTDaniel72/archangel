#pragma once
#include <Arduino.h>

enum RxChannel { RX_ROLL = 0, RX_PITCH, RX_THROTTLE, RX_AUX, RX_RUDD, RX_CHANNEL_COUNT };

void rxInit();

uint32_t rxIsrCount();
uint16_t rxGet(RxChannel ch);

bool rxHealthy();