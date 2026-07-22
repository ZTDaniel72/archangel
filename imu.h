#pragma once
#include <Arduino.h>

void  imuInit();  
void  imuUpdate(); 
float imuRoll();   
float imuPitch();
float imuAlt();
float imuRollRate();
float imuPitchRate();