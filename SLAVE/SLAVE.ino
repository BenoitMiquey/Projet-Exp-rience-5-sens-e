//config HW= Arduino pro mini, ATmega328 3V3 8Mhz
//attention: à 8Mhz, Arduino ne peut pas depasser 57600 bps en UART

#define REDUCELIGHT 0.05

#define UARTSPEED 57600
#define SMARTALPHASPEED 38400
#define _SS_MAX_RX_BUFF 256 // Software serial RX buffer size 
#define BUFFER_LENGHT 64 //taille buffer I2C
// 700;for the purple slave
// 200 for the blanc
#define TOUCH_TRIG 900
#define TOUCH_HYSTERESIS 10
#define TOUCH_INHIB 20
#define TANGAGE_MAX 30

#include <SoftwareSerial.h>
#include <SlaveSeeedRFID.h> //attention: lib mofifiée par JNL, ajout méthode listen()
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include "Slave.h"
#include <Adafruit_NeoPixel.h>
#include "IfDef.h"

#ifdef __AVR__
#include <avr/power.h>
#endif
#define NUMPIXELS 18

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

Adafruit_MMA8451 mma8451 = Adafruit_MMA8451();

extern SoftwareSerial SmartAlpha;
extern SeeedRFID RFID;
int xVal, yVal, zVal;
int Rotation;
struct Inclinaison_ Inclinaison;
int VibrationMotor = 0;
struct Faces Face[6];
struct BGcolor_ BGcolor;
struct Behavior_ Behavior;
struct color Palette[16];
struct Animation_ Anim;
extern bool FlagLedRefresh;
extern int FlagCmd;
void SmartAlphaReinit(void);
void SmartAlphaAddr(char);
void SmartAlphaFlush(void);
long ScanRFID( int Channel);
void SmartAlphaSend(String StringoSend);
void SmartAlphaRxProcess();
void SerialRxProcess();
void CommandExec();
void PaletteInit();
void SerialFlush();
int flashState =1;
int topFaceLight;
void setup()
{
  pinMode(CD4051_A, OUTPUT);
  pinMode(CD4051_B, OUTPUT);
  pinMode(CD4051_C, OUTPUT);
  pinMode(CD4051_I, OUTPUT);
  pinMode(RFID_POWER, OUTPUT);
  pinMode(SMARTALPHA_RTS, OUTPUT);
  pinMode(SMARTALPHA_CTS, INPUT_PULLUP);
  pinMode(MOTOR, OUTPUT);
  digitalWrite(MOTOR, LOW);
  digitalWrite(SMARTALPHA_RTS, HIGH);
  digitalWrite(RFID_POWER, HIGH);
  digitalWrite(CD4051_A, LOW);
  digitalWrite(CD4051_B, LOW);
  digitalWrite(CD4051_C, LOW);
  digitalWrite(CD4051_I, HIGH);
  pixels.begin();
  for (int i = 0; i < NUMPIXELS; i++)  {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
  }
  pixels.show();
  digitalWrite(MOTOR, HIGH);
  delay(50);
  digitalWrite(MOTOR, LOW);
  pixels.clear();
  pixels.show();
  Filter_init(0, T_BUFFER_LENGHT, 0);
  Filter_init(0, T_BUFFER_LENGHT, 1);
  Filter_init(0, T_BUFFER_LENGHT, 2);
  InitFacesLeds();
  Serial.begin(UARTSPEED);
  Serial.println("Connecting I2C MMA8451");
  if (! mma8451.begin())
  {
    while (1);
  }
  Wire.setClock(400000);
  mma8451.setRange(MMA8451_RANGE_2_G);
  delay(20);
  Serial.println("Done !");
  SmartAlpha.begin(SMARTALPHASPEED);
  SmartAlpha.listen();
  SmartAlphaAddr(TOTEMIGO_ADDR);
  digitalWrite(SMARTALPHA_RTS, HIGH);
  Serial.print("TOTEMIGO ");
  Serial.print(TOTEMIGO_ADDR);
  Serial.println(": Ready");
  BGcolor.R = 10;
  BGcolor.G = 0;
  BGcolor.B = 10;
  ClearFaces(1);
  Anim.Mode = 3;
  Anim.Red = 255;
  Anim.Green = 255;
  Anim.Blue = 255;
  Anim.Speed = 1;
  Anim.BdT = 1;
  Anim.Face = 0;
  PaletteInit();
  Behavior.State = STANDBY;
  SmartAlphaFlush();
  SerialFlush();

}
//************************************************************
void loop()
{
  bool InclinoRun = false;

#ifdef COMUART
  SerialRxProcess();
#else
  SmartAlphaRxProcess();
#endif
  if (FlagCmd > 0)
  {
    CommandExec();
    FlagCmd = 0;
  }
  if (VibrationMotor > 0) VibrationMotor--;
  if (VibrationMotor == 0) digitalWrite(MOTOR, LOW);
  else digitalWrite(MOTOR, HIGH);
  UpdateFacesTouch();

  /*
    InclinoRun = false;
    if (Anim.Mode == 2) InclinoRun = true;
    if (Behavior.State == WAIT_FACEMATCH) InclinoRun = true;*/
  InclinoRun = true;

  if (VibrationMotor > 0) InclinoRun = false;
  if (InclinoRun)
  {
    Inclinometre();
  }

  UpdateFacesLeds();
  BehaviorRun();
}
//************************************************************
void BehaviorRun()
{
  int F;
  F = Behavior.FaceMatch;
  int TopFace = -1;
  static int PreviousFace = -1;
  bool TangageOK = false;
  long temp;
  bool RfidOK;

  if (abs(Inclinaison.Tangage) < TANGAGE_MAX) TangageOK = true;

  TopFace = Inclinaison.Secteur;
  //if ((TopFace != PreviousFace)&& (abs(Inclinaison.Tangage) < TANGAGE_MAX)) VibrationMotor = 3;
  PreviousFace = TopFace;

  switch (Behavior.State)
  {
    case INIT:  //----------------------------------------------------
      Anim.Mode = STOP;
      for (int f = 0; f < 6; f++)
      {
        Face[f].ClignoTime = 1;
        Face[f].ClignoCount = 1;
        Face[f].ClignoState = false;
      }
      if (F >= 0)
      {
        Face[F].Red = Palette[Behavior.FaceColor].R;
        Face[F].Green = Palette[Behavior.FaceColor].G;
        Face[F].Blue = Palette[Behavior.FaceColor].B;
        Face[F].ClignoTime = Behavior.Cligno;
        Face[F].ClignoCount = 1;
        Face[F].ClignoState = true;
      }
      FlagLedRefresh = true;
      Behavior.ErrorFace = -1;
      if (F >= 0) Behavior.State = WAIT_FACEMATCH;
      else Behavior.State = WAIT_NOFACE;
      break;

    case WAIT_NOFACE: //----------------------------------------------------
      for (int i = 0; i < 6; i++)
      {
        if (Face[i].TouchState == 1)
        {
          Behavior.ErrorFace = i;         //on memorise la touche/face erronee
          Behavior.ErrorSector = TopFace;
          Behavior.State = MATCH_ERROR;
        }
      }
      break;

    case WAIT_FACEMATCH: //----------------------------------------------------

      if ((Face[F].TouchState == 1) && (TopFace == F) && TangageOK )
      {
        Behavior.State = CHECK_RFID;
      }
      else
      {
        for (int i = 0; i < 6; i++)
        {
          if (Face[i].TouchState == 1)
          {
            Behavior.ErrorFace = i;         //on memorise la touche/face erronee
            Behavior.ErrorSector = TopFace;
            Behavior.State = MATCH_ERROR;
          }
        }
      }
      break;

    case CHECK_RFID: //----------------------------------------------------

      if ( Behavior.RFID != "noRFID")
      {
        temp = ScanRFID(5);
        if ( Behavior.RFID == String(temp, HEX)) RfidOK = true;
        else RfidOK = false;
      }
      else RfidOK = true;

      if (RfidOK) Behavior.State = MATCHED;
      else Behavior.State = WAIT_FACEMATCH;

      break;

    case MATCHED: //----------------------------------------------------
      if (Behavior.Last) //Animation si c'est le dernier de la série
      {
        /*
          VibrationMotor = 3;
          Anim.Mode = ROTATION;
          Anim.Red = 0;
          Anim.Green = 255;
          Anim.Blue = 0;
          Anim.Speed = 5;
          Anim.BdT = 1;
          Anim.Face = F;
          Behavior.State = STANDBY;*/
        Anim.Mode = STOP;
        for (int f = 0; f < 6; f++)
        {
          Face[f].Red = 0;
          Face[f].Green = 255;
          Face[f].Blue = 0;
          Face[f].ClignoTime = 1;
          Face[f].ClignoCount = 1;
          Face[f].ClignoState = true;
        }
        FlagLedRefresh = true;
        Behavior.State = STANDBY;
      }
      else  //Si ce n'est pas le dernier, on éteint toutes les faces
      {
        //VibrationMotor = 3;
        Face[F].ClignoTime = 1;
        Face[F].ClignoCount = 1;
        Face[F].ClignoState = true;
        ClearFaces(1);
        if (Face[F].TouchState == 0) //attendre que l'appui soit relaché
          Behavior.State = WAIT_NOFACE;
      }
      break;

    case MATCH_ERROR: //----------------------------------------------------
      /*
        Anim.Mode = ROTATION;
        Anim.Red = 255;
        Anim.Green = 0;
        Anim.Blue = 0;
        Anim.Speed = 5;
        Anim.BdT = 1;
        Anim.Face = F;
        Behavior.State = STANDBY;*/
      /*
        Anim.Mode = STOP;
        for (int f = 0; f < 6; f++)
        {
        Face[f].Red = 255;
        Face[f].Green = 0;
        Face[f].Blue = 0;
        Face[f].ClignoTime = 1;
        Face[f].ClignoCount = 1;
        Face[f].ClignoState = true;
        }
        FlagLedRefresh = true;*/
      Behavior.State = STANDBY;

      break;
    case STANDBY: //----------------------------------------------------

      break;
  }

}

//************************************************************
void UpdateFacesTouch()
{
  static long Inhib[] = {0, 0, 0, 0, 0, 0};

  for (int i = 0; i < 6; i++)
  {
    Face[i].TouchLevel = ScanTouch(i);

    if ( (Face[i].TouchLevel >= (TOUCH_TRIG + TOUCH_HYSTERESIS)) &&  (Inhib[i] == 0))
    {
      Face[i].TouchState = 1;
      Inhib[i] = TOUCH_INHIB;
    }
    if (Face[i].TouchLevel <= (TOUCH_TRIG - TOUCH_HYSTERESIS))
    {
      Face[i].TouchState = 0;
      if (Inhib[i] > 0)Inhib[i]--;
    }
  }
  /*
    Serial.print(Face[0].TouchState);
    Serial.print("/");
    Serial.println(Inhib[0]);*/
}
//**************************************************************
void UpdateFacesLeds()
{
  int face;
  static int PreviousFace = -1;
  switch (Anim.Mode)
  {
    case STOP:
      NoAnimation();
      break;

    case ROTATION:
      if (Anim.BdT > 0) Anim.BdT--;
      if (Anim.BdT == 0)
      {
        Anim.BdT = Anim.Speed;
        Anim.Face++;
        if (Anim.Face == 6)
        {
          Anim.Face = 0;
        }
        ClearFaces(0);
        face = Anim.Face;
        pixels.setPixelColor(Face[face].LedIndex[0], Anim.Red, Anim.Green, Anim.Blue );
        pixels.setPixelColor(Face[face].LedIndex[1], Anim.Red, Anim.Green, Anim.Blue );
        pixels.setPixelColor(Face[face].LedIndex[2], Anim.Red, Anim.Green, Anim.Blue );
        pixels.show();
      }
      break;

    case TOPLIGHT:
      face = Inclinaison.Secteur;
      if (abs(Inclinaison.Tangage) < TANGAGE_MAX)
      {
        if (face != PreviousFace)
        {
          ClearFaces(0);
          pixels.setPixelColor(Face[face].LedIndex[0], Anim.Red, Anim.Green, Anim.Blue );
          pixels.setPixelColor(Face[face].LedIndex[1], Anim.Red, Anim.Green, Anim.Blue );
          pixels.setPixelColor(Face[face].LedIndex[2], Anim.Red, Anim.Green, Anim.Blue );
          pixels.show();
          if (Anim.Speed > 0) VibrationMotor = 3;
        }
        PreviousFace = face;
      }
      else
      {
        PreviousFace = -1;
        ClearFaces(1);
      }
      break;
      case ANIMFLASH:
      //Serial.print(Rfid);
      
        if (flashState == 1)
        {
          flashState = 0;
          for(int i; i < 6; i++)
          {
            //Serial.print("init flash demo...");
            Face[i].colorIndex = i;
            VibrationMotor += 5;
            pixels.setPixelColor(Face[i].LedIndex[0], Palette[i].R*REDUCELIGHT, Palette[i].G*REDUCELIGHT , Palette[i].B*REDUCELIGHT );
            pixels.setPixelColor(Face[i].LedIndex[1], Palette[i].R*REDUCELIGHT, Palette[i].G*REDUCELIGHT , Palette[i].B*REDUCELIGHT );
            pixels.setPixelColor(Face[i].LedIndex[2], Palette[i].R*REDUCELIGHT, Palette[i].G*REDUCELIGHT , Palette[i].B*REDUCELIGHT );
          }
          
          pixels.show();
        }




        face = Inclinaison.Secteur;
        for( int i; i < 6; i++)
        {
          if((Face[i].TouchState == 1) && !(Face[i].WaitRelease ))
          {
            
            VibrationMotor += 8;
            Face[i].WaitRelease = 1;
            Face[i].colorIndex = (Face[i].colorIndex + 1)%6;
            if (face == i) // si la top face est celle sur laquelle on appuie, on applique pas de reduction de luminosité
            {
              pixels.setPixelColor(Face[i].LedIndex[0], Palette[Face[i].colorIndex].R, Palette[Face[i].colorIndex].G, Palette[Face[i].colorIndex].B);
              pixels.setPixelColor(Face[i].LedIndex[1], Palette[Face[i].colorIndex].R, Palette[Face[i].colorIndex].G, Palette[Face[i].colorIndex].B);
              pixels.setPixelColor(Face[i].LedIndex[2], Palette[Face[i].colorIndex].R, Palette[Face[i].colorIndex].G, Palette[Face[i].colorIndex].B);
            }
            else
            {
              pixels.setPixelColor(Face[i].LedIndex[0], Palette[Face[i].colorIndex].R*REDUCELIGHT, Palette[Face[i].colorIndex].G*REDUCELIGHT , Palette[Face[i].colorIndex].B*REDUCELIGHT );
              pixels.setPixelColor(Face[i].LedIndex[1], Palette[Face[i].colorIndex].R*REDUCELIGHT, Palette[Face[i].colorIndex].G*REDUCELIGHT , Palette[Face[i].colorIndex].B*REDUCELIGHT );
              pixels.setPixelColor(Face[i].LedIndex[2], Palette[Face[i].colorIndex].R*REDUCELIGHT, Palette[Face[i].colorIndex].G*REDUCELIGHT , Palette[Face[i].colorIndex].B*REDUCELIGHT );
            }
            pixels.show();
            break;
          }
        }
        for( int i; i < 6; i++)
        {
          if((Face[i].TouchState == 0) && (Face[i].WaitRelease ))
          {
            Face[i].WaitRelease = 0;
            break;
          }
        }
        if ( AllSameFaces()){
          for ( int i=0; i < 255; i++)
          {
            for(int j=0; j < 6; j++)
            {
              pixels.setPixelColor(Face[j].LedIndex[0], (Palette[Face[j].colorIndex].R+i)%255 , (Palette[Face[j].colorIndex].G+i)%255 , (Palette[Face[j].colorIndex].B+i)%255 );
              pixels.setPixelColor(Face[j].LedIndex[1], (Palette[Face[j].colorIndex].R+i)%255, (Palette[Face[j].colorIndex].G+i)%255 , (Palette[Face[j].colorIndex].B+i)%255 );
              pixels.setPixelColor(Face[j].LedIndex[2], (Palette[Face[j].colorIndex].R+i)%255,( Palette[Face[j].colorIndex].G+i)%255 , (Palette[Face[j].colorIndex].B+i)%255);
            }
          pixels.show();
          }
          for ( int i=255; i > 0; i--)
          {
            for(int j=0; j < 6; j++)
            {
              pixels.setPixelColor(Face[j].LedIndex[0], (Palette[Face[j].colorIndex].R+i)%255 , (Palette[Face[j].colorIndex].G+i)%255 , (Palette[Face[j].colorIndex].B+i)%255 );
              pixels.setPixelColor(Face[j].LedIndex[1], (Palette[Face[j].colorIndex].R+i)%255, (Palette[Face[j].colorIndex].G+i)%255 , (Palette[Face[j].colorIndex].B+i)%255 );
              pixels.setPixelColor(Face[j].LedIndex[2], (Palette[Face[j].colorIndex].R+i)%255,( Palette[Face[j].colorIndex].G+i)%255 , (Palette[Face[j].colorIndex].B+i)%255);
            }
          pixels.show();
          }
        VibrationMotor += 20;
        flashState = 1;   
        }




      face = Inclinaison.Secteur;
      if (abs(Inclinaison.Tangage) < TANGAGE_MAX)
      {
        if (face != PreviousFace)
        {
          //ClearFaces(0); on remet tout en low
          for ( int i= 0; i < 6 ; i ++)
          {
            pixels.setPixelColor(Face[i].LedIndex[0], Palette[Face[i].colorIndex].R*REDUCELIGHT, Palette[Face[i].colorIndex].G*REDUCELIGHT , Palette[Face[i].colorIndex].B*REDUCELIGHT );
            pixels.setPixelColor(Face[i].LedIndex[1], Palette[Face[i].colorIndex].R*REDUCELIGHT, Palette[Face[i].colorIndex].G*REDUCELIGHT , Palette[Face[i].colorIndex].B*REDUCELIGHT );
            pixels.setPixelColor(Face[i].LedIndex[2], Palette[Face[i].colorIndex].R*REDUCELIGHT, Palette[Face[i].colorIndex].G*REDUCELIGHT , Palette[Face[i].colorIndex].B*REDUCELIGHT );
          }

          // on met en hight la topface
          pixels.setPixelColor(Face[face].LedIndex[0], Palette[Face[face].colorIndex].R, Palette[Face[face].colorIndex].G , Palette[Face[face].colorIndex].B );
          pixels.setPixelColor(Face[face].LedIndex[1], Palette[Face[face].colorIndex].R, Palette[Face[face].colorIndex].G, Palette[Face[face].colorIndex].B );
          pixels.setPixelColor(Face[face].LedIndex[2], Palette[Face[face].colorIndex].R, Palette[Face[face].colorIndex].G , Palette[Face[face].colorIndex].B);
          pixels.show();
          if (Anim.Speed > 0) VibrationMotor = 3;
        }
        PreviousFace = face;
      }
      else
      {
        PreviousFace = -1;
        // ClearFaces(1); sinon on met tout en low

        for ( int i= 0; i < 6 ; i ++)
          {
            pixels.setPixelColor(Face[i].LedIndex[0], Palette[Face[i].colorIndex].R*REDUCELIGHT, Palette[Face[i].colorIndex].G*REDUCELIGHT , Palette[Face[i].colorIndex].B*REDUCELIGHT );
            pixels.setPixelColor(Face[i].LedIndex[1], Palette[Face[i].colorIndex].R*REDUCELIGHT, Palette[Face[i].colorIndex].G*REDUCELIGHT , Palette[Face[i].colorIndex].B*REDUCELIGHT );
            pixels.setPixelColor(Face[i].LedIndex[2], Palette[Face[i].colorIndex].R*REDUCELIGHT, Palette[Face[i].colorIndex].G*REDUCELIGHT , Palette[Face[i].colorIndex].B*REDUCELIGHT );
          }
          pixels.show();
      }

      break;
  }
}

//************************************************************
void NoAnimation()
{
  int TopFace = -1;
  static int PreviousFace = -1;
  int face;
   
  face = Inclinaison.Secteur;
  if (abs(Inclinaison.Tangage) < TANGAGE_MAX)
  {
    if (face != PreviousFace)
    {
      VibrationMotor = 3;
    }
    PreviousFace = face;
  }
    for (int face = 0; face < 6; face++)
    {
      if (Face[face].ClignoTime > 1)
      {
        Face[face].ClignoCount--;
        if (Face[face].ClignoCount == 0)
        {
          Face[face].ClignoCount = Face[face].ClignoTime;
          Face[face].ClignoState = !(Face[face].ClignoState);
          FlagLedRefresh = true;
        }
      }
      //if (Face[face].ClignoState || (Inclinaison.Secteur == face))
      if (Face[face].ClignoState )
      {
        pixels.setPixelColor(Face[face].LedIndex[0], pixels.Color(Face[face].Red, Face[face].Green, Face[face].Blue));
        pixels.setPixelColor(Face[face].LedIndex[1], pixels.Color(Face[face].Red, Face[face].Green, Face[face].Blue));
        pixels.setPixelColor(Face[face].LedIndex[2], pixels.Color(Face[face].Red, Face[face].Green, Face[face].Blue));
      }
      else
      {
        pixels.setPixelColor(Face[face].LedIndex[0], BGcolor.R, BGcolor.G, BGcolor.B);
        pixels.setPixelColor(Face[face].LedIndex[1], BGcolor.R, BGcolor.G, BGcolor.B);
        pixels.setPixelColor(Face[face].LedIndex[2], BGcolor.R, BGcolor.G, BGcolor.B);
      }
    }
    if (FlagLedRefresh)
    {
      pixels.show();
      FlagLedRefresh = false;
    }
  }
  //*************************************************************
  void InitFacesLeds()
  {
    Face[0].LedIndex[0] = 15;
    Face[0].LedIndex[1] = 16;
    Face[0].LedIndex[2] = 17;
    Face[5].LedIndex[0] = 0;
    Face[5].LedIndex[1] = 1;
    Face[5].LedIndex[2] = 2;
    Face[4].LedIndex[0] = 3;
    Face[4].LedIndex[1] = 4;
    Face[4].LedIndex[2] = 5;
    Face[3].LedIndex[0] = 6;
    Face[3].LedIndex[1] = 7;
    Face[3].LedIndex[2] = 8;
    Face[2].LedIndex[0] = 9;
    Face[2].LedIndex[1] = 10;
    Face[2].LedIndex[2] = 11;
    Face[1].LedIndex[0] = 12;
    Face[1].LedIndex[1] = 13;
    Face[1].LedIndex[2] = 14;
    ClearFaces(1);
  }
  //****************************************
  void ClearFaces(int show)
  {
    for (int i = 0; i < 6; i++)
    {
      Face[i].Red = 0;
      Face[i].Green = 0;
      Face[i].Blue = 0;
      pixels.setPixelColor(Face[i].LedIndex[0], BGcolor.R, BGcolor.G, BGcolor.B);
      pixels.setPixelColor(Face[i].LedIndex[1], BGcolor.R, BGcolor.G, BGcolor.B);
      pixels.setPixelColor(Face[i].LedIndex[2], BGcolor.R, BGcolor.G, BGcolor.B);
    }
    if (show) pixels.show();
  }

  bool AllSameFaces()
  {
    for( int i = 0; i < 5; i ++)
    {
      if( Face[i].colorIndex != Face[i+1].colorIndex ) return false;
    }
    return true;
  }



