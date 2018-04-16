#ifndef SLAVE_H
#define SLAVE_H

#define T_BUFFER_LENGHT 8
#define GRIDSTEP 12 		
#define GRIDCOEF 0.9	

#define CD4051_A 8
#define CD4051_B 9
#define CD4051_C 10
#define CD4051_I 11
#define RFID_RX_PIN 12
#define RFID_POWER 13
#define SMARTALPHA_CTS 4
#define SMARTALPHA_RTS 5
#define NEOPIXEL_PIN 6
#define MOTOR 7

int ScanTouch( int Channel);
long ScanRFID( int Channel);
void Inclinometre(void);
void CD4051addr(int Channel);

struct BGcolor_
{
	int R;
	int G;
	int B;
};

struct Faces
{
	long Rfid;
	int TouchLevel;
	int TouchState;
	int LedIndex[3];
	int colorIndex; // Petit ajout qui permet d'obtenir l'index de la couleur ( palette )
	bool WaitRelease;
	int Red;
	int Green;
	int Blue;
	int ClignoTime;
	int ClignoCount;
	bool ClignoState;
}; 

struct Inclinaison_
{
	int Angle;
	int Secteur;
	int Tangage;
};

struct Animation_
{
  int Mode=0;
  int Speed=0;
  int BdT=0;
  int Face=0;
  int Red=0;
  int Green=0;
  int Blue=0;
};
enum modes { STOP, ROTATION, TOPLIGHT , ANIMFLASH};

//*************** Filtrage et grille magnetique inclinometre
struct FilterStruct
{
        int Index;       
        int Buffer[T_BUFFER_LENGHT]; 
        int Lenght;      
};
void Filter_init(int ,int , int );
int Filter_compute(unsigned int ,int );

void MagnetGrid(  struct CoordTypeAbs *Point);
int FindNearGridPoint(int Coord);

struct CoordTypeAbs
	{
        int X;
        int Y;
        int Z;
    };
struct CoordTypeDyn
	{
        float X;
        float Y;
        float Z;
    };




#endif