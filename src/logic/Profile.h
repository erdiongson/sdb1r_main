/* Author : XentiQ
Date created - 2022.12.14 - XentiQ version
*/

#pragma once

#ifndef _PROFILE_H_
#define _PROFILE_H_

#include <Arduino.h>
#include "../../XY_Table.h"
#include "../gpu/Platform.h"

#define EEPROM_SIZE 2048 
#define MAX_PROFILES 10 //(EEPROM_SIZE / sizeof(Profile))
#define PROFILE_START_ADDR 0

#define PROFILE_NAME_MAX_LEN 30
#define PASSWORD_MAX_LEN 30
#define KEYPAD_MAX_LEN 6
#define ROW_COL_MAX_LEN 45

//Change MAX_BUTTONS_X and MAX_BUTTONS_Y depending on the use;
//for (L) 300x300 - MAX_BUTTONS_X 33; MAX_BUTTONS_Y 33
//for (S) 200x300 - MAX_BUTTONS_X 20; MAX_BUTTONS_Y 27
#define MAX_BUTTONS_X 42//try054 33// Columns - small = 20
#define MAX_BUTTONS_Y 33 // Rows   - small = 27
#define MAXORGX 999//v204 99.9
#define MAXORGY 999//v204 99.9

#define MINZDIP 0
#define MAXZDIP 999

#define MINNUMX 1
#define MINNUMY 1
//Change MAXNUMX and MAXNUMY depending on the use;
//for (L) 300x300 - MAXNUMX 33; MAXNUMY 33
//for (S) 200x300 - MAXNUMX 20; MAXNUMY 27
#define MAXNUMX 42 //try054 33
#define MAXNUMY 33


#define MINPITCHX 9 //v205
#define MINPITCHY 9 //v205
#define MAXPITCHX 999 //v204 99.9
#define MAXPITCHY 999 //v204 99.9
#define MAXCYCLE 99
#define MINCYCLE 1

typedef struct
{
    char profileName[PROFILE_NAME_MAX_LEN];
    int8_t profileId = 0;
    int8_t Tube_No_x = 0; //represent the number of columns
    int8_t Tube_No_y = 0; //represent the number of rows
    float pitch_x = 0.0;
    float pitch_y = 0.0;
    float trayOriginX = 0.0;
    float trayOriginY = 0.0;
    int16_t Cycles = 1;
    int16_t CurrentCycle = 0; //New field for storing the current cycle
    int16_t vibrationEnabled = 0;
    bool dispenseEnabled = false;
    int16_t passwordEnabled = 0;
    int16_t vibrationDuration = 2;
    int16_t sizeFlag = 1; //small = 0; large = 1;
    float ZDip = 0.0;
    char skipCol[ROW_COL_MAX_LEN];
    char skipRow[ROW_COL_MAX_LEN];
    char skipSinglePos[ROW_COL_MAX_LEN];
    bool staggered = false;
} Profile;

uint8_t loadProfile(void);

void preLoadEEPROM(void);
void blankEEPROM(void);

void writeCurIDEEPROM(uint8_t curprofid);
uint8_t readCurIDEEPROM(void);
void writePassEEPROM(char *pass);

void readPassEEPROM(char *pass);

void writeProfileEEPROM(int address); //try054 , Profile &profile);
void readProfileEEPROM(int address);

// Password verification result enum.
enum PasswordVerificationResult {
  PASSWORD_SUCCESS,
  PASSWORD_INCORRECT,
  PASSWORD_CANCELLED
};

// Verifies the password by prompting the user for input.
// @param phost GPU context for displaying the keyboard.
// @return PasswordVerificationResult indicating success, incorrect, or cancelled.
PasswordVerificationResult verifyPassword(Gpu_Hal_Context_t *phost);

#endif /*_PROFILE_H_*/

extern Profile CurProf;
extern uint8_t CurProfNum;
