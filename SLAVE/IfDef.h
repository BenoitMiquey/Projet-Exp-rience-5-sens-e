#ifndef BITS_H
#define BITS_H

//#define COMUART // choix entrée commandes par USB-UART ou Par SMARTALPHA 433Mhz
//#define DEBUG

#define TOTEMIGO_ADDR 'A'

struct color
{
  int R;
  int G;
  int B;
};

struct Behavior_
{
  int State;
  bool Last;
  int FaceMatch;  //la face a valider dans le Gameplay (si pas mon tour = -1)
  int FaceColor;  //Couleur de la face a valider, choix dans la palette
  int Cligno;     //base de temps du clignotement (si fixe =1)
  String RFID;    //Le tag RFID du voisin a gauche (si premier de la ligne => pas de voisin => "noRFID"
  int ErrorFace;  //La face appuyée erronee
  int ErrorSector;//l'inclinaison lors de l'erreur
};
enum BehaviorStates {INIT, WAIT_FACEMATCH, WAIT_NOFACE, CHECK_RFID, MATCHED, MATCH_ERROR, STANDBY};
enum Couleurs {BLEU, VERT, JAUNE, ORANGE, ROUGE, VIOLET, ROSE, MARRON, BLANC, NOIR,CYAN, BG};
//enum Couleurs {BLEU, VERT, JAUNE, ORANGE, ROUGE, VIOLET, BLANC, NOIR, BG};

#endif // not defined BITS_H
