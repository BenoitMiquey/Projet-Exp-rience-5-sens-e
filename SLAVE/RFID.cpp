#include <Arduino.h>
#include "Slave.h"
#include <SlaveSeeedRFID.h> //attention: lib mofifiée par JNL, ajout méthode listen()

extern SoftwareSerial SmartAlpha;
SeeedRFID RFID(RFID_RX_PIN, RFID_POWER);
RFIDdata tag;

long ScanRFID( int Channel)
{
  long TagNumber = 0;
  RFID.listen();
  CD4051addr(Channel);
  digitalWrite(CD4051_I, LOW);
  delay(150);
  if (RFID.isAvailable())
  {
    tag = RFID.data();
    TagNumber = RFID.cardNumber();
  }
  digitalWrite(CD4051_I, HIGH);
  SmartAlpha.listen();
  delay(10);
  return (TagNumber);
}
