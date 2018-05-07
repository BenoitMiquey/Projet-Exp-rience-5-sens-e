//config HW= Arduino pro mini, ATmega328 3V3 8Mhz

//TODO:
// led IHM
// Check Inscope
// Tirage Serie Gameplay: ne pas tirer les faces Bottom

using namespace std;
#include "ifDef.h"
#include "CSlave.h"

#ifdef __AVR__
#include <avr/power.h>
#endif

extern String ReceivedString;
extern String Answer[];
extern RTC_DS1307 rtc;
extern int Param[];


Slave Tot[4];


Adafruit_VS1053_FilePlayer mp3 =
  Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);

int Cpt = 0;

void setup()
{
  pinMode(BUTTON, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(SMARTALPHA_RTS, OUTPUT);
  pinMode(SMARTALPHA_CTS, INPUT_PULLUP);
  digitalWrite(SMARTALPHA_RTS, HIGH);
  digitalWrite(RFID_POWER, HIGH);
  digitalWrite(LED, HIGH);
  Serial.begin(115200);
  RTCinit();
  Serial.println(" Master initialisation...");
  Serial1.begin(38400);
  SmartAlphaReinit();
  SmartAlphaAddr('*');
  SD.begin(CARDCS);    // initialise the SD card

  if (! mp3.begin()) { // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1);
  }
  Serial.println(F("\r\nMP3 Player VS1053 found"));
  mp3.setVolume(0, 0); // Set volume for left, right channels. lower numbers == louder volume!
  delay(300);
  //mp3.playFullFile("Ring.mp3");

  Serial.print("Date & Time (Y,M,D,h,m,s) ");
  Serial.print("(");
  RTCread();
  Serial.println(")");
  Serial.println();
  SDlogWrite(-1, "Reboot");
  SDconfigRead();
  Serial.println();
  PaletteInit();
  digitalWrite(LED, LOW);

  Tot[0].Addr = 'A';
  Tot[1].Addr = 'B';
  Tot[2].Addr = 'C';
  Tot[3].Addr = 'D';

  TalkToSlave('A', "BGcolor,0,0,0", MAXRETRY);
  delay(1000);
  
 
  Serial.println("\n\r Master ready, please type your command");
  
}
//************************************************************
void loop()
{
  if (SerialRxProcess())CmdInterpreter();
  //SimonPlay();
  //NewSimonPlay();
  CalculPlay();
}

//*******************************************************
void CmdInterpreter()
{
  int temp;
  String Stemp = "";
  char Tot;
  bool Unknown = true;
#ifdef DEBUG
  Serial.print("Command Interpreter: ");
  Serial.println(ReceivedString);
#endif


  //-------------------------------------------------------
  if (Answer[0] == "ReadConfig")
  {
    Serial.println("Configuration File:");
    SDconfigRead();
    Unknown = false;
  }

  //-------------------------------------------------------
  if (Answer[0] == "Reboot")
  {
    Serial.println("<REBOOT>");
    Unknown = false;
    software_Reboot();
  }

  //-------------------------------------------------------
  if (Answer[0] == "Log")
  {
    Tot = Answer[1][0];
    Serial.print("Log File /");
    Serial.print(Tot);
    Serial.print("-> ");
    if (Tot == 'C') //effacement du fichier Log
    {
      SDlogWrite(0, "");
      Unknown = false;
      Serial.println("<>");
    }
    if (Tot == 'W') //on envoie un Event
    {
      SDlogWrite(-1, Answer[2]);
      Unknown = false;
      Serial.println("<>");
    }
    if (Tot == 'R') //lecture du fichier Log
    {
      Serial.println();
      Serial.println("< BEGIN");
      SDlogRead();
      Unknown = false;
      Serial.println("END >");
    }
  }
  //-------------------------------------------------------
  if (Answer[0] == "Clock")
  {
    Tot = Answer[1][0];
    Serial.print("Real Time Clock /");
    Serial.print(Tot);

    if (Tot == 'R')
    {
      Serial.print(" (Y,M,D,h,m,s) ");
      Serial.print("<");
      RTCread();
      Serial.println(">");
      Unknown = false;
    }
    if (Tot == 'W')
    {
      Serial.print(" (Y,M,D,h,m,s) ");
      rtc.adjust(DateTime(Param[2], Param[3], Param[4], Param[5], Param[6], Param[7]));
      Serial.print("<");
      RTCread();
      Serial.println(">");
      Unknown = false;
    }
  }
  //-------------------------------------------------------
  if (Answer[0] == "Sound")
  {
    Tot = Answer[1][0];
    Serial.print("Play Mp3: ");
    Serial.print(Tot);
    Serial.print("->");
    if (Tot == 'W')
    {
      Serial.println("Winning.mp3");
      Unknown = false;
      mp3.playFullFile("Winning.mp3");
      Serial.println("<>");
    }
    if (Tot == 'L')
    {
      Serial.println("Losing.mp3");
      Unknown = false;
      mp3.playFullFile("Losing.mp3");
      Serial.println("<>");
    }
    if (Tot == 'R')
    {
      Serial.println("Ring.mp3");
      Unknown = false;
      mp3.playFullFile("Ring.mp3");
      Serial.println("<>");
    }
  }
  //-------------------------------------------------------
  if (Answer[0] == "Tot")
  {
    Tot = Answer[1][0];
    Serial.print("Adressing Slave ");
    Serial.print(Tot);
    Stemp = ReceivedString.substring(6);
    Serial.print(", with command Line: ");
    Serial.println(Stemp);
    temp = TalkToSlave(Tot, Stemp, MAXRETRY);
    Stemp = "<";
    Stemp += ReceivedString;
    Stemp += ">";
    Serial.print("\n\rAnswer: ");
    if (temp < MAXRETRY) Serial.println(Stemp);
    else Serial.print("<RETRY ERROR>");
    Unknown = false;
  }
  //-------------------------------------------------------
  if (Unknown)
  {
    Serial.println("Unknown command, please try again");
    Serial.println("<?>");
  }
  Serial.println();
  Serial.println("Master ready!");
}
//*******************************************************
bool SerialRxProcess()
{
  char RecievedChar;
  bool FlagEnd;
  bool Cmd = false;

  ReceivedString = "";
  if (Serial.available())
  {
    RecievedChar = Serial.read();
    if (RecievedChar == TRAME_START)
    {
      FlagEnd = false;
      while (!FlagEnd)
      {
        if (Serial.available())
        {
          RecievedChar = Serial.read();
          if (RecievedChar == TRAME_END) FlagEnd = true;
          else ReceivedString += RecievedChar;
        }
      }
      Serial.print("\n\rReceived from UART: ");
      Serial.println(ReceivedString);
      String_Split(SPLIT);
      Cmd = true;
    }
  }
  return (Cmd);
}
//*********************************************

void software_Reboot()
{
  wdt_enable(WDTO_15MS);
  while (1) {}
}

