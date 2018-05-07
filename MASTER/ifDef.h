#ifndef BITS_H
#define BITS_H

//#define DEBUG 

#include <SlaveSeeedRFID.h>
#include <Wire.h>
#include "Slave.h"
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include "RTClib.h"
#include <SD.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <Wire.h>
#include <avr/wdt.h>

// These are the pins used for the breakout example ADAPTE POUR CARTE SEEED
#define BREAKOUT_RESET  A0      // VS1053 reset pin (output)
#define BREAKOUT_CS     A3     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    A2      // VS1053 Data/command select pin (output)
#define CARDCS 10     // Card chip select pin
#define DREQ A1       // VS1053 Data request, ideally an Interrupt pin

#define SMARTALPHA_CTS 4
#define SMARTALPHA_RTS 5
#define BUTTON 24
#define LED 22

#define CR 0x0D
#define LF 0x0A
#define SPLITTERS 16 // nombre de délimiteurs MAX pris en compte
#define SPLIT ','
#define TRAME_START '<'
#define TRAME_END '>'
#define TO_SMARTALPHA 40000
#define ANSWER_DELAY 60000 //attention: la lecture RFID prend du temps...
#define MAXRETRY 10000

void SmartAlphaAddr(char Slave);
int calcrc(char *ptr, int count);
int SmartAlphaRxProcess(void);
void String_Split(char Splitter);
void SmartAlphaReinit();
int TalkToSlave(char Slave, String Cmd, int MaxRetry);

void SDconfigRead(void);
void ListSdFiles(void);
void SDlogWrite(int Seek, String Event);
void SDlogRead(void);
void RTCread(void);
void RTCinit(void);
void PaletteInit(void);
void Demo1(void);
void Demo2(void);
void Demo3(void);
void Demo4(void);
void Demo5(void);
void SimonPlay(void);
void NewSimonPlay(void);
void CalculPlay(void);
void ColorieFace(char Tot, int Face, int Couleur, int Mode);
void DeckInit(void);
enum BehaviorStates {INIT, WAIT_FACEMATCH, WAIT_NOFACE, CHECK_RFID, MATCHED, MATCH_ERROR, STANDBY};
//enum Couleurs {BLEU, VERT, JAUNE, ORANGE, ROUGE, VIOLET, BLANC, NOIR, BG};
enum Couleurs {BLEU, VERT, JAUNE, ORANGE, ROUGE, VIOLET, ROSE, MARRON, BLANC, NOIR, CYAN, BG};

#endif // not defined BITS_H
// contenu de UnProjet.cpp
