
#include "ifDef.h"
#include "CSlave.h"
#include "stdlib.h"
#include "LinkedList.h"

int FocusSlave ;
String music;
extern Adafruit_VS1053_FilePlayer mp3;
char SlaveToChar[] = {'A','B','C'};
String message = "";
String CodeRFID[] = {"", "", "", ""};
extern Slave TotA, TotB, TotC, TotD;
extern Slave Tot[4];
String msgLog; // contient le message a envoyer dans le fichier de log
// la liste chainée qui contient des elements de type SimonStep
// fonction pour enregistrer dans le fichier log
void LogPlay(String Message)
{
  Serial.println(Message);
  SDlogWrite(-1, Message);
}

struct color
{
  int R;
  int G;
  int B;
};
color ColorSlavesPatern[3][6];
color colorFaces[6];
struct color Palette[16];
enum FacesList {FACE_0, FACE_1, FACE_2, FACE_3, FACE_4, FACE_5, FACE_ALL};
extern Adafruit_VS1053_FilePlayer mp3;
extern String Answer[SPLITTERS];
extern int Param[SPLITTERS];
extern RTC_DS1307 rtc;
// le token va servir de jeton de position, pour savoir quelle action est a effectuer
// chaques méthode le changera de manière a donner la main a une autre methode
int token = 0;
String ligne;
void colorFace(int face, color couleur, char Slave)
{
  
  message = "FaceLed,";
  message += face;
  message +=",1,";
  message += couleur.R;
  message +=",";
  message +=couleur.G;
  message +=",";
  message +=couleur.B;
  TalkToSlave(Slave,message,MAXRETRY); 
}
void readLog()
{
  File myFile;
  myFile = SD.open("Moule.txt");
  if (myFile)
  {
    Serial.print("lecture réussie");

      ligne =  myFile.readStringUntil('\n');
      //Serial.println(myFile.readStringUntil('\n'));

      myFile.close();

  }
  else Serial.println("error opening Log.txt");
}


void Play()
{
  switch(token)
  {
    case 0:
    readLog();
          Serial.println(ligne);
    token =1;
    break;
  }
  
}





// initialisation palette, on peut biensur ajouter des couleurs si besoin
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
  Palette[CYAN].R = 66;
  Palette[CYAN].G = 245;
  Palette[CYAN].B = 245;
  
}

