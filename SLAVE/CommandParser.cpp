#include <arduino.h>
#include <SoftwareSerial.h>
#include "Slave.h"
#include "ifdef.h"

#define CR 0x0D
#define LF 0x0A
#define SPLITTERS 16 // nombre de délimiteurs MAX pris en compte
#define SPLIT ','
#define TRAME_START '<'
#define TRAME_END '>'

#define TO_UART 500
#define TO_SMARTALPHA 3

extern SoftwareSerial SmartAlpha;

String Commande[SPLITTERS];
int Param[SPLITTERS];
int SplitIndex[SPLITTERS];
int FlagCmd = 0;
extern int calcrc(char *ptr, int count);
String ReceivedString = "";
void String_Split(char Splitter);
void SmartAlphaFlush(void);

//****************************************************************
void SmartAlphaRxProcess()
{
  char RecievedChar;
  int TimeOut;
  bool FlagEnd, FlagTO;
  unsigned short CRC ;
  String CRCrecieved = "";
  String CRCcalc = "";
  bool Zap=false;
  char Addr, _Addr;

  if (SmartAlpha.available())
  {
    RecievedChar = SmartAlpha.read();
    if (RecievedChar == TRAME_START)
    {
      while (!SmartAlpha.available()){}; //test de la correspondance d'adresse
      {
        Addr = (unsigned char)SmartAlpha.read();
        while (!SmartAlpha.available()){};
        _Addr = (unsigned char)SmartAlpha.read();
        _Addr= ~_Addr;
        Serial.print(Addr);
        Serial.print("|");
        Serial.println(_Addr);
        if(Addr != _Addr) 
        {
          Serial.println("Erreur sur complement adresse");
         SmartAlphaFlush();
          FlagCmd = 0;
          return(0);
        }
        if ( Addr != TOTEMIGO_ADDR ) //ce message ne me concerne pas
        {
          
          while (SmartAlpha.available())
          {
            RecievedChar = SmartAlpha.read();
          };
          FlagCmd = 0;
          return(0);
        }
      }
      //Serial.println("Message pour moi");
      TimeOut = 0;
      FlagEnd = false;
      FlagTO = false;
      ReceivedString = "";
      while (!FlagEnd && !FlagTO)
      {
        TimeOut++;
        if (TimeOut > TO_SMARTALPHA) FlagTO = true;
        while (SmartAlpha.available())
        {
          RecievedChar = SmartAlpha.read();
          if (RecievedChar == TRAME_END) FlagEnd = true;
          else ReceivedString += RecievedChar;
        }
      }
      
      while(SmartAlpha.available()) //Flush du buffer RX
      {
        RecievedChar = SmartAlpha.read();
      }
      
      if (! FlagTO)
      {
        int CRCstart = ReceivedString.indexOf('|');
        CRCrecieved = ReceivedString.substring(CRCstart + 1, CRCstart + 5);
        ReceivedString = ReceivedString.substring(0, CRCstart);
        CRC = calcrc(ReceivedString.c_str(), ReceivedString.length());
        #ifdef DEBUG
        Serial.print("Received from SmartAlpha: ");
        Serial.println(ReceivedString);
        Serial.print("CRCrecieved: ");
        Serial.println(CRCrecieved);
        Serial.print("CRC calcule: ");
        #endif
        CRCcalc = String(CRC, HEX);
        #ifdef DEBUG
        Serial.println(CRCcalc);
        Serial.print(" TO =");
        Serial.println(TimeOut);
        #endif
        if (CRCcalc != CRCrecieved)
        {
          #ifdef DEBUG
          Serial.println("CRC error");
          #endif
          FlagCmd = 2;
          SmartAlphaFlush();
        }
        else
        {
          FlagCmd = 1;
          String_Split(SPLIT);
        }
      }
      else
      {
        #ifdef DEBUG
        Serial.print("TIME OUT");
        #endif
        FlagCmd = 3;
        SmartAlphaFlush();
      }
    }
  }
}
//*******************************************************
void SerialRxProcess()
{
  char RecievedChar;
  long TimeOut;
  bool FlagTO;
  bool FlagEnd=false;
  
  if (Serial.available())
  {
    RecievedChar = Serial.read();
    if (RecievedChar == TRAME_START)
    {
      TimeOut = 0;
      FlagEnd = false;
      FlagTO = false;
      while (!FlagEnd && !FlagTO)
      {
        TimeOut++;
        if (TimeOut > TO_UART) FlagTO = true;

        if (Serial.available())
        {
          RecievedChar = Serial.read();
          if (RecievedChar == TRAME_END) FlagEnd = true;
          else ReceivedString += RecievedChar;
        }
      }
      if (! FlagTO)
      {
        #ifdef DEBUG
        Serial.print("Received from UART: ");
        Serial.print(ReceivedString);
        #endif
        String_Split(SPLIT);
        #ifdef DEBUG
        Serial.print("TO=");
        Serial.println(TimeOut);
        #endif
        FlagCmd = 1;
      }
      #ifdef DEBUG
      else Serial.print("TIME OUT");
      #endif
    }
  }
}
//***************************************************************
void String_Split(char Splitter)
{
  int i;
  int Index = 0;
  int Nsubs;
  for (i = 0; i < SPLITTERS; i++)
  {
    SplitIndex[i] = ReceivedString.indexOf(Splitter, Index);
    Index = SplitIndex[i] + 1;
    if (!Index) break;
  }
  Nsubs = i + 1;
  Index = -1;
  for (i = 0; i < Nsubs; i++)
  {
    Commande[i] =  ReceivedString.substring(Index + 1, SplitIndex[i]);
    Index = SplitIndex[i];
  }
  #ifdef DEBUG
  Serial.print("Commande ");
  Serial.print(": ");
  Serial.println(Commande[0]);
  #endif

  for (i = 1; i < Nsubs; i++)
  {
    Param[i] = Commande[i].toInt();
    #ifdef DEBUG
    Serial.print("Parametre ");
    Serial.print(i);
    Serial.print("= ");
    Serial.println(Param[i]);
    #endif
  }
  ReceivedString = "";
  for (i = 0; i < SPLITTERS; i++)
  {
    SplitIndex[i] = -1;
  }
}
//***************************************************************
void SerialFlush()
{
  char Dummy;
  while (Serial.available())
  {
    Dummy = Serial.read();
  }

}


