#pragma once
#include <Arduino.h>

enum RxChannel {RX_ROLL = 0, RX_PITCH, RX_THROTTLE, RX_AUX, RX_CHANNEL_COUNT};

void rxInit();

uint16_t rxGet(RxChannel ch);

bool rxHealthy();