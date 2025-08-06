/* Author : XentiQ
Date created - 2022.12.14 - XentiQ version
*/

#ifndef _UART_H_
#define _UART_H_

#include <Arduino.h>

#define Serial_SOT 0xEF
#define Serial_EOT 0xFE
#define BUFFER_SIZE 5

//Command Responses from PIC Microcontroller
#define ACKNOWLEDGE         0x60
#define VIBRATION_ON        0x1C
#define VIBRATION_OFF       0x2C
#define DISPENSE_DONE       0x3C
#define IR_SENSOR_FAILURE   0xE1
#define MARKER_NOT_DETECTED 0xE2

//Vibration Mode Commands
#define VIBMODE_U0          0x71  //former 0xC2, changed for 1RC
#define VIBMODE_U1          0x72  //former 0xB1, changed for 1RC
#define VIBMODE_U2          0x73  //former 0xB2, changed for 1RC
#define VIBMODE_U3          0x74  //former 0xB3, changed for 1RC
#define VIBMODE_U4          0x75  //former 0xB4, changed for 1RC

//Vibration Duration/Time Commands
#define VIBDUR_1          0x81  //1 second/s
#define VIBDUR_2          0x82  //2 second/s (default in SDB)
#define VIBDUR_3          0x83  //3 second/s
#define VIBDUR_4          0x84  //4 second/s
#define VIBDUR_5          0x85  //5 second/s

#define SERIAL_2_DEFAULT  //Comment this out for Serial 3 usage

typedef struct Response
{
  uint8_t sot;
  uint8_t command;
  uint8_t data1;
  uint8_t data2;
  uint8_t eot;
} Response;

typedef struct Send
{
  uint8_t sot;
  uint8_t command;
  uint8_t data1;
  uint8_t data2;
  uint8_t eot;
} Send;
enum CommandState
{
  IDLE,
  HANDSHAKE,
  START_BUTTON,
  PAUSE_BUTTON,
  STOP_BUTTON,
  ERROR1,
  ERROR2,
  HOMING
};

enum Command
{
  Handshake = 0x06,
  Vibrate_Mode_ON = 0x54,       //former 0xC1, changed for 1RC
  Vibrate_Mode_OFF = 0xC2,      //Not used for 1RC
  Set_Vibration_Time = 0x65,    //20241001: Added Feature to Set Vibration Time
  Set_Pause_Time = 0x23,        //20241001: Ready Feature to Set Vibration Pause Time
  SDB_Dispense_START = 0x46,    //former 0xC3, changed for 1RC
  SDB_Dispense_PAUSE = 0x31,    //former 0xC4, changed for 1RC    
  SDB_Dispense_STOP = 0x32      //former 0xC5, changed for 1RC
};

enum ResponseType
{
  Acknowledge = 0x60,
  Vibration_ON = 0x1C,
  Vibration_OFF = 0x2C,
  Dispense_DONE = 0x3C,
  Error_IR_Censor_Failure = 0xE1,
  Error_Marker_Not_Detected = 0xE2
};

void flushSerialBuffer(Stream &serial);
void sendCommand(Command cmd);

Response receiveResponse();
Response handle_uart_command(Command cmd);

#endif /*_UART_H_*/
