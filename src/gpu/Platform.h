
/*
 * Copyright (c) Riverdi Sp. z o.o. sp. k. <riverdi@riverdi.com>
 * Copyright (c) Skalski Embedded Technologies <contact@lukasz-skalski.com>
 */

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

// #if EVE_CHIPID <= EVE_FT801
// #define ROMFONT_TABLEADDRESS (0xFFFFC)
// #else
// #define ROMFONT_TABLEADDRESS 3145724UL // 2F FFFCh
// #endif

// #include "Gpu_Hal.h"
// #include "Gpu_CoCmd.h"

extern Gpu_Hal_Context_t host, *phost;

#endif /*_PLATFORM_H_*/
