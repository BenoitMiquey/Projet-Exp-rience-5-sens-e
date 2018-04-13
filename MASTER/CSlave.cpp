#include "CSlave.h"

char NeighbourFind(String);
extern RTC_DS1307 rtc;

Slave::Slave()
{
}
/*
Slave::Slave(char addr, String ID)
{
  Addr = addr;
  for (int F = 0; F < 6; F++)
  {
    Face[F].Couleur = NOIR;
  }
  Inscope = false;
  RFID = "";
}*/


Slave::BehaviorW(int State, int Fmatch, int Fcolor, int Fcligno, String RFID, int Last)
{
  String Cmd = "BeW,";
  Cmd += String(State);
  Cmd += ",";
  Cmd += String(Fmatch);
  Cmd += ",";
  Cmd += String(Fcolor);
  Cmd += ",";
  Cmd += String(Fcligno);
  Cmd += ",";
  Cmd += RFID;
  Cmd += ",";
  Cmd += String(Last);

  TalkToSlave(Addr, Cmd, MAXRETRY);
}

Slave::BehaviorR(void)
{
  TalkToSlave(Addr, "BeR", MAXRETRY);
  State = Param[2];
  //Serial.println();
  //Serial.print("State= ");
  //Serial.print(State);
  ErrorFace = Param[3];
  //Serial.print("/ ErrorFace= ");
  //Serial.println(ErrorFace);
  ErrorSector = Param[4];
  FaceTop = Param[5];
  FaceBotUpdate();
}

Slave::Sprint(void)
{
  Serial.print("\n\rSLAVE:"); Serial.print(Addr);
  Serial.print(" ID:"); Serial.print(ID);
  Serial.print(" RFID:"); Serial.print(RFID);
  Serial.print(" Neighbour:"); Serial.println(Neighbour);
  Serial.print("Inscope:"); Serial.print(Inscope);
  Serial.print(" FaceTop:"); Serial.print(FaceTop);
  Serial.print(" FaceBot:"); Serial.print(FaceBot);
  Serial.print(" Tangage:"); Serial.println(Tangage);
  Serial.print("Couleurs Faces: ");
  for (int f = 0; f < 6; f++)
  {
    Serial.print(Face[f].Couleur);
    Serial.print("|");
  }
  Serial.println();
  Serial.print("Touch: ");
  for (int f = 0; f < 6; f++)
  {
    Serial.print(Face[f].Touch);
    Serial.print("|");
  }
  Serial.println();

}

Slave::ReadState(void)
{
  TalkToSlave(Addr, "TotState", MAXRETRY);

  ///Mise a jour les attributs du Slave
  int Touch = Param[2];
  if (Touch & 1) Face[0].Touch = true;
  else Face[0].Touch = false;
  if (Touch & 2) Face[1].Touch = true;
  else Face[1].Touch = false;
  if (Touch & 4) Face[2].Touch = true;
  else Face[2].Touch = false;
  if (Touch & 8) Face[3].Touch = true;
  else Face[3].Touch = false;
  if (Touch & 16) Face[4].Touch = true;
  else Face[4].Touch = false;
  if (Touch & 32) Face[5].Touch = true;
  else Face[5].Touch = false;

  FaceTop = Param[3];
  FaceBotUpdate();
  Tangage = Param[4];
  //RFID = Answer[5];
}

Slave::ReadRFID(void)
{
  TalkToSlave(Addr, "FaceID", MAXRETRY);
  RFID = Answer[2];
  Neighbour = NeighbourFind(RFID);
}

Slave::Vibre(void)
{
  TalkToSlave(Addr, "Vibre,3", MAXRETRY);
}

Slave::LightFace(int Face, int Palette)
{
  int Temp[] = {NOIR, NOIR, NOIR, NOIR, NOIR, NOIR};
  Temp[Face] = Palette;
  String Cmd = "Col,";
  Cmd += String(Temp[0]);
  Cmd += ",";
  Cmd += String(Temp[1]);
  Cmd += ",";
  Cmd += String(Temp[2]);
  Cmd += ",";
  Cmd += String(Temp[3]);
  Cmd += ",";
  Cmd += String(Temp[4]);
  Cmd += ",";
  Cmd += String(Temp[5]);
  TalkToSlave(Addr, Cmd, MAXRETRY);
}

Slave::Colorie(int F0, int F1, int F2, int F3, int F4, int F5, bool Apply)
{
  Face[0].Couleur = F0;
  Face[1].Couleur = F1;
  Face[2].Couleur = F2;
  Face[3].Couleur = F3;
  Face[4].Couleur = F4;
  Face[5].Couleur = F5;

  if (Apply)
  {
    String Cmd = "Col,";
    Cmd += String(F0);
    Cmd += ",";
    Cmd += String(F1);
    Cmd += ",";
    Cmd += String(F2);
    Cmd += ",";
    Cmd += String(F3);
    Cmd += ",";
    Cmd += String(F4);
    Cmd += ",";
    Cmd += String(F5);

    TalkToSlave(Addr, Cmd, MAXRETRY);
  }
}

Slave::FaceBotUpdate(void)
{
  switch (FaceTop)
  {
    case 0: FaceBot = 3;
      break;
    case 1: FaceBot = 4;
      break;
    case 2: FaceBot = 5;
      break;
    case 3: FaceBot = 0;
      break;
    case 4: FaceBot = 1;
      break;
    case 5: FaceBot = 2;
      break;
  }
}
//*****************************
char NeighbourFind(String RFID)
{
  extern Slave TotA, TotB, TotC, TotD;
  char Result = '*';
  if (RFID == TotA.ID) Result = 'A';
  if (RFID == TotB.ID) Result = 'B';
  if (RFID == TotC.ID) Result = 'C';
  if (RFID == TotD.ID) Result = 'D';
  return (Result);
}

