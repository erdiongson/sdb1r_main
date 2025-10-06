/* Author : XentiQ
Date created - 2022.12.14 - XentiQ version

* Author: erdiongson
* Date Created: 2024.02.26
* Version 2.04: i. Changed the logic for the ReadPassEEPROM and WritePassEEPROM
*                  to read and write the characters of the password;
*/

#include "Profile.h"
#include <EEPROM.h>
#include "src/gpu/Platform.h"
#include "src/gpu/app_common.h"
#include "src/Utils.h"

#define SINGLE_PROFILE

const int PROFILE_SIZE = sizeof(Profile);
//const int NUM_PROFILES = EEPROM_SIZE / PROFILE_SIZE;
const int NUM_PROFILES = 1;
const int MAX_BUTTONS = 250;

int getProfileAddress(int profileId) {
    return PROFILE_START_ADDR + profileId * PROFILE_SIZE;
}

bool checkPasscode(Gpu_Hal_Context_t *phost, const char *enteredPasscode, const char *correctPasscode)
{
    if (strcmp(enteredPasscode, correctPasscode) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
void BlankEEPROM(void)
{
	int i;

	for(i=0;i<4096;i++) EEPROM.put(i,0);
}

void PreLoadEEPROM(void)
{
	uint8_t i,x,y;
	char buf[20];
	//try054 Profile CurProf;

	for(i=0;i<MAX_PROFILES;i++)
	{
		sprintf(buf,"profile %d",i+1);
		strcpy(CurProf.profileName,buf);//try054

#if 0//try054
    if(i==15)//v205
    {
      tempprof.Tube_No_x=2;
      tempprof.Tube_No_y=2;
      tempprof.pitch_x=9;//v205
      tempprof.pitch_y=9;//v205
      tempprof.trayOriginX=10;
      tempprof.trayOriginY=10;
      tempprof.Cycles=2;
      tempprof.vibrationEnabled=0;
      tempprof.passwordEnabled=TRUE;
      tempprof.vibrationDuration=2;
      tempprof.sizeFlag = 1;
    }
    else
    {
#endif//try054 B
      CurProf.Tube_No_x=MAX_BUTTONS_X;
      CurProf.Tube_No_y=MAX_BUTTONS_Y;
      CurProf.pitch_x=9;//v205
      CurProf.pitch_y=9;//v205
      CurProf.trayOriginX=10;
      CurProf.trayOriginY=10;
      CurProf.Cycles=2;
      CurProf.vibrationEnabled=0;
      CurProf.passwordEnabled=TRUE;
      CurProf.vibrationDuration=2;
      CurProf.sizeFlag = 1;
      CurProf.ZDip = 0.0;
      CurProf.skipCol[0] = '\0';
      CurProf.skipRow[0] = '\0';
      CurProf.skipSinglePos[0] = '\0';
      CurProf.staggered = false;

    //try054 E }
    WriteProfileEEPROM(i);//try054 ,CurProf);

  }
  WriteCurIDEEPROM(0);//reset current profile in eeprom to 0 
}

void CheckProfile(void)
{
	if(CurProf.Cycles>MAXCYCLE) CurProf.Cycles=MAXCYCLE;
	if(CurProf.Cycles<0) CurProf.Cycles=0;
	if(CurProf.pitch_x>MAXPITCHX) CurProf.pitch_x=MAXPITCHX;
	if(CurProf.pitch_x<0) CurProf.pitch_x=0;
	if(CurProf.pitch_y>MAXPITCHY) CurProf.pitch_y=MAXPITCHY;
	if(CurProf.pitch_y<0) CurProf.pitch_y=0;
	if(CurProf.trayOriginX>MAXORGX) CurProf.trayOriginX=MAXORGX;
	if(CurProf.trayOriginX<0) CurProf.trayOriginX=0;
	if(CurProf.trayOriginY>MAXORGY) CurProf.trayOriginY=MAXORGY;
	if(CurProf.trayOriginY<0) CurProf.trayOriginY=0;
	if(CurProf.Tube_No_x>MAX_TUBES_X) CurProf.Tube_No_x=MAX_TUBES_X;
	if(CurProf.Tube_No_x<0) CurProf.Tube_No_x=0;
	if(CurProf.Tube_No_y>MAX_TUBES_Y) CurProf.Tube_No_y=MAX_TUBES_Y;
	if(CurProf.Tube_No_y<0) CurProf.Tube_No_y=0;
	if(CurProf.ZDip<0) CurProf.ZDip=0;
	if(CurProf.ZDip>MAX_ZDIP) CurProf.ZDip=MAX_ZDIP;
}
uint8_t LoadProfile(void)
{
	uint8_t ret=0;

	ret=ReadCurIDEEPROM();
	if(ret>=MAX_PROFILES)  ret=0;// if corrcupt data from eeprom, set id=0
	
	ReadProfileEEPROM(ret);//reread as it is not profile id 0
	if(CurProf.Cycles>MAXCYCLE) CurProf.Cycles=MAXCYCLE;
	if(CurProf.pitch_x>MAXPITCHX) CurProf.pitch_x=MAXPITCHX;
	if(CurProf.pitch_y>MAXPITCHY) CurProf.pitch_y=MAXPITCHY;
	if(CurProf.trayOriginX>MAXORGX) CurProf.trayOriginX=MAXORGX;
	if(CurProf.trayOriginY>MAXORGY) CurProf.trayOriginY=MAXORGY;
	if(CurProf.Tube_No_x>MAX_TUBES_X) CurProf.Tube_No_x=MAX_TUBES_X;
	if(CurProf.Tube_No_y>MAX_TUBES_Y) CurProf.Tube_No_y=MAX_TUBES_Y;
	if(CurProf.ZDip<0) CurProf.ZDip=0;
	if(CurProf.ZDip>MAX_ZDIP) CurProf.ZDip=MAX_ZDIP;
	// if(CurProf.staggered) CurProf.staggered=TRUE;
    CurProf.staggered = TRUE;
	return ret;
}

/*
void WritePassEEPROM(char *pass)
{
	EEPROM.put(4000, *pass);
}

void ReadPassEEPROM(char *pass)
{
    EEPROM.get(4000, *pass);

}
*/
//20240226: erdiongson - updated the logic of ReadPassEEPROM to read the whole string
void ReadPassEEPROM(char *pass)
{
    int addr = 4000;
    char c;
    int i = 0;
    do {
        EEPROM.get(addr++, c); // Read one character from EEPROM
        pass[i++] = c; // Store the character in pass and increment the index
    } while (c != '\0'); // Repeat until null terminator is encountered
}

//20240226: erdiongson - updated the logic of WritePassEEPROM to write the whole string
void WritePassEEPROM(char *pass)
{
    int addr = 4000;
    int i = 0;
    char c;
    do {
        c = pass[i++]; // Get the next character from the pass array and increment the index
        EEPROM.put(addr++, c); // Write the character to EEPROM
    } while (c != '\0'); // Repeat until null terminator is encountered
}

void WriteCurIDEEPROM(uint8_t curprofid)
{
	EEPROM.put(0, curprofid);
}

uint8_t ReadCurIDEEPROM(void)
{
	uint8_t temp;

	EEPROM.get(0, temp);
	if(temp>MAX_PROFILES) temp=0;
	return temp;
}

void WriteProfileEEPROM(int address)//try054 , Profile &profile)
{
	int x,y;
	byte dat;
	int bit;
	
	address=(address*300)+sizeof(uint8_t);//v205
	
    //EEPROM.put(address, profile.profileId);
    //address += sizeof(profile.profileId);
    EEPROM.put(address, CurProf.profileName);
    address += sizeof(CurProf.profileName);
    EEPROM.put(address, CurProf.Tube_No_x);
    address += sizeof(CurProf.Tube_No_x);
    EEPROM.put(address, CurProf.Tube_No_y);
    address += sizeof(CurProf.Tube_No_y);
    EEPROM.put(address, CurProf.pitch_x);
    address += sizeof(CurProf.pitch_x);
    EEPROM.put(address, CurProf.pitch_y);
    address += sizeof(CurProf.pitch_y);
    EEPROM.put(address, CurProf.trayOriginX);
    address += sizeof(CurProf.trayOriginX);
    EEPROM.put(address, CurProf.trayOriginY);
    address += sizeof(CurProf.trayOriginY);
    EEPROM.put(address, CurProf.Cycles);
    address += sizeof(CurProf.Cycles);
    EEPROM.put(address, CurProf.vibrationEnabled);
    address += sizeof(CurProf.vibrationEnabled);
    EEPROM.put(address, CurProf.passwordEnabled);
    address += sizeof(CurProf.passwordEnabled);
    EEPROM.put(address, CurProf.vibrationDuration);
    address += sizeof(CurProf.vibrationDuration);        
    EEPROM.put(address, CurProf.dispenseEnabled);
	  address += sizeof(CurProf.dispenseEnabled);
    EEPROM.put(address, CurProf.sizeFlag);
    address += sizeof(CurProf.sizeFlag);
    EEPROM.put(address, CurProf.ZDip);
    address += sizeof(CurProf.ZDip);
    EEPROM.put(address, CurProf.skipCol);
    address += sizeof(CurProf.skipCol);
    EEPROM.put(address, CurProf.skipRow);
    address += sizeof(CurProf.skipRow);
    EEPROM.put(address, CurProf.skipSinglePos);
    address += sizeof(CurProf.skipSinglePos);
    EEPROM.put(address, CurProf.staggered);
    address += sizeof(CurProf.staggered);
	//Dsprintln("size");
	//Dprintln(sizeof(profile));
}

void ReadProfileEEPROM(int address)
{
	int x,y;
	byte dat;
	int bit;
  char buf[10];//v205
  
	Dprint("profile size=", (float)sizeof(CurProf));
	address=(address*300) + sizeof(uint8_t);//v205

    //EEPROM.get(address, profile.profileId);
    //address += sizeof(profile.profileId);
    EEPROM.get(address, CurProf.profileName);

    address += sizeof(CurProf.profileName);

    EEPROM.get(address, CurProf.Tube_No_x);
    address += sizeof(CurProf.Tube_No_x);
    EEPROM.get(address, CurProf.Tube_No_y);
    address += sizeof(CurProf.Tube_No_y);
    EEPROM.get(address, CurProf.pitch_x);
    address += sizeof(CurProf.pitch_x);
    EEPROM.get(address, CurProf.pitch_y);
    address += sizeof(CurProf.pitch_y);
    EEPROM.get(address, CurProf.trayOriginX);
    address += sizeof(CurProf.trayOriginX);
    EEPROM.get(address, CurProf.trayOriginY);
    address += sizeof(CurProf.trayOriginY);
    EEPROM.get(address, CurProf.Cycles);
    address += sizeof(CurProf.Cycles);
    EEPROM.get(address, CurProf.vibrationEnabled);
    address += sizeof(CurProf.vibrationEnabled);
    EEPROM.get(address, CurProf.passwordEnabled);
    address += sizeof(CurProf.passwordEnabled);
    EEPROM.get(address, CurProf.vibrationDuration);
    address += sizeof(CurProf.vibrationDuration);
    EEPROM.get(address, CurProf.dispenseEnabled);
    address += sizeof(CurProf.dispenseEnabled);
    EEPROM.get(address, CurProf.sizeFlag);
    address += sizeof(CurProf.sizeFlag);
    EEPROM.get(address, CurProf.ZDip);
    address += sizeof(CurProf.ZDip);
    EEPROM.get(address, CurProf.skipCol);
    address += sizeof(CurProf.skipCol);
    EEPROM.get(address, CurProf.skipRow);
    address += sizeof(CurProf.skipRow);
    EEPROM.get(address, CurProf.skipSinglePos);
    address += sizeof(CurProf.skipSinglePos);
    EEPROM.get(address, CurProf.staggered);
    address += sizeof(CurProf.staggered);

	Dprint("add=",(float)address );
  	CheckProfile();
}