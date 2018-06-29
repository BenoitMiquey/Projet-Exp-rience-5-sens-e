#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Slave.h"
#include "IfDef.h"

byte DestAddr[] = {0x30, 0x30, 0x30, 0x4D};
byte MyAddr[] = {0x30, 0x30, 0x30, 0x30};
//byte DestAddr[] = {0x7E, 0x7E, 0x7E, 0x7E};
//byte MyAddr[] = {0x7E, 0x7E, 0x7E, 0x7E};
byte ConfigRF[] = {0x06, 0x75, 0x00}; //dernier octet pour regler puissance emission

SoftwareSerial SmartAlpha(2, 3); // RX, TX

int calcrc(char *ptr, int count);

//***************************************************************
void SmartAlphaFlush()
{
  char Dummy;
  while (SmartAlpha.available())
  {
    Dummy = SmartAlpha.read();
  }
}

//***************************************************************
void SmartAlphaSend(String Command)
{
  //WARNING: trames de 64 octets maxi (taille du buffer)
  unsigned short CRC ;
 
  String CRCstring = "";
  String Trame = "";
  int Temp;

  CRC = calcrc(Command.c_str(), Command.length());
  Trame = "<" + Command;      // 1 chr
  Trame += "|";               // 1 chr
  Trame += String(CRC, HEX);  // 4 chr
  Trame += ">";               // 1chr
  // Total = 7 chr
  // buffer capacity =64 bytes - 7 = 57

  while (digitalRead(SMARTALPHA_CTS))
  {
    Serial.println("Wait.....");
     //attente module RF libre
  }
  delay(20);
  SmartAlpha.print(Trame);
  SmartAlpha.write('\r');
  SmartAlpha.write('\n');
}
//**************************************************************
void SmartAlphaAddr(char Slave)
{
  Serial.print("ADDR: ");
  SmartAlpha.print("+++");
  delay(18);
  SmartAlpha.write(DestAddr[0]);
  SmartAlpha.write(DestAddr[1]);
  SmartAlpha.write(DestAddr[2]);
  SmartAlpha.write(DestAddr[3]);
  SmartAlpha.write(MyAddr[0]);
  SmartAlpha.write(MyAddr[1]);
  SmartAlpha.write(MyAddr[2]);
  SmartAlpha.write(MyAddr[3]);
  //SmartAlpha.write(Slave);

  delay(80);
  SmartAlpha.print("---");
  delay(20);
  while (SmartAlpha.available())
  {
    Serial.write( SmartAlpha.read());
  }
  Serial.println();



}
//************************************************
int calcrc(char *ptr, int count)
{
  int  crc;
  char i;
  crc = 0;
  while (--count >= 0)
  {
    crc = crc ^ (int) * ptr++ << 8;
    i = 8;
    do
    {
      if (crc & 0x8000)
        crc = crc << 1 ^ 0x1021;
      else
        crc = crc << 1;
    } while (--i);
  }
  return (crc);
}

/*
  void SmartAlphaReinit(void)
  {
  byte  Value=0x00;
  Serial.print("reinit ");
  SmartAlpha.print("+++");
  delay(20);
  SmartAlpha.write(Value);
  SmartAlpha.write(Value);
  SmartAlpha.write(Value);
  SmartAlpha.write(Value);
  SmartAlpha.write(Value);
  SmartAlpha.write(Value);
  SmartAlpha.write(Value);
  SmartAlpha.write(Value);
  delay(80);
  SmartAlpha.print("---");
  delay(20);
  Serial.println("Done!");
  }*/
