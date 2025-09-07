/* Author : XentiQ
Date created - 2022.12.14 - XentiQ version
*/

#ifndef _SAVEPROFILE_H_
#define _SAVEPROFILE_H_

#include <Arduino.h>
#include "XY_Table.h"
#include "Platform.h"

#define EEPROM_SIZE 2048 
#define MAX_PROFILES 10//(EEPROM_SIZE / sizeof(Profile))
#define PROFILE_START_ADDR 0

#define PROFILE_NAME_MAX_LEN 30
#define KEYPAD_MAX_LEN 6

//Change MAX_BUTTONS_X and MAX_BUTTONS_Y depending on the use;
//for (L) 300x300 - MAX_BUTTONS_X 33; MAX_BUTTONS_Y 33
//for (S) 200x300 - MAX_BUTTONS_X 20; MAX_BUTTONS_Y 27
#define MAX_BUTTONS_X 42//try054 33// Columns - small = 20
#define MAX_BUTTONS_Y 33 // Rows   - small = 27
#define MAXORGX 999//v204 99.9
#define MAXORGY 999//v204 99.9

#define MINZDIP 1
#define MAXZDIP 999

#define MINNUMX 1
#define MINNUMY 1
//Change MAXNUMX and MAXNUMY depending on the use;
//for (L) 300x300 - MAXNUMX 33; MAXNUMY 33
//for (S) 200x300 - MAXNUMX 20; MAXNUMY 27
#define MAXNUMX 42//try054 33
#define MAXNUMY 33


#define MINPITCHX 9//v205
#define MINPITCHY 9//v205
#define MAXPITCHX 999//v204 99.9
#define MAXPITCHY 999//v204 99.9
#define MAXCYCLE 99
#define MINCYCLE 1


extern char Password[4][PROFILE_NAME_MAX_LEN];


typedef struct
{
    char profileName[PROFILE_NAME_MAX_LEN];
    int8_t profileId = 0;
    int8_t Tube_No_x = 0; // represent the number of columns
    int8_t Tube_No_y = 0; // represent the number of rows
    float pitch_x = 0.0;
    float pitch_y = 0.0;
    float trayOriginX = 0.0;
    float trayOriginY = 0.0;
    int16_t Cycles = 1;
    int16_t CurrentCycle = 0; // New field for storing the current cycle
    int16_t vibrationEnabled = 0;
    bool dispenseEnabled = false;
    int16_t passwordEnabled = 0;
    int16_t vibrationDuration = 2;
    int16_t sizeFlag = 1; //small = 0; large = 1;
    float ZDip = 0.0;
    // bool **buttonStates; // Declare as a double pointer
    bool buttonStates[MAX_BUTTONS_X][MAX_BUTTONS_Y]; // Declare as a static 2D array
} Profile;


typedef struct
{
    char profileName[PROFILE_NAME_MAX_LEN];
    int8_t profileId = 0;
    int8_t Tube_No_x = 0; // represent the number of columns
    int8_t Tube_No_y = 0; // represent the number of rows
    float pitch_x = 0.0;
    float pitch_y = 0.0;
    float trayOriginX = 0.0;
    float trayOriginY = 0.0;
    int16_t Cycles = 1;
    int16_t CurrentCycle = 0; // New field for storing the current cycle
    float ZDip = 0.0;
    int16_t vibrationEnabled = 0;
    bool dispenseEnabled = false;
    int16_t passwordEnabled = 0;
    int16_t vibrationDuration = 2;
    int16_t sizeFlag = 1; //small = 0; large = 1;
} ProfileMin;


bool checkPasscode(Gpu_Hal_Context_t *phost, const char *enteredPasscode, const char *correctPasscode);

uint16_t calculateChecksum(const Profile &profile);
void loadProfile(void);
uint8_t LoadProfile(void);

void PreLoadEEPROM(void);
void BlankEEPROM(void);


void WriteCurIDEEPROM(uint8_t curprofid);
uint8_t ReadCurIDEEPROM(void);
void WritePassEEPROM(char *pass);

void ReadPassEEPROM(char *pass);



void WriteProfileEEPROM(int address);//try054 , Profile &profile);
void ReadProfileEEPROM(int address);
void ReadProfileMinEEPROM(int address);

#endif /*_SAVEPROFILE_H_*/
extern Profile CurProf;
extern ProfileMin SelectProf;//try055


extern uint8_t CurProfNum;
