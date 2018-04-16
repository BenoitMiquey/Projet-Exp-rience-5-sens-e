#include "ifDef.h"
#include "CSlave.h"
#include "stdlib.h"
#include "LinkedList.h"
extern Adafruit_VS1053_FilePlayer mp3;
 LinkedList<int> suiteFaces = LinkedList<int>();
extern String Answer[SPLITTERS];
extern int Param[SPLITTERS];
String message = "";
int sablier;
int suitePosition;
static int current;
String CodeRFID[] = {"", "", "", ""};
extern Slave TotA, TotB, TotC, TotD;
extern Slave Tot[4];
int faceClicked;
String msgLog;
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
int lastFaceClicked; // la   dernière face qui a été clickée ( pratique pour la remettre en couleur au click suivant
struct color Palette[16];
enum FacesList {FACE_0, FACE_1, FACE_2, FACE_3, FACE_4, FACE_5, FACE_ALL};
extern Adafruit_VS1053_FilePlayer mp3;
extern String Answer[SPLITTERS];
extern int Param[SPLITTERS];
extern RTC_DS1307 rtc;
// le token va servir de jeton de position, pour savoir quelle action est a effectuer
// chaques méthode le changera de manière a donner la main a une autre methode
int token = 0;

//  Fontcion pour faire clignotter une face
void blinkFace(int face ,color couleur)
{
  message = "FaceLed,";
  message += face;
  message +=",20,";
  message += couleur.R;
  message +=",";
  message +=couleur.G;
  message +=",";
  message +=couleur.B;
  TalkToSlave('A',message,MAXRETRY); 
}
// fonction pour colorier une face
void colorFace(int face, color couleur)
{
  
  message = "FaceLed,";
  message += face;
  message +=",1,";
  message += couleur.R;
  message +=",";
  message +=couleur.G;
  message +=",";
  message +=couleur.B;
  TalkToSlave('A',message,MAXRETRY); 
}
// choisir une face de manière presque aléatoire ( fonction juste pour le premier choix )
int chooseAFace()
{
  Tot[0].ReadState();
  DateTime now = rtc.now();
  randomSeed(now.second());
  int a;
  do {
    a =random(0,6);
  }
  while( a == Tot[0].FaceBot );
  msgLog = "Face attendue :";
  msgLog +=a;
  LogPlay(msgLog);
  return a;
}
// Fonction de choix d'une face a proximité
int chooseANeighbourFace()
{ 
  DateTime now = rtc.now();
  randomSeed(now.second());
  int a,b;
  int result;
  Tot[0].ReadState();
  do {
    a =random(0,4);
    b = random ( 0,2);
    a= a -2;
    result = abs((suiteFaces.get((suiteFaces.size()-1))-a)%6);
  }
  while ( result == Tot[0].FaceBot || (result ==(suiteFaces.get((suiteFaces.size()-1) ))) );
  msgLog = "Face attendue :";
  msgLog +=result;
  LogPlay(msgLog);
  return result; 
}
// Fonction pour areter le clignottement d'une face
void unBlinkFace(int face, color couleur)
{
  message = "FaceLed,";
  message += face;
  message +=",1,";
  message += couleur.R;
  message +=",";
  message +=couleur.G;
  message +=",";
  message +=couleur.B;
  TalkToSlave('A',message,MAXRETRY);
}
// fonction pour actualiser la couleur d'une face unique stable ( cette fonction n'est plus utilisée)
void totDisplayFaces(int face, color couleur)
{
  message = "FaceLed,";
  message += face;
  message +=",1,";
  message += couleur.R;
  message +=",";
  message +=couleur.G;
  message +=",";
  message +=couleur.B;
  TalkToSlave('A',message,MAXRETRY); 
}
// equivalent de la fonction main
void NewSimonPlay(){
  // fonctionnement par jetton, de base il vaut 0 pour initialiser la partie
  switch(token)
  {
    case 0: // si on est dans l'état d'initialisation du jeu
      LogPlay("PREPARATION DU JEU");
    //mp3.playFullFile("Start.mp3");
      PaletteInit(); // On initialise la palette de couleurs
      colorFaces[0] = Palette[BLEU];
      colorFaces[1] = Palette[ROSE];
      colorFaces[2] = Palette[JAUNE];
      colorFaces[3] = Palette[ORANGE];
      colorFaces[4] = Palette[CYAN];
      colorFaces[5] = Palette[VIOLET];
      message = "Animation,0";
      TalkToSlave('A', "BGcolor,0,0,0", MAXRETRY);
      TalkToSlave('A',message,MAXRETRY);
      Tot[0].Colorie( BLEU,ROSE,JAUNE,ORANGE,CYAN,VIOLET,1);
      token = 1; // on donne la main a lajout de faces dans la lise
    break;
    case 1: // on ajoute une face aleatoire a la liste chainee
      //delay(random(0,40));
      if (suiteFaces.size() == NULL)
      {
        suiteFaces.add(chooseAFace());
      }
      else 
      {
        suiteFaces.add(chooseANeighbourFace());
      }
      token = 2; // on donne la main au blink 
    break;
    case 2: // on va faire clignotter chaques faces
      for ( int i=0; i < suiteFaces.size(); i++)
      {
        blinkFace(suiteFaces.get(i),colorFaces[suiteFaces.get(i)]); // on fait clignotter la face
        delay(600); // on fait durer un peut le clignottement
        unBlinkFace(suiteFaces.get(i), colorFaces[suiteFaces.get(i)]); // on arrete le clignottement
      }
      token = 3; // on donne la main a l'attente d'appuis sur une touche
      suitePosition = 0; // on met notre position a la première face de la suite
      break;
      case 3: // on capte// attend un click 
        Tot[0].ReadState();
        for(int i=0; i <6; i++)
        {
          if (Tot[0].Face[i].Touch){
            faceClicked = i;
            token = 4;
          }
        }
      break;
      case 4:  // analyse du click
        if(faceClicked == suiteFaces.get(suitePosition)){ // si on a bien cliqué
          TalkToSlave('A', "Vibre,17", MAXRETRY);
          colorFace(faceClicked,Palette[VERT]); // ensuite on colorie la face en vert ( la couleur a dailleur ete enlevee )
          delay(200);
          colorFace(faceClicked,colorFaces[faceClicked]);
          faceClicked = -1; // on ddeclique la face 
          token = 5;
        }
        else  // ou bien si on a appuyé sur la mauvaise face
        {
           token = 6;
           //mp3.playFullFile("Fail.mp3");
        }
      break;
      case 5: // après la validation de la case
        if ( suitePosition == (suiteFaces.size()-1))
        {
            token =1;
        }
        else
        {
          ++suitePosition;
          token = 3;
        }
      break;
      case 6: // mauvais appuis sur une face
        msgLog ="PERDU SCORE :";
        msgLog +=(suiteFaces.size());
        LogPlay(msgLog);
        suiteFaces.clear();
        TalkToSlave('A', "Vibre,80", MAXRETRY);
        Tot[0].Colorie(ROUGE, ROUGE, ROUGE, ROUGE, ROUGE, ROUGE, 1);
        delay(1500);
        Tot[0].Colorie(NOIR, NOIR, NOIR, NOIR, NOIR, NOIR, 1);
        //mp3.playFullFile("Fail.mp3");
        token = 0;
        
      break;
  }
}

// méthodes
// méthode pour choisir une face au hasard pour commencer
// initialisation palette
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


