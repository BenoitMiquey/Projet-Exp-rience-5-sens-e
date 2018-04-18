#include "ifDef.h"
#include "CSlave.h"
#include "stdlib.h"
#include "LinkedList.h"


#define NBSLAVE 2 // le nombre de slaves participant au jeu


extern Adafruit_VS1053_FilePlayer mp3;
String message = "";
int suitePosition;
String CodeRFID[] = {"", "", "", ""};
extern Slave TotA, TotB, TotC, TotD;
extern Slave Tot[4];
int faceClicked; // contient le numéro de la face cliquée 
int slaveClicked; // contient le numéro du slave cliqué
String msgLog; // contient le message a envoyer dans le fichier de log

// variables temporaires pour les choix de face aléatoire
int tmpSlave;
int tmpFace;

// structure du maillon de notre liste chainée qui contient le slave et la face à appuyer
struct SimonStep
{
  int Slave;
  int Face;
};

// la liste chainée qui contient des elements de type SimonStep
LinkedList<SimonStep> SimonSuite= LinkedList<SimonStep>();

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

//  Fontcion pour faire clignotter une face
void blinkFace(int face ,color couleur, char Slave)
{
  message = "FaceLed,";
  message += face;
  message +=",20,";
  message += couleur.R;
  message +=",";
  message +=couleur.G;
  message +=",";
  message +=couleur.B;
  TalkToSlave(Slave,message,MAXRETRY); 
}
// fonction pour colorier une face
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
// choisir une face de manière presque aléatoire ( fonction juste pour le premier choix )
int chooseAFace(int Slave)
{
  Tot[Slave].ReadState();
  DateTime now = rtc.now();
  randomSeed(now.second());
  int a;
  do {
    a =random(0,6);
  }
  while( a == Tot[Slave].FaceBot );
  msgLog = "Face attendue :";
  msgLog +=a;
  LogPlay(msgLog);
  return a;
}
int chooseASlave()
{
  DateTime now = rtc.now();
  randomSeed(now.second());
  return random(0, NBSLAVE*100)%NBSLAVE;
}
char intSlaveToChar(int Slave)
{
  if (Slave == 0){
    return 'A';
  }
  if (Slave == 1){
    return 'B';
  }
  if (Slave == 2){
    return 'C';
  }
  if (Slave == 3){
    return 'D';
  }
}

// Fonction de choix d'une face a proximité
int chooseANeighbourFace(int Slave)
{ 
  DateTime now = rtc.now();
  randomSeed(now.second());
  int a,b;
  int result;
  Tot[Slave].ReadState();
  do {
    a =random(0,4);
    b = random ( 0,2);
    a= a -2;
    result = abs((SimonSuite.get((SimonSuite.size()-1)).Face-b*a)%6);
  }
  while ( result == Tot[0].FaceBot || (result ==(SimonSuite.get((SimonSuite.size()-1) ).Face)) );
  msgLog = "Face attendue :";
  msgLog +=result;
  LogPlay(msgLog);
  return result; 
}
// Fonction pour areter le clignottement d'une face
void unBlinkFace(int face, color couleur, char Slave)
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
      if ( NBSLAVE > 1)
      {
          TalkToSlave('B', "BGcolor,0,0,0", MAXRETRY);
          TalkToSlave('B',message,MAXRETRY);
          Tot[1].Colorie( BLEU,ROSE,JAUNE,ORANGE,CYAN,VIOLET,1);
      }
      if ( NBSLAVE > 2)
      {
          TalkToSlave('C', "BGcolor,0,0,0", MAXRETRY);
          TalkToSlave('C',message,MAXRETRY);
          Tot[2].Colorie( BLEU,ROSE,JAUNE,ORANGE,CYAN,VIOLET,1);
      }
      if ( NBSLAVE > 3)
      {
          TalkToSlave('D', "BGcolor,0,0,0", MAXRETRY);
          TalkToSlave('D',message,MAXRETRY);
          Tot[3].Colorie( BLEU,ROSE,JAUNE,ORANGE,CYAN,VIOLET,1);
      }



      token = 1; // on donne la main a lajout de faces dans la lise
    break;
    case 1: // on ajoute une face aleatoire a la liste chainee
      tmpSlave = chooseASlave();
      if ((SimonSuite.size() == NULL) || ( tmpSlave != SimonSuite.get(SimonSuite.size() -1).Slave))
      {
        tmpFace =chooseAFace(tmpSlave);
      }
      else 
      {
        tmpFace = chooseANeighbourFace(tmpSlave);
      }
      struct SimonStep tmpStep ;
      tmpStep.Slave =tmpSlave;
      tmpStep.Face = tmpFace;
      SimonSuite.add(tmpStep);
      token = 2; // on donne la main au blink 
    break;
    case 2: // on va faire clignotter chaques faces
      for ( int i=0; i < SimonSuite.size(); i++)
      {
        blinkFace(SimonSuite.get(i).Face,colorFaces[SimonSuite.get(i).Face],intSlaveToChar( SimonSuite.get(i).Slave)); // on fait clignotter la face
        delay(600); // on fait durer un peut le clignottement
        unBlinkFace(SimonSuite.get(i).Face,colorFaces[SimonSuite.get(i).Face],intSlaveToChar( SimonSuite.get(i).Slave)); // on arrete le clignottement
      }
      token = 3; // on donne la main a l'attente d'appuis sur une touche
      suitePosition = 0; // on met notre position a la première face de la suite
      break;
      case 3: // etat d'attente de clic ( on balaye chaque totemigo afin de savoir si une face est cliquée )
        for( int j=0; j<NBSLAVE; j++)
        {
          //Serial.print("reading...");
          Tot[j].ReadState();
          for(int i=0; i < 6; i++)
          {
            if (Tot[j].Face[i].Touch)
            {
              faceClicked = i;
              slaveClicked = j;
              token = 4;
              Serial.print("click!");
              break;
            }
          }
        }
      break;

      
      case 4:  // analyse du clic
        if( (faceClicked == SimonSuite.get(suitePosition).Face) && (slaveClicked == SimonSuite.get(suitePosition).Slave )) // si on a cliqué sur la bonne face du bon slave
        {
          TalkToSlave(intSlaveToChar(SimonSuite.get(suitePosition).Slave), "Vibre,17", MAXRETRY);
          colorFace(faceClicked,Palette[VERT],intSlaveToChar(SimonSuite.get(suitePosition).Slave) ); // ensuite on colorie la face en vert ( la couleur a dailleur ete enlevee )
          delay(200);
          colorFace(faceClicked,colorFaces[faceClicked], intSlaveToChar(SimonSuite.get(suitePosition).Slave));
          faceClicked = -1; // on declique la face 
          slaveClicked = -1;
          token = 5;
        }
        else  // ou bien si on a appuyé sur la mauvaise face
        {
           token = 6;
           //mp3.playFullFile("Fail.mp3");
        }
      break;
      case 5: // après la validation de la case
        if ( suitePosition == (SimonSuite.size()-1))
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
        msgLog +=(SimonSuite.size());
        LogPlay(msgLog);
        SimonSuite.clear();
        for ( int i=0; i < NBSLAVE; i ++)
        {
          TalkToSlave(intSlaveToChar(i), "Vibre,80", MAXRETRY);
          Tot[i].Colorie(ROUGE, ROUGE, ROUGE, ROUGE, ROUGE, ROUGE, 1);
        }
        delay(1500);
        for ( int i=0; i < NBSLAVE; i ++)
        {
          Tot[i].Colorie(NOIR, NOIR, NOIR, NOIR, NOIR, NOIR, 1);
        }
        //mp3.playFullFile("Fail.mp3");  // on joue un son de fail
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


