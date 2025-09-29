/* Author : XentiQ
Date created - 2022.12.14 - XentiQ version
*/

#ifndef _UART_H_
#define _UART_H_

#include <Arduino.h>

// Serial communication constants
#define Serial_SOT 0xEF  // Start of Transmission byte
#define Serial_EOT 0xFE  // End of Transmission byte
#define BUFFER_SIZE 5    // Size of the command/response buffer

// Command values
#define SDB_Handshake 0x06       // Handshake command
#define Vibrate_Mode_ON 0x54     // Command to set vibration mode ON
#define Vibrate_Mode_OFF 0xC2    // Command to set vibration mode OFF (Not used for 1RC)
#define Set_Vibration_Time 0x65  // Command to set vibration time
#define Set_Pause_Time 0x23      // Command to set vibration pause time
#define SDB_Dispense_START 0x46  // Command to start dispensing
#define SDB_Dispense_PAUSE 0x31  // Command to pause dispensing
#define SDB_Dispense_STOP 0x32   // Command to stop dispensing

// Response command values
#define ACKNOWLEDGE 0x60          // Acknowledgment response
#define VIBRATION_ON 0x1C         // Vibration ON response
#define VIBRATION_OFF 0x2C        // Vibration OFF response
#define DISPENSE_DONE 0x3C        // Dispense operation completed
#define IR_SENSOR_FAILURE 0xE1    // Error: IR sensor failure
#define MARKER_NOT_DETECTED 0xE2  // Error: Marker not detected

//Vibration Mode Commands
#define VIBMODE_U0 0x71  //former 0xC2, changed for 1RC
#define VIBMODE_U1 0x72  //former 0xB1, changed for 1RC
#define VIBMODE_U2 0x73  //former 0xB2, changed for 1RC
#define VIBMODE_U3 0x74  //former 0xB3, changed for 1RC
#define VIBMODE_U4 0x75  //former 0xB4, changed for 1RC

//Vibration Duration/Time Commands
#define VIBDUR_1 0x81  //1 second/s
#define VIBDUR_2 0x82  //2 second/s (default in SDB)
#define VIBDUR_3 0x83  //3 second/s
#define VIBDUR_4 0x84  //4 second/s
#define VIBDUR_5 0x85  //5 second/s

#define SERIAL_2_DEFAULT  //Comment this out for Serial 3 usage

// Response structure
typedef struct {
  uint8_t sot;      // Start of Transmission
  uint8_t command;  // Command/status byte
  uint8_t data1;    // Data byte 1
  uint8_t data2;    // Data byte 2
  uint8_t eot;      // End of Transmission
} Response;

// Command structure
typedef struct {
  uint8_t sot;      // Start of Transmission
  uint8_t command;  // Command byte
  uint8_t data1;    // Data byte 1
  uint8_t data2;    // Data byte 2
  uint8_t eot;      // End of Transmission
} Send;

#endif /*_UART_H_*/
