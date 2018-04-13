#include "Arduino.h"
#include "ifDef.h"

String Answer[SPLITTERS];
int Param[SPLITTERS];
int SplitIndex[SPLITTERS];
String ReceivedString = "";

byte DestAddr[] = {0x30, 0x30, 0x30, 0x30};
byte MyAddr[] = {0x30, 0x30, 0x30, 0x4D};
byte ConfigRF[] = {0x06, 0x75, 0x00}; //dernier octet pour regler puissance emission

//********************************************************************
int TalkToSlave(char Slave, String Cmd, int MaxRetry)
{
  String Trame = "<>";
  String Command = "";
  unsigned short CRC;
  static char LastSlave = '*';
  int RxProcess;
  bool Success = false;
  long Retry = 0;
  char Addr, _Addr;
  int Temp;
  long ErrCpt = 0;

  //if (Slave != LastSlave) SmartAlphaAddr(Slave);
  Addr = (unsigned char)Slave;
  _Addr = ~Addr;

  do
  {
    while (digitalRead(SMARTALPHA_CTS));
    delay(15);
    CRC = calcrc(Cmd.c_str(), Cmd.length());
    Trame = "<";
    Trame += Addr;
    Trame += _Addr;
    Trame += Cmd;
    Trame += "|";
    Trame += String(CRC, HEX);
    Trame += ">";

    Serial.print(Slave);
    Serial.print("/");
    Serial.print(Cmd);
#ifdef DEBUG
    Serial.print("/");
    Serial.print(String(CRC, HEX));
#endif
    Serial.print(" ");
    Serial1.print(Trame);

    RxProcess = SmartAlphaRxProcess(); //renvoie une valeur >0 en cas d'erreur (delai de reponse, TO trame ou bien CRC faux)
    if ((Answer[0] == "OK") && (RxProcess == 0))
    {
      Success = true;
#ifdef DEBUG
      Serial.println("Congrats :-)");
#endif
    }
    else
    {
      Success = false;
      ErrCpt++;
      Serial.print("!!ERREUR =");
      Serial.println(ErrCpt);
    }
    Retry++;
    //delay(20);
#ifdef DEBUG
    Serial.print("Retry= ");
    Serial.println(Retry);
#endif
    if (Retry >= MaxRetry) break;
  } while (!Success );

  LastSlave = Slave;
  return (Retry);
}

//*******************************************************************
int SmartAlphaRxProcess()
{
  char RecievedChar;
  long TimeOut;
  bool FlagEnd, FlagTO;
  unsigned short CRC ;
  String CRCrecieved = "";
  String CRCcalc = "";
  int Result = 0;
  long Delay = 0;
  ReceivedString = "";
  do
  {
    if (Serial1.available())
    {
      RecievedChar = Serial1.read();
      if (RecievedChar == TRAME_START)
      {
        TimeOut = 0;
        FlagEnd = false;
        FlagTO = false;
        ReceivedString = "";
        while (!FlagEnd && !FlagTO)
        {
          TimeOut++;
          if (TimeOut > TO_SMARTALPHA) FlagTO = true;
          while (Serial1.available())
          {
            RecievedChar = Serial1.read();
            if (RecievedChar == TRAME_END) FlagEnd = true;
            else ReceivedString += RecievedChar;
          }
        }
        if (! FlagTO)
        {
          int CRCstart = ReceivedString.indexOf('|');
          CRCrecieved = ReceivedString.substring(CRCstart + 1, CRCstart + 5);
          ReceivedString = ReceivedString.substring(0, CRCstart);
          CRC = calcrc(ReceivedString.c_str(), ReceivedString.length());

          Serial.print("=>");
          Serial.println(ReceivedString);
#ifdef DEBUG
          Serial.print("CRCrecieved: ");
          Serial.println(CRCrecieved);
          Serial.print("CRC calcule: ");
#endif
          CRCcalc = String(CRC, HEX);
#ifdef DEBUG
          Serial.println(CRCcalc);
          Serial.print("ToTrame =");
          Serial.println(TimeOut);
          Serial.print("Delay=");
          Serial.println(Delay);
#endif
          if (CRCcalc != CRCrecieved)
          {
#ifdef DEBUG
            Serial.println("CRC error !");
#endif
            Result = 1;
          }
          else
          {
            String_Split(SPLIT);
          }
        }
        else
        {
#ifdef DEBUG
          Serial.println("Time Out Trame !");
#endif
          Result = 2;
        }
      }
    }
    Delay++;

    if (Delay >= ANSWER_DELAY)
    {
#ifdef DEBUG
      Serial.println("Delai de reponse depasse !");
#endif
      Result = 3;
      break;
    }

  } while (!FlagEnd && !FlagTO);

  return (Result);

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
    Answer[i] =  ReceivedString.substring(Index + 1, SplitIndex[i]);
    Index = SplitIndex[i];
  }
#ifdef DEBUG
  Serial.print("Cmd");
  Serial.print(": ");
  Serial.println(Answer[0]);
#endif

  for (i = 1; i < Nsubs; i++)
  {
    Param[i] = Answer[i].toInt();
#ifdef DEBUG
    Serial.print("Param ");
    Serial.print(i);
    Serial.print("= ");
    Serial.println(Param[i]);
#endif
  }
  //ReceivedString = "";
  for (i = 0; i < SPLITTERS; i++)
  {
    SplitIndex[i] = -1;
  }
}

//************************************************************
void SmartAlphaReinit()
{
  byte Value = 0x00;
  Serial.print("SmartAlpha reinit ");
  Serial1.print("+++");
  delay(20);
  Serial1.write(Value);
  Serial1.write(Value);
  Serial1.write(Value);
  Serial1.write(Value);
  Serial1.write(Value);
  Serial1.write(Value);
  Serial1.write(Value);
  Serial1.write(Value);
  delay(80);
  Serial1.print("---");
  delay(20);
  while (Serial1.available())
  {
    Serial.write( Serial1.read());
  }
  Serial.println();
}
//**************************************************************
void SmartAlphaAddr(char Slave)
{
  Serial.println();
  Serial.print("ADDR: ");
  Serial1.print("+++");
  delay(20);
  Serial1.write(DestAddr[0]);
  Serial1.write(DestAddr[1]);
  Serial1.write(DestAddr[2]);
  Serial1.write(DestAddr[3]);
  //Serial1.write(Slave);
  Serial1.write(MyAddr[0]);
  Serial1.write(MyAddr[1]);
  Serial1.write(MyAddr[2]);
  Serial1.write(MyAddr[3]);
  delay(80);
  Serial1.print("---");
  delay(20);
  while (Serial1.available())
  {
    Serial.write( Serial1.read());
  }
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
