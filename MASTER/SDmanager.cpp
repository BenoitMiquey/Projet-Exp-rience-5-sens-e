

#include "ifDef.h"
#include "CSlave.h"

extern RTC_DS1307 rtc;
extern String CodeRFID[];

//extern Slave TotA,TotB,TotC,TotD;
extern Slave Tot[4];

//******************************************************************************
void SDconfigRead()
{
  File myFile;
  myFile = SD.open("Config.txt");
  String Buffer="";
  String Tag="";

  if (myFile)
  {
    Buffer = myFile.readStringUntil('\n');
    Serial.println(Buffer); 
    for(int i=0; i<4; i++)
    {
      if (myFile.available())
      {
        Buffer = myFile.readStringUntil('\n');
        Serial.println(Buffer); 
        Tag=Buffer.substring(0, 6); //lecture du code tag RFID
        CodeRFID[i]=Tag;
        //Tot[i].ID=Tag;
      }
    }
    myFile.close();
  }
  else Serial.println("error opening Config.txt");
}

//******************************************************************************
void SDlogRead()
{
  File myFile;
  myFile = SD.open("Log.txt");
  if (myFile)
  {
    while (myFile.available())
    {
      Serial.write(myFile.read());
    }
    myFile.close();
  }
  else Serial.println("error opening Log.txt");
}

//******************************************************************************
void SDlogWrite(int Seek, String Event)
{
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  if (Seek >= 0) SD.remove("Log.txt");

  File myFile;
  myFile = SD.open("Log.txt", FILE_WRITE);
  DateTime now = rtc.now();

  // if the file opened okay, write to it:
  if (myFile) {
    if (Seek >= 0)
    {
      Serial.print("Clear Log.txt, Seek=");
      Serial.println(Seek);
      myFile.seek(Seek);
      Event = "Log file created";
    }
    else
    {
      Serial.print("Writing to Log.txt: ");
      Serial.println(Event);
    }
    myFile.print(now.year(), DEC);
    myFile.print('/');
    myFile.print(now.month(), DEC);
    myFile.print('/');
    myFile.print(now.day(), DEC);
    myFile.print('-');
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    myFile.print(now.minute(), DEC);
    myFile.print(':');
    myFile.print(now.second(), DEC);
    myFile.print(" Ev->");
    myFile.println(Event);
    
    myFile.close();
  } else Serial.println("error opening Log.txt");
}

