#include "loralink.h"
#include <SPI.h>
#include <LoRa.h>

extern SPIClass sdSPI;    

#define LORA_CS   2
#define LORA_RST  15
#define LORA_DIO0 39
#define LORA_FREQ 868E6

static bool ready = false;

bool loraInit() {
  LoRa.setSPI(sdSPI);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(LORA_FREQ)) return false;
  LoRa.setSpreadingFactor(7);   
  LoRa.setSignalBandwidth(125E3);
  ready = true;
  return true;
}

void loraSend(float roll, float pitch, float alt, int thr, bool healthy) {
  if (!ready) return;
  if (LoRa.beginPacket() == 0) return;   
  LoRa.print(roll, 1);   LoRa.print(',');
  LoRa.print(pitch, 1);  LoRa.print(',');
  LoRa.print(alt, 1);    LoRa.print(',');
  LoRa.print(thr);       LoRa.print(',');
  LoRa.print(healthy ? 1 : 0);
  LoRa.endPacket(true);                   
}