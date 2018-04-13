#ifndef _SLAVE
#define _SLAVE

#include "ifDef.h"

extern String Answer[SPLITTERS];
extern int Param[SPLITTERS];


struct TotFace
{
  int Couleur;
  bool Touch;
};


class Slave
{
  public:
    char Addr;
    char Neighbour;
    bool Inscope;
    struct TotFace Face[6];
    int FaceTop;
    int FaceBot;
    int Tangage;
    String RFID;
    String ID;
    int State;
    int ErrorFace;
    int ErrorSector;

    Slave(void);
    //Slave(char addr, String ID);
    Vibre();
    Colorie(int F0, int F1, int F2, int F3, int F4, int F5, bool Apply);
    LightFace(int Face, int palette);
    ReadState();
    Sprint(void);
    ReadRFID(void);
    BehaviorW(int State, int Fmatch, int Fcolor, int Fcligno, String RFID, int Last);
    BehaviorR(void);
    FaceBotUpdate(void);
};

#endif
