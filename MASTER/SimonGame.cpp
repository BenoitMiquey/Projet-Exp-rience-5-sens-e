//#include "ifDef.h"
//#include "CSlave.h"
//
//#define PROFONDEUR 100 // Le nombre max de face a appuyer a la suite
//
//extern Adafruit_VS1053_FilePlayer mp3;
//
//extern String Answer[SPLITTERS];
//extern int Param[SPLITTERS];
//extern RTC_DS1307 rtc;
//
////extern Slave TotA, TotB, TotC, TotD;
//extern Slave Tot[4];
//
//extern void DefineSimonSuite(void);
//
//struct card
//{
//  int Animal;
//  int PaletteColor;
//  bool Drawn;
//};
//struct card Deck[36];
//
//struct Steps
//{
//  char Slave;
//  int Face;
//  int PaletteColor;
//};
//struct Steps Step[4];
//String CodeRFID[] = {"", "", "", ""};
//int SimonSuite[PROFONDEUR];
//
//struct color
//{
//  int R;
//  int G;
//  int B;
//};
//
//struct color Palette[16];
//
//
//void DefineSteps(bool Random);
//
////***************************************************************************
//void LogPlay(String Message)
//{
//  Serial.println(Message);
//  SDlogWrite(-1, Message);
//}
//
////***************************************************************************
//void SimonPlay()
//{
//  static int Play = -1;
//  static int PreviousPlay = -2;
//  static bool Change = true;
//  String Message = "";
//  String Log="";
//  enum Faces {FACE_0, FACE_1, FACE_2, FACE_3, FACE_4, FACE_5, FACE_ALL};
//  PreviousPlay = Play;
//  static int varB, varC, varX;
//  int Cligno = 10;
//  static long Timer = 0;
//
//  switch (Play)
//  {
//    case -1:
//      LogPlay("INITIALISATION DU GAMEPLAY");
//      Tot[0].Colorie(BLEU, VIOLET, JAUNE, ORANGE, MARRON, VERT, 1);
//      Play = 0;
//      break;
//
//    case 0:
//      if (Change)
//      {
//        LogPlay("ATTENTE BOUTON START");
//        digitalWrite(LED, HIGH);
//      }
//      if (digitalRead(BUTTON))
//      {
//        LogPlay("STARTED !");
//        Play = 1;
//        DefineSimonSuite();
//        digitalWrite(LED, LOW);
//        varB = 0;
//        varC = 0;
//        TalkToSlave('A', "Vibre,70", MAXRETRY);
//      }
//      break;
//
//    case 1:
//      varX = SimonSuite[varB];
//      Message = "FaceLed,";
//      Message += + varX;
//      Message += ",";
//      Message += Cligno;
//      Message += ",";
//      Message += Palette[Tot[0].Face[varX].Couleur].R;
//      Message += ",";
//      Message += Palette[Tot[0].Face[varX].Couleur].G;
//      Message += ",";
//      Message += Palette[Tot[0].Face[varX].Couleur].B;
//      TalkToSlave('A', Message, MAXRETRY);
//      Serial.print("Index=");
//      Serial.print(varB);
//      Serial.print(",face=");
//      Serial.println(varX);
//      delay(1200);
//      Play = 2;
//      break;
//
//    case 2:
//      varB++;
//      if (varB > varC)
//      {
//        Play = 3;
//        varB = 0;
//      }
//      else Play = 1;
//      Tot[0].Colorie(BLEU, VIOLET, JAUNE, ORANGE, MARRON, VERT, 1);
//      break;
//
//    case 3:
//      if (Change)
//      {
//        varX = SimonSuite[varB];
//      }
//      Tot[0].ReadState();
//      if (Tot[0].Face[varX].Touch)
//      {
//        TalkToSlave('A', "Vibre,10", MAXRETRY);
//        Tot[0].Colorie(BLEU, VIOLET, JAUNE, ORANGE, MARRON, VERT, 1);
//        Message = "FaceLed,";
//        Message += + varX;
//        Message += ",1,255,255,255";
//        TalkToSlave('A', Message, MAXRETRY);
//        Serial.print("Index=");
//        Serial.print(varB);
//        Serial.print(",face=");
//        Serial.println(varX);
//        Play = 4;
//      }
//      for (int T = 0; T < 6; T++)
//      {
//        if ((Tot[0].Face[T].Touch) && (T != varX)) Play = 6;
//      }
//      break;
//
//    case 4:
//      varB++;
//      if (varB > varC) Play = 5;
//      else  Play = 3;
//      break;
//
//    case 5:
//      Log="gagne :-) Score=";
//      Log+=varC+1;
//      LogPlay(Log);
//      Tot[0].Colorie(BLANC, BLANC, BLANC, BLANC, BLANC, BLANC, 1);
//      mp3.playFullFile("Winning.mp3");
//      Tot[0].Colorie(BLEU, VIOLET, JAUNE, ORANGE, MARRON, VERT, 1);
//      varC++;
//      varB = 0;
//      if (varC < PROFONDEUR) Play = 1;
//      else
//      {
//        Tot[0].Colorie(NOIR, NOIR, NOIR, NOIR, NOIR, NOIR, 1);
//        delay(500);
//        Play = -1;
//      }
//      break;
//
//    case 6:
//      Log="perdu :-( Score=";
//      Log+=varC+1;
//      LogPlay(Log);
//      Tot[0].Colorie(ROUGE, ROUGE, ROUGE, ROUGE, ROUGE, ROUGE,  1);
//      mp3.playFullFile("Losing.mp3");
//      Tot[0].Colorie(NOIR, NOIR, NOIR, NOIR, NOIR, NOIR, 1);
//      delay(500);
//      Play = -1;
//
//      break;
//  }
//
//  Timer++;
//  if (PreviousPlay != Play)
//  {
//    Change = true;
//    Timer = 0;
//  }
//  else
//  {
//    Change = false;
//    Timer++;
//  }
//}
//
//void DefineSimonSuite()
//{
//  static int Previous = -1;
//  int Tirage;
//
//  DateTime now = rtc.now();
//  randomSeed(now.second());
//  Serial.println("Tirage de la suite du SIMON:");
//  for (int i = 0; i < PROFONDEUR; i++)
//  {
//    do
//    {
//      Tirage = random(0, 6);
//    }  while (Tirage == Previous);
//
//    SimonSuite[i] = Tirage;
//    Previous = Tirage;
//
//    Serial.print(i);
//    Serial.print(":");
//    Serial.println(SimonSuite[i]);
//  }
//}
//
////*******************************************************************************
//
//
//void PaletteInit()
//{
//  Palette[BLEU].R = 0;
//  Palette[BLEU].G = 0;
//  Palette[BLEU].B = 255;
//  Palette[VERT].R = 0;
//  Palette[VERT].G = 255;
//  Palette[VERT].B = 50;
//  Palette[JAUNE].R = 255;
//  Palette[JAUNE].G = 200;
//  Palette[JAUNE].B = 0;
//  Palette[ORANGE].R = 255;
//  Palette[ORANGE].G = 80;
//  Palette[ORANGE].B = 0;
//  Palette[ROUGE].R = 255;
//  Palette[ROUGE].G = 0;
//  Palette[ROUGE].B = 0;
//  Palette[VIOLET].R = 255;
//  Palette[VIOLET].G = 0;
//  Palette[VIOLET].B = 255;
//  Palette[BLANC].R = 255;
//  Palette[BLANC].G = 255;
//  Palette[BLANC].B = 255;
//  Palette[NOIR].R = 0;
//  Palette[NOIR].G = 0;
//  Palette[NOIR].B = 0;
//  Palette[ROSE].R = 255;
//  Palette[ROSE].G = 50;
//  Palette[ROSE].B = 50;
//  Palette[MARRON].R = 35;
//  Palette[MARRON].G = 10;
//  Palette[MARRON].B = 3;
//}
//
//void ColorieFace(char Tot, int Face, int Couleur, int Mode)
//{
//  String Stemp = "FaceLed,";
//  Stemp += String(Face);
//  Stemp += ",";
//  Stemp += String(Mode);
//  Stemp += ",";
//  Stemp += String(Palette[Couleur].R);
//  Stemp += ",";
//  Stemp += String(Palette[Couleur].G);
//  Stemp += ",";
//  Stemp += String(Palette[Couleur].B);
//  TalkToSlave(Tot, Stemp, MAXRETRY);
//}

