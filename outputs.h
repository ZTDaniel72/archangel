#pragma once
#include <Arduino.h>

void outputsInit();
void outputsWrite(uint16_t ailL, uint16_t ailR, uint16_t ruddL, uint16_t ruddR, uint16_t throttle);
void outputsFailsafe();