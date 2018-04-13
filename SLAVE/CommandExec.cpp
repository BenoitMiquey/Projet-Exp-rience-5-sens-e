#include <arduino.h>
#include "Slave.h"
#include "Slave.h"
#include "IfDef.h"

extern struct color Palette[];
extern int Param[];
extern String Commande[];
bool FlagLedRefresh = false;
extern struct Inclinaison_ Inclinaison;
void SmartAlphaSend(String StringoSend);
void UpdateFacesLeds();
extern int VibrationMotor;
extern struct Faces Face[];
String Trame = "";
extern struct Animation_ Anim;
extern struct BGcolor_ BGcolor;
extern int FlagCmd;
extern struct Behavior_ Behavior;

void ClearFaces(int show);

void CommandExec()
{
#ifdef DEBUG
  Serial.print("CommandExec => ");
  Serial.println(Commande[0]);
#endif
  Trame = "ERR,";
  Trame += FlagCmd;

  if (FlagCmd > 1) goto Error;

  if (Commande[0] == "BeR") //**********************************
  {
    Trame = "OK,";
    Trame += Commande[0];
    Trame += ",";
    Trame += String(Behavior.State);
    Trame += ",";
    Trame += String(Behavior.ErrorFace);
    Trame += ",";
    Trame += String(Behavior.ErrorSector);
    Trame += ",";
    Trame += String(Inclinaison.Secteur);
  }

  if (Commande[0] == "BeW") //**********************************
  {
    Trame = "OK,";
    Trame += Commande[0];

    //enum BehaviorStates {INIT, WAIT_FACEMATCH, WAIT_NOFACE, CHECK_RFID, MATCHED, MATCH_ERROR, STANDBY};

    Behavior.State = Param[1];
    Behavior.FaceMatch = Param[2];
    Behavior.FaceColor = Param[3];
    Behavior.Cligno = Param[4];
    Behavior.RFID = Commande[5];
    if (Param[6] == 1) Behavior.Last = true;
    else Behavior.Last = false;
    Behavior.ErrorFace=-1;
    Behavior.ErrorSector=-1;
  }

  if (Commande[0] == "Col") //**********************************
  {
    Trame = "OK,";
    Trame += Commande[0];
    for (int F = 0; F < 6; F++)
    {
      if (Param[F + 1] != BG)
      {
        Face[F].Red = Palette[Param[F + 1]].R;
        Face[F].Green = Palette[Param[F + 1]].G;
        Face[F].Blue = Palette[Param[F + 1]].B;
      }
      else
      {
        Face[F].Red = BGcolor.R;
        Face[F].Green = BGcolor.G;
        Face[F].Blue = BGcolor.B;
      }

      Face[F].ClignoTime = 1;
      Face[F].ClignoCount = 1;
      Face[F].ClignoState = true;
    }
    Anim.Mode = 0;
    Anim.BdT = 1;
    FlagLedRefresh = true;
  }

  if (Commande[0] == "BGcolor") //**********************************
  {
    Trame = "OK,";
    Trame += Commande[0];

    BGcolor.R = Param[1];
    BGcolor.G = Param[2];
    BGcolor.B = Param[3];
    ClearFaces(1);
  }


  if (Commande[0] == "Animation") //**********************************
  {
    Trame = "OK,";
    Trame += Commande[0];

    Anim.Mode = Param[1];
    Anim.Red = Param[2];
    Anim.Green = Param[3];
    Anim.Blue = Param[4];
    Anim.Speed = Param[5];
    Anim.BdT = 1;
    Anim.Face = 0;
    if (Anim.Mode == 0)
    {
      for (int F = 0; F < 6; F++)
      {
        Face[F].ClignoTime = 1;
        Face[F].ClignoCount = 1;
        Face[F].ClignoState = true;
      }
      ClearFaces(1);
    }
  }

  if (Commande[0] == "FaceLed") //**********************************
  {
    Trame = "OK,";
    Trame += Commande[0];

    if (Param[1] < 6)
    {
      if (Param[2] > 0)
      {
        Face[Param[1]].Red = Param[3];
        Face[Param[1]].Green = Param[4];
        Face[Param[1]].Blue = Param[5];
        Face[Param[1]].ClignoTime = Param[2];
        Face[Param[1]].ClignoCount = Param[2];
        Face[Param[1]].ClignoState = true;
      }
      else
      {
        Face[Param[1]].Red = 0;
        Face[Param[1]].Green = 0;
        Face[Param[1]].Blue = 0;
        Face[Param[1]].ClignoTime = 0;
        Face[Param[1]].ClignoCount = 0;
        Face[Param[1]].ClignoState = false;
      }
    }
    else
    {
      for (int i = 0; i < 6; i++)
      {
        if (Param[2] > 0)
        {
          Face[i].Red = Param[3];
          Face[i].Green = Param[4];
          Face[i].Blue = Param[5];
          Face[i].ClignoTime = Param[2];
          Face[i].ClignoCount = Param[2];
          Face[i].ClignoState = true;
        }
        else
        {
          Face[i].Red = 0;
          Face[i].Green = 0;
          Face[i].Blue = 0;
          Face[i].ClignoTime = 0;
          Face[i].ClignoCount = 0;
          Face[i].ClignoState = false;
        }
      }
    }
    FlagLedRefresh = true;
  }

  if (Commande[0] == "TotState") //**********************************
  {
    int Tstate = 0;
    //Face[5].Rfid = ScanRFID(5);

    Trame = "OK,";
    Trame += Commande[0];

    if (Face[0].TouchState == 1) Tstate += 1;
    if (Face[1].TouchState == 1) Tstate += 2;
    if (Face[2].TouchState == 1) Tstate += 4;
    if (Face[3].TouchState == 1) Tstate += 8;
    if (Face[4].TouchState == 1) Tstate += 16;
    if (Face[5].TouchState == 1) Tstate += 32;
    Trame += ",";
    Trame += String(Tstate);
    Trame += ",";
    Trame += String(Inclinaison.Secteur);
    Trame += ",";
    Trame += String(Inclinaison.Tangage);
    //Trame += ",";
    //Trame += String(Face[5].Rfid, HEX);
  }

  if (Commande[0] == "Update") //**********************************
  {
    int Tstate = 0;
    int F;
    Trame = "OK,";
    Trame += Commande[0];

    for (F = 0; F < 6; F++)
    {
      Face[F].Red = Palette[Param[F + 1]].R;
      Face[F].Green = Palette[Param[F + 1]].G;
      Face[F].Blue = Palette[Param[F + 1]].B;
      Face[F].ClignoTime = 1;
      Face[F].ClignoCount = 1;
      Face[F].ClignoState = true;
    }
    if (Param[7] > 0)  VibrationMotor = Param[7];


    FlagLedRefresh = true;
    if (Face[0].TouchState == 1) Tstate += 1;
    if (Face[1].TouchState == 1) Tstate += 2;
    if (Face[2].TouchState == 1) Tstate += 4;
    if (Face[3].TouchState == 1) Tstate += 8;
    if (Face[4].TouchState == 1) Tstate += 16;
    if (Face[5].TouchState == 1) Tstate += 32;
    Trame += ",";
    Trame += String(Tstate);
    Trame += ",";
    Trame += String(Inclinaison.Secteur);
    Trame += ",";
    Trame += String(Inclinaison.Tangage);
  }

  if (Commande[0] == "TouchState") //**********************************
  {
    Trame = "OK,";
    Trame += Commande[0];

    for (int i = 0; i < 6; i++)
    {
      Trame += ",";
      Trame += String(Face[i].TouchState);
    }
  }

  if (Commande[0] == "TouchLevel") //**********************************
  {
    Trame = "OK,";
    Trame += Commande[0];

    for (int i = 0; i < 6; i++)
    {
      Trame += ",";
      Trame += String(Face[i].TouchLevel);
    }
  }

  if (Commande[0] == "FaceID") //**********************************
  {
    Trame = "OK,";
    Trame += Commande[0];
    Face[5].Rfid = ScanRFID(5);
    Trame += ",";
    Trame += String(Face[5].Rfid, HEX);
    /*
      for (int i = 0; i < 6; i++)
      {
      Face[i].Rfid = ScanRFID(i);
      Trame += ",";
      Trame += String(Face[i].Rfid, HEX);
      }*/
  }

  if (Commande[0] == "Vibre") //**********************************
  {
    VibrationMotor = Param[1];
    Trame = "OK,";
    Trame += Commande[0];
  }

  if (Commande[0] == "Inclino") //**********************************
  {
    Trame = "OK,";
    Trame += Commande[0];
    Trame += ",";
    Trame += String(Inclinaison.Secteur);
    Trame += ",";
    Trame += String(Inclinaison.Angle);
    Trame += ",";
    Trame += String(Inclinaison.Tangage);
  }

  if (Commande[0] == "Sector") //**********************************
  {
    Trame = "OK,";
    Trame += Commande[0];
    Trame += ",";
    Trame += String(Inclinaison.Secteur);
  }

Error:
#ifdef COMUART
  Serial.println(Trame);
#else
  SmartAlphaSend(Trame);
  Serial.println(Trame);
#endif

}

void PaletteInit()
{
  Palette[BLEU].R = 0;
  Palette[BLEU].G = 0;
  Palette[BLEU].B = 255;
  Palette[VERT].R = 0;
  Palette[VERT].G = 255;
  Palette[VERT].B = 50;
  Palette[JAUNE].R = 255;
  Palette[JAUNE].G = 200;
  Palette[JAUNE].B = 0;
  Palette[ORANGE].R = 255;
  Palette[ORANGE].G = 80;
  Palette[ORANGE].B = 0;
  Palette[ROUGE].R = 255;
  Palette[ROUGE].G = 0;
  Palette[ROUGE].B = 0;
  Palette[VIOLET].R = 255;
  Palette[VIOLET].G = 0;
  Palette[VIOLET].B = 255;
  Palette[BLANC].R = 255;
  Palette[BLANC].G = 255;
  Palette[BLANC].B = 255;
  Palette[NOIR].R = 0;
  Palette[NOIR].G = 0;
  Palette[NOIR].B = 0;
  Palette[ROSE].R = 255;
  Palette[ROSE].G = 50;
  Palette[ROSE].B = 50;
  Palette[MARRON].R = 35;
  Palette[MARRON].G = 10;
  Palette[MARRON].B = 3;
}

