#include "ifDef.h"
#include "CSlave.h"
#include "stdlib.h"
#include "LinkedList.h"
int FocusSlave ;
String music;
int sum;
int num;
int sol;
char op;
int k;
#define NBSLAVE 2 // nombre de slaves participant, au moins un chiffré et un avec des signes 

extern Adafruit_VS1053_FilePlayer mp3;

String message = "";
String CodeRFID[] = {"", "", "", ""};
extern Slave TotA, TotB, TotC, TotD;
extern Slave Tot[4];
int faceClicked; // contient le numéro de la face cliquée
int slaveClicked; // contient le numéro du slave cliqué
String msgLog; // contient le message a envoyer dans le fichier de log
// la liste chainée qui contient des elements de type SimonStep
struct CalculStep // la structure des éléments de la liste des actions effectués par la personne
{
  int Slave;
  char Operator; // l'opérateur  0 si rien, + si plus et - si moins
  int Number; // le numéro de la face sera number -1
};
LinkedList<CalculStep> ActionsSuite= LinkedList<CalculStep>(); 
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
struct CalculStep gameStep;

int chooseANumber() // retourne un nombre aléatoire entre 2 et 12, qui devra etre calculé par la personne
{
  DateTime now = rtc.now();
  randomSeed(now.second());
  return random(5,13);
}
bool pingSlave(int Slave)// permet de savoir si le slave est actif
{
  return true;
}
void addASlaveToSuite(int Slave)// ajoute u slave a la liste
{
  
}
long test;
void playsong(int i)
{
  switch(i)
  {
    
    case 1:
    mp3.playFullFile("un.mp3");
    break;
    case 2:
    mp3.playFullFile("deux.mp3");
    break;
    case 3:
    mp3.playFullFile("trois.mp3");
    break;
    case 4:
    mp3.playFullFile("quatre.mp3");
    break;
    case 5:
    mp3.playFullFile("cinq.mp3");
    break;
    case 6:
    mp3.playFullFile("six.mp3");
    break;
    case 7:
    mp3.playFullFile("sept.mp3");
    break;
    case 8:
    mp3.playFullFile("huit.mp3");
    break;
    case 9:
    mp3.playFullFile("neuf.mp3");
    break;
    case 10:
    mp3.playFullFile("dix.mp3");
    break;
    case 11:
    mp3.playFullFile("onze.mp3");
    break;
    case 12:
    mp3.playFullFile("douze.mp3");
    break;
    case 13:
    mp3.playFullFile("plus.mp3");
    break;
    case 14:
    mp3.playFullFile("moins.mp3");
    break;
    case 15: 
    mp3.playFullFile("egal.mp3");
    break;
    
  }
}
void ChangeFocus()
{
  if (FocusSlave == 1 )
  {
    FocusSlave = 0;
  }
  else FocusSlave =1;
}
void CalculPlay()
{
  switch(token)
  {
    case 0: // création de la suite d'emboitement des slaves
        sol = chooseANumber();
        playsong(sol);
        //Tot[0].ReadState();
        //Serial.print(" rfid : "+Tot[0].RFID);
        Tot[1].Colorie( NOIR,NOIR,NOIR,NOIR,NOIR,NOIR,1);
        Tot[0].Colorie( JAUNE , ORANGE,VIOLET, BLEU,ROSE, CYAN,1);
        Serial.print("test");
        token = 1;
        FocusSlave = 0;
    break;
    case 1: // ici on ecoute les clics des chiffres

      Tot[FocusSlave].ReadState();
      for (int j=0; j < 6; j ++)
      {
        if ( Tot[FocusSlave].Face[j].Touch)
        {
          faceClicked = j;
          slaveClicked = 0;
          Tot[FocusSlave].Colorie( NOIR,NOIR,NOIR,NOIR,NOIR,NOIR,1);
          if ( FocusSlave == 0)
          {
            gameStep.Number = faceClicked +1;
            gameStep.Operator = '0';
            gameStep.Slave = FocusSlave;
            
          }
          else
          {
            gameStep.Number = 0;
            if(faceClicked%2 == 0)
            {
              gameStep.Operator = '+';
            }
            else
            {
              gameStep.Operator = '-';
            }
          }
          
          ChangeFocus();
          if ( FocusSlave == 1)
          {
            Tot[FocusSlave].Colorie( VERT, ROUGE,VERT, ROUGE,VERT, ROUGE,1);
          }
          else
          {
            Tot[FocusSlave].Colorie( JAUNE , ORANGE,VIOLET, BLEU,ROSE, CYAN,1);
          }
          ActionsSuite.add(gameStep);
          token = 2;
          break;
        }
      }

    break;
    case 2:
      sum = ActionsSuite.get(0).Number;
      num =  0;
      op = ' ';
      for( int k=0; k < ActionsSuite.size();k++)
      {
         if( k == 0)
         {
          num = ActionsSuite.get(0).Number;
         }
         else
         {
          if( (ActionsSuite.get(k).Number == 0) && (k != (ActionsSuite.size()-1) ) )
          {
            if ( strncmp( ActionsSuite.get(k).Operator , '-',1))
            {
              sum = num + sum;
            }
            else
            {
              sum = sum - num;
            }
          }
          else
          {
            num = ActionsSuite.get(k).Number;
          }
        }
      }
      Serial.print(sum);
      
      if(( sum != sol) && ( sum > sol))
      {
        token = 5; 
      }
      else
      {
        token = 3;
      }
      if (sum == sol)
      {
        token = 4;
      }
    break;
    case 3:
        if ( FocusSlave == 1)
        {
          playsong((faceClicked + 1));
          
        }
        else
        {
          playsong(((faceClicked)%2)+13);
        }


      token = 1;
    break;
    case 4:
    Serial.print("gagne!!!!");
     ActionsSuite.clear();
     Tot[0].Colorie(VERT, VERT, VERT, VERT, VERT, VERT, 1);
     Tot[1].Colorie(VERT, VERT, VERT, VERT, VERT, VERT, 1);
      mp3.playFullFile("Winning.mp3");
      delay(500);
      token = 0;
    break;
    case 5:
       ActionsSuite.clear();
       Tot[0].Colorie(ROUGE, ROUGE, ROUGE, ROUGE, ROUGE, ROUGE, 1);
       Tot[1].Colorie(ROUGE, ROUGE, ROUGE, ROUGE, ROUGE, ROUGE, 1);
      //mp3.playFullFile("Fail.mp3");
       delay(500);
      token = 0;
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


