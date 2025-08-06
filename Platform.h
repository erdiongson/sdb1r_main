
/*
 * Copyright (c) Riverdi Sp. z o.o. sp. k. <riverdi@riverdi.com>
 * Copyright (c) Skalski Embedded Technologies <contact@lukasz-skalski.com>
 */
#define FWVER "3.2"
#define DEBUG 0



#ifndef _PLATFORM_H_
#define _PLATFORM_H_

/*****************************************************************************/

// #define EVE_1		/* for FT80x series */
// #define EVE_2		/* for FT81x series */
#define EVE_4 /* for BT81x series */

/*****************************************************************************/

/*
 * Touch Screen:
 *   NTP_XX -> None
 *   RTP_XX -> Resisitve
 *   CTP_XX -> Capacitive
 *
 * Size:
 *   XXX_35 -> 3.5' TFT DISPLAY
 *   XXX_43 -> 4.3' TFT DISPLAY
 *   XXX_50 -> 5.0' TFT DISPLAY
 *   XXX_70 -> 7.0' TFT DISPLAY
 */

// #define NTP_35
// #define RTP_35
// #define CTP_35
#define IPS_35

// #define NTP_43
// #define RTP_43
// #define CTP_43

// #define NTP_50
// #define RTP_50
#define CTP_50

// #define NTP_70
// #define RTP_70
// #define CTP_70

/*****************************************************************************/

#define ARDUINO_PLATFORM
#define ARDUINO_PLATFORM_COCMD_BURST

#ifdef __AVR__
#define GPIO_CS 10 // 10//53
#define GPIO_PD 8

#define A0 (D14)
#define A1 (D15)
#define A2 (D16)
#define A3 (D17)
#define A4 (D18)
#define A5 (D19)

// #define SD_CS 5   // 4, SD card select pin
#endif

#ifdef ESP32 /* Riverdi IoT Display */
#define GPIO_CS 4
#define GPIO_PD 33
#endif

/* Standard C libraries */
#include <stdio.h>

/* Standard Arduino libraries */
#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#endif

/*****************************************************************************/

/* type definitions for EVE HAL library */

#define TRUE (1)
#define FALSE (0)
#define TAG_TOGGLE 25

#define dirPin0 27
#define stepPin0 25
#define motorInterfaceType 1
#define dirPin1 29
#define stepPin1 31

typedef char bool_t;
typedef char char8_t;
typedef unsigned char uchar8_t;
typedef signed char schar8_t;
typedef float float_t;

#ifdef ESP32 /* Riverdi IoT Display */
typedef PROGMEM const unsigned char prog_uchar8_t;
#endif

#ifdef __AVR__
typedef PROGMEM const unsigned char prog_uchar8_t;
typedef PROGMEM const char prog_char8_t;
typedef PROGMEM const uint8_t prog_uint8_t;
typedef PROGMEM const int8_t prog_int8_t;
typedef PROGMEM const uint16_t prog_uint16_t;
typedef PROGMEM const int16_t prog_int16_t;
typedef PROGMEM const uint32_t prog_uint32_t;
typedef PROGMEM const int32_t prog_int32_t;
#endif

/* Predefined Riverdi modules */
#include "Riverdi_Modules.h"

/* EVE inclusions */
#include "Gpu_Hal.h"
#include "Gpu.h"
#include "CoPro_Cmds.h"
#include "Hal_Utils.h"
#include "Assets.h"
#include <EEPROM.h>

#define ON 1
#define OFF 0
#define Font 27 // Font Size
#ifdef DISPLAY_RESOLUTION_WVGA
#if defined(MSVC_PLATFORM) || defined(MSVC_FT800EMU)
#define MAX_LINES 6 // Max Lines allows to Display
#else
#define MAX_LINES 5 // Max Lines allows to Display
#endif
#define MAXPROFILETEXT 31
#else
#define MAX_LINES 4
#endif
#define SPECIAL_FUN 250
#define BACK_SPACE 251 // Back space
#define CAPS_LOCK 252  // Caps Lock

#define NUMBER_LOCK 253 // Number Lock
#define SAVE_KEY 249    // Number sAVE FROM kEYPAD
#define CLEAR_KEY 254        // Exit
#define BACK 254
#define KBBACK 248    // Exit Keyboard
#define NUM_ENTER 247   // Enter Numeric Number
#define STOP 5
#define START 3
#define PAUSE 4
#define ERROR 6
#define SETTING 2
#define LOGO 0
#define MAINMENU 0
#define RUNMENU 1
#define PAUSEMENU 2
#define TXRXERROR 3
#define HOMEERROR 4

#define PROFILEBACK 6
#define PROFILELOAD 7
#define CONFIGADVANCE 8
#define PROFILEUP 9
#define PROFILEDOWN 10
#define PROFILEPASS 11
#define VIBLVL 28
#define PASSEN 29
#define VIBDURATION 30

#define MAXXMM 400 //190 (S) //max x/col dimension in mm
#define MAXYMM 330 //250 (S) //max y/row dimension in mm


#define LINE_STARTPOS DispWidth / 50 // Start of Line
#define LINE_ENDPOS DispWidth        // max length of the line

// #if EVE_CHIPID <= EVE_FT801
// #define ROMFONT_TABLEADDRESS (0xFFFFC)
// #else
// #define ROMFONT_TABLEADDRESS 3145724UL // 2F FFFCh
// #endif

// #include "Gpu_Hal.h"
// #include "Gpu_CoCmd.h"

#define NUM_ROWS 5
#define NUM_COLS 5
#define BUTTON_SIZE 300
#define BUTTON_PADDING 10
#define START_X 20
#define START_Y 20

//(S)
//#define STEPS_PER_UNIT_X 400L // (X step motor specs: lead screw is 4mm, 1600 pulse/rev(8 microsteps driver), 4mm/1600 = 0.0025mm, 1mm/0.0025 = 400 steps)
//#define STEPS_PER_UNIT_Y 320L // (Y step motor specs: lead screw is 10mm, 3200 pulse/rev(16 microsteps driver), 10mm/3200 = 0.003125mm, 1mm/0.003125 = 320 steps/unit)
//20240917: erdiongson - Use the STEPS_PER_UNIT_Y 160L if we are using the new motor. use this moving forward
//#define STEPS_PER_UNIT_Y 160L // (Y step motor specs: lead screw is 20mm, 1600 pulse/rev(8 microsteps driver), 20mm/3200 = 0.00625mm, 1mm/0.00625 = 160 steps/unit)

//(L)
//20240625: erdiongson - Trying out 3200 pulse per revolution (v2.05L)
#define STEPS_PER_UNIT_X 320L // (X step motor specs: lead screw is 10mm, 3200 pulse/rev(16 microsteps driver), 10mm/3200 = 0.003125mm, 1mm/0.003125 = 320 steps/unit)
//#define STEPS_PER_UNIT_X 160L // (X step motor specs: lead screw is 10mm, 3200 pulse/rev(16 microsteps driver), 10mm/3200 = 0.003125mm, 1mm/0.003125 = 320 steps/unit)
#define STEPS_PER_UNIT_Y 320L // (Y step motor specs: lead screw is 10mm, 3200 pulse/rev(16 microsteps driver), 10mm/3200 = 0.003125mm, 1mm/0.003125 = 320 steps/unit)
//20240917: erdiongson - Use the STEPS_PER_UNIT_Y 160L if we are using the new motor. use this moving forward
//#define STEPS_PER_UNIT_Y 160L // (Y step motor specs: lead screw is 20mm, 1600 pulse/rev(8 microsteps driver), 20mm/3200 = 0.00625mm, 1mm/0.00625 = 160 steps/unit)

extern Gpu_Hal_Context_t host, *phost;

extern int tag;


extern int32_t count;
extern int vibration;

extern int32_t selectedProfileId;

extern bool stop_flag;

extern uint8_t CurX;
extern uint8_t CurY;
extern uint16_t TotalTubeLeft;

extern uint16_t err_flag; //E1 = 1, E2 = 2;

void vibration_on(void);
void vibration_time(void);

#endif /*_PLATFORM_H_*/
