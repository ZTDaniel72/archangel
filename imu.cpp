#include "imu.h"
#include <Wire.h>
#include <math.h>


#define IMU_ADDR   0x68
#define IMU_SDA    16
#define IMU_SCL    17
static const float FILTER_A = 0.98f; 


static float rollF = 0, pitchF = 0;
static float gxBias = 0, gyBias = 0, gzBias = 0;
static uint32_t lastUs = 0;

static void readRaw(int16_t &ax, int16_t &ay, int16_t &az,
                    int16_t &gx, int16_t &gy, int16_t &gz) {
  Wire.beginTransmission(IMU_ADDR);
  Wire.write(0x3B);                
  Wire.endTransmission(false);
  Wire.requestFrom(IMU_ADDR, 14, true);
  ax = (Wire.read() << 8) | Wire.read();
  ay = (Wire.read() << 8) | Wire.read();
  az = (Wire.read() << 8) | Wire.read();
  Wire.read(); Wire.read();   
  gx = (Wire.read() << 8) | Wire.read();
  gy = (Wire.read() << 8) | Wire.read();
  gz = (Wire.read() << 8) | Wire.read();
}

void imuInit() {
  Wire.begin(IMU_SDA, IMU_SCL);
  Wire.beginTransmission(IMU_ADDR);
  Wire.write(0x6B); Wire.write(0x00);   
  Wire.endTransmission();
  delay(100);


  long sx = 0, sy = 0, sz = 0;
  const int N = 1000;
  for (int i = 0; i < N; i++) {
    int16_t ax, ay, az, gx, gy, gz;
    readRaw(ax, ay, az, gx, gy, gz);
    sx += gx; sy += gy; sz += gz;
    delay(2);
  }
  gxBias = sx / (float)N;
  gyBias = sy / (float)N;
  gzBias = sz / (float)N;


  int16_t ax, ay, az, gx, gy, gz;
  readRaw(ax, ay, az, gx, gy, gz);
  float fx = ax / 16384.0f, fy = ay / 16384.0f, fz = az / 16384.0f;
  rollF  = atan2(fy, fz) * 180.0f / PI;
  pitchF = atan2(-fx, sqrt(fy*fy + fz*fz)) * 180.0f / PI;

  lastUs = micros();
}

void imuUpdate() {
  int16_t ax, ay, az, gx, gy, gz;
  readRaw(ax, ay, az, gx, gy, gz);

  uint32_t now = micros();
  float dt = (now - lastUs) / 1000000.0f;
  lastUs = now;
  if (dt <= 0 || dt > 0.2f) dt = 0.01f;

  float fx = ax / 16384.0f, fy = ay / 16384.0f, fz = az / 16384.0f;
  float accRoll  = atan2(fy, fz) * 180.0f / PI;
  float accPitch = atan2(-fx, sqrt(fy*fy + fz*fz)) * 180.0f / PI;

  float gRoll  = (gx - gxBias) / 131.0f;
  float gPitch = (gy - gyBias) / 131.0f;

  rollF  = FILTER_A * (rollF  + gRoll  * dt) + (1 - FILTER_A) * accRoll;
  pitchF = FILTER_A * (pitchF + gPitch * dt) + (1 - FILTER_A) * accPitch;
}

float imuRoll()  { return rollF; }
float imuPitch() { return pitchF; }
