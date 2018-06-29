
#include "ifDef.h"
#include "CSlave.h"
#include "stdlib.h"
#include "LinkedList.h"
String State;
int FocusSlave ;
String music;
bool win = 0;
extern Adafruit_VS1053_FilePlayer mp3;
char SlaveToChar[] = {'A','B','C'};
String message = "";
String CodeRFID[] = {"", "", "", ""};
extern Slave TotA, TotB, TotC, TotD;
extern Slave Tot[4];
String msgLog; // contient le message a envoyer dans le fichier de log
// la liste chainée qui contient des elements de type SimonStep
 //fonction pour enregistrer dans le fichier log
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
int winAction[5];
int CursorSols = 0;
color ColorSlavesPatern[3][6];
color colorFaces[6];
struct color Palette[16];
int FaceClicked;
int SlaveClicked;
enum FacesList {FACE_0, FACE_1, FACE_2, FACE_3, FACE_4, FACE_5, FACE_ALL};
extern Adafruit_VS1053_FilePlayer mp3;
extern String Answer[SPLITTERS];
extern int Param[SPLITTERS];
extern RTC_DS1307 rtc;
// le token va servir de jeton de position, pour savoir quelle action est a effectuer
// chaques méthode le changera de manière a donner la main a une autre methode
int token = 0;
File myFile;
String ligne;
int conditions[10];
int nbConditions;
int Solutions [3][8];
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
String parseLigne[10];
String*  getCommands(String text)
{
  String commands[10];
  int i=-1;
  int j= 0;
  int minicpt=0;
  String current;
  
  do{
    i++;
    if(text[i] == ':' or text[i] == ';' )
    {
      Serial.println("current : "+current);
      parseLigne[j] = current;
      j ++;
      minicpt=0;
      current = "";
    }
    else
    {
      current = current +text[i];
      Serial.println(text[i]);
      minicpt = minicpt+1;
    }
    
  }while (text[i] != ';');
  
  return commands;
}
String tmp;
File openFile()
{
  
  return SD.open("Moule.txt");
}

/*
void InitVibre(int Slave, bool state)
{
  Serial.println(state);
  if(state) Serial.println("true VIBRE");
  message = "Animation,2,0,0,0,"+(String)state;
  if(Slave == 3)
  {
    
    for(int i = 0; i<3; i++)
    {
      TalkToSlave(SlaveToChar[i],message, MAXRETRY);
      TalkToSlave(SlaveToChar[i],"Animation,0", MAXRETRY);
    }
  }
  else
  {
    TalkToSlave(SlaveToChar[Slave],message,MAXRETRY);
    TalkToSlave(SlaveToChar[Slave],"Animation,0", MAXRETRY);
  }
}*/
void initVibreRotation(int Slave, int dose)
{
    Serial.println("Paramètrage vibration");
  if( Slave == 3)
  {
    for( int i=0; i<3;i++)
    {
      TalkToSlave(SlaveToChar[i],"SetVibration,"+(String)dose,MAXRETRY);
      delay(40);
    }
     
  }
  else
  {
    TalkToSlave(SlaveToChar[Slave],"SetVibration,"+(String)dose,MAXRETRY);
  }

}
void InitColor(int Slave, int Face, int R, int G, int B)
{
  Serial.println("COLORIAGE !!!!");
  int a,b,c,d;
  if(Slave == 3 && Face == 6)
  {
    for( int i=0; i < 3; i++)
    {
      for(int j=0; j <6; j++)
      {
        colorFace(j, {R,G,B},SlaveToChar[i]);
      }
    }
  }
  else
  {
    if ( Face == 6)
    {
      for(int j=0; j <6; j++)
      {
        colorFace(j, {R,G,B},SlaveToChar[Slave]);
      }
    }
    else
    {
      colorFace(Face, {R,G,B},SlaveToChar[Slave]);
    }
    
  }
  
  
  
}

void readFile(File myFile)
{

  if (myFile)
  {
    Serial.println("lecture réussie");

      ligne =  myFile.readStringUntil('\n');
      myFile.close();

  }
  else Serial.println("error opening Log.txt");
}


void Play()
{
  switch(token)
  { 
    case 0:
      TalkToSlave('A',"Animation,0", MAXRETRY);
      delay(100);
      TalkToSlave('B',"Animation,0", MAXRETRY);
      delay(100);
      TalkToSlave('C',"Animation,0", MAXRETRY);
      delay(100);
      /*TalkToSlave('A', "BGcolor,0,0,0", MAXRETRY);
      TalkToSlave('B', "BGcolor,0,0,0", MAXRETRY);
      TalkToSlave('C', "BGcolor,0,0,0", MAXRETRY);*/
      myFile = openFile();
      token = 1;
    break;
    case 1:
      readFile(myFile);
      Serial.println(ligne);
      if(ligne[0] == '#')
      {
        getCommands(ligne);
        
        Serial.println(ligne[0]);
        State = parseLigne[0];
        readFile(myFile);
      }
      token =2;
    break;
    case 2:
      Serial.println("Changement d'état ( goto"+State+")");
      if (State == "#END") token = 6;
      if (State == "#INIT") token = 3;
      if (State == "#CONDITION") token = 4;
      if (State == "#PLAY") token = 5;
      if(State ==  "#EOF") token = 0;
      
    break;
    case 3:
      getCommands(ligne);
      Serial.println("bool avant conv   "+parseLigne[2]);
//      if(parseLigne[0] == "VIBRE") InitVibre(atoi(parseLigne[1].c_str()),( atoi(parseLigne[2].c_str())));
      Serial.println("slave :     "+(String)atoi( parseLigne[1].c_str()));
      if(parseLigne[0] == "COLOR") InitColor(  atoi( parseLigne[1].c_str()  ) ,atoi(parseLigne[2].c_str()),atoi(parseLigne[3].c_str()),atoi(parseLigne[4].c_str()),atoi(parseLigne[5].c_str()));
      if(parseLigne[0] == "VIBROTATE") initVibreRotation(atoi( parseLigne[1].c_str()) ,atoi( parseLigne[2].c_str())); 
      token = 1;
    break;
    case 4:
      getCommands(ligne);
      Serial.println("Chargement des info du scénario : "+parseLigne[0]);
      if(parseLigne[0] == "NBCOND")
      {
        token = 41;
      }
      else
      {
        token = 42;
      }
    break;
    case 41:
      Serial.println("lecture du nombre de conditions    "+parseLigne[1] );
      nbConditions = 8;
      CursorSols = 0;
      Serial.println("nombre de conditions    "+ nbConditions );
      token = 1;
    break;
    case 42:
      Serial.println(" ajout d'une condition");
      token = 43;
      Solutions[0][CursorSols] = atoi( parseLigne[0].c_str() );
      Solutions[1][CursorSols] = atoi( parseLigne[1].c_str() );
      Solutions[2][CursorSols] = atoi( parseLigne[2].c_str() );  
      Serial.println(Solutions[0][CursorSols]);
      Serial.println(Solutions[1][CursorSols]);
      Serial.println(Solutions[2][CursorSols]);
      CursorSols = CursorSols +1;
      token = 1;
    break;
    case 5:
       Tot[0].ReadState();
       Tot[2].ReadState();
       if((Tot[0].Face[Tot[0].FaceTop].Touch) && (Tot[2].Face[Tot[2].FaceTop].Touch) )
       {
         colorFace(Tot[0].FaceTop, {253,106,2},'A');
         colorFace(Tot[2].FaceTop,{253,106,2} ,'C');
         token = 51;
       }
    break;
    case 51://  vérif si c'est bon ou pas
       Tot[1].ReadState();
       token = 53;
       for( int i=0; i < nbConditions; i++)
       {
         if( (Tot[0].FaceTop == Solutions[0][i]) && (Tot[1].FaceTop == Solutions[1][i]) && (Tot[2].FaceTop == Solutions[2][i]) )
         {
           token = 52;
         }
       }
    break;
    case 52://  win
       Serial.print("WIIIINERRRR");
       win = 1;
       token = 1;
      State = "#END";
    break;
    
    case 53://  loose
      Serial.print("LOOOOOOOOOSSSSEEEERRRR");
      token = 1;
      win = 0;
      State = "#END";
    break;
    case 6:
      getCommands(ligne);
      Serial.println("PREPARATION FIN");
      Serial.println(ligne);
//      Serial.println("COMMANDE parseLigne[1]
      if ( (win == 1) &&  ( parseLigne[0] == "WIN"))
      {
        if ( parseLigne[1] == "COLOR" ) InitColor(atoi( parseLigne[2].c_str()  ) ,atoi(parseLigne[3].c_str()),atoi(parseLigne[4].c_str()),atoi(parseLigne[5].c_str()),atoi(parseLigne[6].c_str()));
        token = 1;
      }
      else
      {
//         InitColor(int Slave, int Face, int R, int G, int B)
        if( (win == 0) && ( parseLigne[0] == "LOOSE"))
        {
          if ( parseLigne[1] == "COLOR" ) InitColor(atoi( parseLigne[2].c_str()  ) ,atoi(parseLigne[3].c_str()),atoi(parseLigne[4].c_str()),atoi(parseLigne[5].c_str()),atoi(parseLigne[6].c_str()));
          token = 1;
        }
        else
        {
          token = 1;
        }
      }
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


