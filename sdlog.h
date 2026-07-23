#pragma once
#include <Arduino.h>
bool sdInit();
void sdLog(float roll, float pitch, float alt, float thr, bool healthy);
