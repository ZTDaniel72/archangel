#pragma once
#include <Arduino.h>
void webuiInit();
void webuiUpdate();
void webuiSet(float roll, float pitch, float alt, int thr, bool healthy);