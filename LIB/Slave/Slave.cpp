#include "Slave.h"
#include "stdlib.h"

#include <Adafruit_MMA8451.h>


int ScanTouch( int Channel)
{
  int Value = 0;
  CD4051addr(Channel);
  delay(1);
  Value = analogRead(A0);
  return (Value);
}


//**************************************************************
void CD4051addr(int Channel)
{
  switch (Channel)
  {
    case 0:
      digitalWrite(CD4051_A, LOW);
      digitalWrite(CD4051_B, LOW);
      digitalWrite(CD4051_C, LOW);
      break;
    case 1:
      digitalWrite(CD4051_A, HIGH);
      digitalWrite(CD4051_B, LOW);
      digitalWrite(CD4051_C, LOW);
      break;
    case 2:
      digitalWrite(CD4051_A, LOW);
      digitalWrite(CD4051_B, HIGH);
      digitalWrite(CD4051_C, LOW);
      break;
    case 3:
      digitalWrite(CD4051_A, HIGH);
      digitalWrite(CD4051_B, HIGH);
      digitalWrite(CD4051_C, LOW);
      break;
    case 4:
      digitalWrite(CD4051_A, LOW);
      digitalWrite(CD4051_B, LOW);
      digitalWrite(CD4051_C, HIGH);
      break;
    case 5:
      digitalWrite(CD4051_A, HIGH);
      digitalWrite(CD4051_B, LOW);
      digitalWrite(CD4051_C, HIGH);
      break;
	case 6:
      digitalWrite(CD4051_A, LOW);
      digitalWrite(CD4051_B, HIGH);
      digitalWrite(CD4051_C, HIGH);
      break; 
	case 7:
      digitalWrite(CD4051_A, HIGH);
      digitalWrite(CD4051_B, HIGH);
      digitalWrite(CD4051_C, HIGH);
      break; 
  }
}


//**********************************************************************************
extern Adafruit_MMA8451 mma8451;
extern int xVal, yVal, zVal;
extern struct Inclinaison_ Inclinaison;

void Inclinometre()
{
  int Quadrant, Rotation;
  struct CoordTypeAbs CurseurMm;
  int temp;
  float angleX, angleY, angleZ, total;
  
 
  mma8451.read();
  
  
  temp = (int) mma8451.x / 64;
  xVal = temp;
  temp = (int) mma8451.y / 64;
  yVal = temp;
  temp = (int) mma8451.z / 64;
  zVal = temp;
  xVal = Filter_compute(xVal, 0);
  yVal = Filter_compute(yVal, 1);
  zVal = Filter_compute(zVal, 2);
  // valeur totale de l'accélération, peu importe l'orientation
  total = sqrt(xVal * xVal + yVal * yVal + zVal * zVal);
  // calcul des angles et conversion en degrés
  angleX = round(asin(xVal / total ) * 180.0 / 3.1416);
  angleY = round(asin(yVal / total ) * 180.0 / 3.1416);
  angleZ = round(acos(zVal / total ) * 180.0 / 3.1416);
  //modifier attribution XYZ selon orientation du chip
  CurseurMm.Z = (int)angleX;
  CurseurMm.X = (int)angleY;
  CurseurMm.Y = (int)angleZ - 90;
  MagnetGrid(&CurseurMm);
  if (CurseurMm.Z < 0)
  {
    if (CurseurMm.Y < 0) Quadrant = 4;
    else Quadrant = 1;
  }
  else
  {
    if (CurseurMm.Y < 0) Quadrant = 3;
    else Quadrant = 2;
  }
  switch (Quadrant)
  {
    case 1: Rotation = CurseurMm.Y;
      break;
    case 2: Rotation = 90 + CurseurMm.Z;
      break;
    case 3: Rotation = 180 + (90 - CurseurMm.Z);
      break;
    case 4: Rotation = 270 - CurseurMm.Z;
      break;
  }
  
  if ((Rotation >= 0) && (Rotation < 60)) Inclinaison.Secteur = 2;
  if ((Rotation >= 60) && (Rotation < 120)) Inclinaison.Secteur = 1;
  if ((Rotation >= 120) && (Rotation < 180)) Inclinaison.Secteur = 0;
  if ((Rotation >= 180) && (Rotation < 240)) Inclinaison.Secteur = 5;
  if ((Rotation >= 240) && (Rotation < 300)) Inclinaison.Secteur = 4;
  if ((Rotation >= 300) && (Rotation < 360)) Inclinaison.Secteur = 3;
  
  Inclinaison.Angle=Rotation;
  Inclinaison.Tangage=CurseurMm.X;
}

//*************** Filtrage et grille magnetique inclinometre
FilterStruct Filter[3];
	
void Filter_init(int Value,int Lenght, int Num)
{
    int i;
    Filter[Num].Index=0;    
    Filter[Num].Lenght=Lenght;
    
	for(i=0;i<Lenght;i++)
    {
        Filter[Num].Buffer[i]=Value;
    } 
}

int Filter_compute(unsigned int Value,int Num)
{
    

        long Accu=0;
        int Index;
        int i;
		
        Index = Filter[Num].Index;  

        Filter[Num].Buffer[Index] = Value;
		
        Index ++; 
        if(Index >= Filter[Num].Lenght)
            Index = 0;
        Filter[Num].Index = Index;


        for(i=0;i<Filter[Num].Lenght;i++)
        {
            Accu += Filter[Num].Buffer[i];
        }
        Accu = (Accu / Filter[Num].Lenght); 
        return((int)Accu);
}

void MagnetGrid(  struct CoordTypeAbs *Point)
{
static int LastX =0;
static int LastY =0;
static int lastZ =0;
int InputX; 
int InputY; 
int InputZ; 
int Ecart; 

InputX = Point->X;
InputY = Point->Y;
InputZ = Point->Z;

Ecart = abs(InputX - LastX);
if (Ecart > (GRIDSTEP * GRIDCOEF)) Point->X = FindNearGridPoint(InputX);
else    Point->X = LastX;

Ecart = abs(InputY - LastY);
if (Ecart > (GRIDSTEP * GRIDCOEF)) Point->Y = FindNearGridPoint(InputY);
else    Point->Y = LastY;


Ecart = abs(InputZ - lastZ);
if (Ecart > (GRIDSTEP * GRIDCOEF)) Point->Z = FindNearGridPoint(InputZ);
else    Point->Z = lastZ;


LastX = Point->X;
LastY = Point->Y;
lastZ = Point->Z;

}

int FindNearGridPoint(int Coord)
{

int Temp;
int Inf;
int Sup;
int Result; 

Temp = Coord / GRIDSTEP;
Inf = Temp * GRIDSTEP; 
Sup = Inf + GRIDSTEP;  
if ( abs(Coord - Inf) < abs(Coord - Sup) )  Result = Inf;
else Result = Sup;

return(Result);

}
