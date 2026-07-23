#pragma once
#include <Arduino.h>

bool loraInit();
void loraSend(float roll, float pitch, float alt, int thr, bool healthy);