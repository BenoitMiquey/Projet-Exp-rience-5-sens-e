//#include "ifDef.h"
//#include "CSlave.h"
//#include "stdlib.h"
//#include "LinkedList.h"
//int FocusSlave ;
//String music;
//int sum;
//int num;
//int sol;
//char op;
//int k;
//int i,levelVictoire;
//int NeightbourgFace;
//#define NBSLAVE 3 // nombre de slaves participant, au moins un chiffré et un avec des signes 
//extern Adafruit_VS1053_FilePlayer mp3;
//char SlaveToChar[] = {'A','B','C'};
//String message = "";
//String CodeRFID[] = {"", "", "", ""};
//extern Slave TotA, TotB, TotC, TotD;
//extern Slave Tot[4];
//int sablierMelange, neightbourgTrade;
//int faceClicked; // contient le numéro de la face cliquée
//int slaveClicked; // contient le numéro du slave cliqué
//String msgLog; // contient le message a envoyer dans le fichier de log
//// la liste chainée qui contient des elements de type SimonStep
//int indexNeightBourg;
//// fonction pour enregistrer dans le fichier log
//void LogPlay(String Message)
//{
//  Serial.println(Message);
//  SDlogWrite(-1, Message);
//}
//int sablierRotation;
//struct color
//{
//  int R;
//  int G;
//  int B;
//};
//int nextToken;
//int blackSlave;
//color colortmp;
//color ColorSlavesPatern[3][6];
//color colorFaces[6];
//struct color Palette[16];
//enum FacesList {FACE_0, FACE_1, FACE_2, FACE_3, FACE_4, FACE_5, FACE_ALL};
//extern Adafruit_VS1053_FilePlayer mp3;
//extern String Answer[SPLITTERS];
//extern int Param[SPLITTERS];
//extern RTC_DS1307 rtc;
//// le token va servir de jeton de position, pour savoir quelle action est a effectuer
//// chaques méthode le changera de manière a donner la main a une autre methode
//int token = 0;
//int faceTouched,slaveTouched;
//int randomFace, randomNeightbourg;
//int compteurVerif;
//int faceTmpA, faceTmpB;
//int slaveTmpA, slaveTmpB;
//int chooseANumber(int a,int b) // retourne un nombre aléatoire entre 2 et 12, qui devra etre calculé par la personne
//{
//  DateTime now = rtc.now();
//  randomSeed(now.second());
//  return random(a,b);
//}
//void colorFace(int face, color couleur, char Slave)
//{
//  
//  message = "FaceLed,";
//  message += face;
//  message +=",1,";
//  message += couleur.R;
//  message +=",";
//  message +=couleur.G;
//  message +=",";
//  message +=couleur.B;
//  TalkToSlave(Slave,message,MAXRETRY); 
//}
//int findNeightbourgFace(int Slave, int NeightBourg,int Face)
//{
//  int relative = 0;
//  Tot[Slave].ReadState();
//  Tot[NeightBourg].ReadState();
//  while( ((Tot[Slave].FaceTop + relative)%6) != Face)
//  {
//    relative++;
//  }
//  //colorFace((Tot[NeightBourg].FaceTop + relative)%6, Palette[BLANC],intSlaveToChar(NeightBourg));
//  return((Tot[NeightBourg].FaceTop + relative)%6);
//}
//bool isBlackFace(int Slave, int Face)
//{
//  return ((ColorSlavesPatern[Slave][Face].R == 0) && (ColorSlavesPatern[Slave][Face].G == 0) && (ColorSlavesPatern[Slave][Face].B == 0) );
//}
//int passiveNeightBourgFace(int Slave, int NeightBourg, int Face)
//{
//  int relative = 0;
//  Tot[NeightBourg].ReadState();
//  while( ((Tot[Slave].FaceTop + relative)%6) != Face)
//  {
//    relative++;
//  }
//  return((Tot[NeightBourg].FaceTop + relative)%6);
//}
//bool equalColor(color a, color b)
//{
//  return ((a.R == b.R) && (a.G == b.G) && (a.B ==b.B));
//}
//void rubikPlay()
//{
//  
//  switch(token)
//  {
//    case 0: // initialisation du GP
//      sablierMelange = 0;
//      blackSlave = 1;
//      PaletteInit();
//
//      for( int i=0; i < 3; i++)
//      {
//       for( int j= 0; j <6; j++)
//        {
//          ColorSlavesPatern[i][j].R = Palette[j].R;
//          ColorSlavesPatern[i][j].G = Palette[j].G;
//          ColorSlavesPatern[i][j].B = Palette[j].B;
//
//        }
//        
//      }
//      for( int i=0; i < 3; i++)
//      {
//        message = "Animation,0";
//        TalkToSlave(SlaveToChar[i], "BGcolor,0,0,0", MAXRETRY);
//        TalkToSlave(SlaveToChar[i],message,MAXRETRY);
//        for( int j= 0; j <6; j++)
//        {
//          colorFace(j,ColorSlavesPatern[i][j],SlaveToChar[i]);
//        }
//        
//      }
//      token =1;
//    break;
//
//    case 1:
//    Serial.print("choix random du slave");
//     slaveTmpA = chooseANumber(0,3);
//     Serial.print(slaveTmpA);
//     token = 2;
//     sablierRotation = chooseANumber(1,4);
//    break;
//      
//    case 2:
//       Serial.print("rotation dans la matrice");
//       sablierMelange--;
//      colortmp = ColorSlavesPatern[slaveTmpA][0] ;
//      for(int j=0; j < 5; j++)
//      {
//        ColorSlavesPatern[slaveTmpA][j] = ColorSlavesPatern[slaveTmpA][j+1];
//      }
//      
//      ColorSlavesPatern[slaveTmpA][5] = colortmp;
//      token = 3;
//     break;
//
//    case 3:
//      Serial.print("rotation visuelle");
//      if ( sablierMelange > 0)
//      {
//        for( int j=0; j<6; j++)
//          {
//            colorFace(j,ColorSlavesPatern[slaveTmpA][j],SlaveToChar[slaveTmpA]);
//          }
//      if(sablierRotation > 0 )
//      {
//        if(chooseANumber(0,3) == 2)
//        {
//          Serial.print("Changement !!!!!!!!!!!!!");
//          token = 4;
//        }
//        else
//        {
//          token = 2;
//          sablierRotation = sablierRotation -1;
//        }
//      }
//      else
//      {
//        token = 1;
//      }
//      }
//      else
//      {
//        token = 5;
//      }
//
//    break;
//    case 4:
//       randomFace = chooseANumber(0,6);
//        sablierMelange--;
//       do
//       {
//        randomNeightbourg = chooseANumber(-1,1);
//       }while( randomNeightbourg == 0);
//       colortmp = ColorSlavesPatern[(slaveTmpA + randomNeightbourg)%3][randomFace];
//       ColorSlavesPatern[(slaveTmpA + randomNeightbourg)%3][randomFace] = ColorSlavesPatern[slaveTmpA][randomFace];
//       ColorSlavesPatern[slaveTmpA][randomFace] = colortmp;
//       colorFace(randomFace, ColorSlavesPatern[(slaveTmpA + randomNeightbourg)%3][randomFace],SlaveToChar[(slaveTmpA + randomNeightbourg)%3]);
//       colorFace(randomFace, ColorSlavesPatern[slaveTmpA][randomFace],SlaveToChar[slaveTmpA]);
//       
//    token = 3;
//
//    break;
//    case 5:
//      for( int i=0; i < 3; i++)
//      {
//        for( int j= 0; j <6; j++)
//        {
//          colorFace(j,ColorSlavesPatern[i][j],SlaveToChar[i]);
//        }
//        
//      }
//      token = 6;
//    break;
//    case 6:
//    Serial.print("mise en noir de la face");
//      Tot[1].ReadState();
//      
//     ColorSlavesPatern[1][Tot[1].FaceTop].R = 0;
//      ColorSlavesPatern[1][Tot[1].FaceTop].G = 0;
//      ColorSlavesPatern[1][Tot[1].FaceTop].B = 0;
//      colorFace(Tot[1].FaceTop,ColorSlavesPatern[1][Tot[1].FaceTop],SlaveToChar[1]);
//      token = 7;
//    break;
//    case 7:
//
//      switch(blackSlave)
//      {
//        case 0:
//          Tot[1].ReadState();
//          for( int j=0; j<6; j++)
//          {
//            if(Tot[1].Face[j].Touch)
//            {
//              Serial.print("\n Touch!!!!!");
//              faceTouched = j;
//              slaveTouched = 1;
//              token = 8;
//              goto endA;
//            }
//          }
//        break;
//
//        case 2:
//          Tot[1].ReadState();
//          for( int j=0; j<6; j++)
//          {
//            if(Tot[1].Face[j].Touch)
//            {
//              Serial.print("\n Touch!!!!!");
//              faceTouched = j;
//              slaveTouched = 1;
//              token = 8;
//              goto endA;
//            }
//          }
//        break;
//        case 1:
//          for( int i = 0; i <3; i= i+2)
//          {
//            Tot[i].ReadState();
//            for( int j=0; j<6; j++)
//            {
//              if(Tot[i].Face[j].Touch)
//              {
//                Serial.print("\n Touch!!!!!");
//                faceTouched = j;
//                slaveTouched = i;
//                token = 8;
//                goto endA;
//              }
//            }
//          }
//
//        break;
//      }
//      nextToken = 7;
//      token =9;
//        
//      endA:
//    break;
//    case 8:
//    indexNeightBourg = -1;
//      if( (slaveTouched == 0) || (slaveTouched == 2))
//      {
//        NeightbourgFace = findNeightbourgFace( slaveTouched, 1,faceTouched);
//        if(isBlackFace(1,NeightbourgFace))
//        {
//          indexNeightBourg = 1;
//        }
//      }
//      if (slaveTouched == 1) 
//      {
//        NeightbourgFace = findNeightbourgFace( slaveTouched, 0,faceTouched);
//        if (isBlackFace(0,NeightbourgFace))
//        {
//          indexNeightBourg = 0;
//        }
//        else
//        {
//          NeightbourgFace = findNeightbourgFace( slaveTouched, 2,faceTouched);
//        }
//        
//        if ( isBlackFace(2,NeightbourgFace))
//          {
//            indexNeightBourg = 2;
//          }
//        }
//        if( indexNeightBourg != -1)
//        {
//           //Serial.print("yeaaaah");
//            ColorSlavesPatern[indexNeightBourg][NeightbourgFace] = ColorSlavesPatern[slaveTouched][faceTouched];
//            ColorSlavesPatern[slaveTouched][faceTouched] =Palette[NOIR];
//            colorFace(NeightbourgFace,ColorSlavesPatern[indexNeightBourg][NeightbourgFace],SlaveToChar[indexNeightBourg]);
//            colorFace(faceTouched,ColorSlavesPatern[slaveTouched][faceTouched],SlaveToChar[slaveTouched]);
//            blackSlave = slaveTouched;
//        }
// 
//      token =9;
//    
//    break;
//
//    case 9: // check si la personne a gagné
//      Tot[0].ReadState();
//      Tot[1].ReadState();
//      Tot[2].ReadState();
//      if ( compteurVerif == 6)
//      {
//        i = Tot[0].FaceTop;
//        compteurVerif = 0;
//      }
//      while((compteurVerif) < 6 && (token == 9)) 
//      {
//        if ( (equalColor(ColorSlavesPatern[0][(Tot[0].FaceTop+i)%6] , ColorSlavesPatern[1][(Tot[1].FaceTop+i)%6]) || ( isBlackFace(1,(Tot[1].FaceTop+i)%6) || isBlackFace(0,(Tot[0].FaceTop+i)%6)))   &&   (equalColor(ColorSlavesPatern[0][(Tot[0].FaceTop+i)%6] , ColorSlavesPatern[2][(Tot[2].FaceTop+i)%6]) || ( isBlackFace(2,(Tot[2].FaceTop+i)%6) || isBlackFace(0,(Tot[0].FaceTop+i)%6))) && (equalColor(ColorSlavesPatern[1][(Tot[1].FaceTop+i)%6] , ColorSlavesPatern[2][(Tot[2].FaceTop+i)%6]) || ( isBlackFace(1,(Tot[1].FaceTop+i)%6) || isBlackFace(2,(Tot[2].FaceTop+i)%6)) ) )
//        {
//          Serial.print("c'est bon, bravo!");
//          Serial.print(compteurVerif %6);
//          levelVictoire++;
//        }
//        else
//        {
//          levelVictoire =0;
//          Serial.print("raté : ");
//          Serial.print( i);
//          token = nextToken;
//        }
//        if(levelVictoire < 9)
//        {
//          i++;
//          compteurVerif++;
//        }
//        else
//        {
//          levelVictoire= 0;
//          
//          
//          for( int n=0; n<3; n++)
//          {
//          Tot[n].Colorie(VERT,VERT,VERT,VERT,VERT,VERT,1);
//          TalkToSlave(SlaveToChar[n], "Vibre,200", MAXRETRY);
//          }
//          mp3.playFullFile("Winning.mp3");
//          delay(3000);
//          token = 0;
//        }
//      }
//    break;
//  }
//}
//
//
//
//
//
//// initialisation palette, on peut biensur ajouter des couleurs si besoin
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
//  Palette[CYAN].R = 66;
//  Palette[CYAN].G = 245;
//  Palette[CYAN].B = 245;
//  
//}

