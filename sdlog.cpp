#include "sdlog.h"
#include <SPI.h>
#include <SD.h>

#define SD_SCK 5
#define SD_MISO 36
#define SD_MOSI 4
#define SD_CS 26

SPIClass sdSPI(VSPI);
static bool ready = false;
static File logFile;

bool sdInit() {
  sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS, sdSPI)) return false;

  int n = 0;
  char path[16];
  do {
    snprintf(path, sizeof(path), "/log%d.csv", n++);
  } while (SD.exists(path) && n < 1000);

  logFile = SD.open(path, FILE_WRITE);
  if (!logFile) return false;
  logFile.println("ms,roll,pitch,alt,thr,healthy");
  logFile.flush();
  ready = true;
  return true;
}

void sdLog(float roll, float pitch, float alt, float thr, bool healthy) {
  if (!ready) return;
  logFile.printf("%lu,%.1f,%.1f,%.1f,%d,%d\n",
              millis(), roll, pitch, alt, thr, healthy ? 1 : 0);
  static uint32_t lastFlush = 0;
  if (millis() - lastFlush > 1000) {
    lastFlush = millis();
    logFile.flush();
  }
}