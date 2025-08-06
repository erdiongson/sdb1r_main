/* Author : XentiQ
Date created - 2022.12.14 - XentiQ version
*/

#include "UART.h"
#include "SaveProfile.h"
#include "App_Common.h"

extern bool pausestop_flag;
extern uint8_t ret_flag;

void flushSerialBuffer(Stream &serial)
{
    while (serial.available())
    {
        serial.read();
    }
}

void sendCommand(Command cmd)
{
    // byte buffer[BUFFER_SIZE] = {Serial_SOT, cmd, 0x00, 0x00, Serial_EOT};
    // Serial1.write(buffer, BUFFER_SIZE);
    unsigned char data1, data2;
    data1 = 0x00;
    
    //delay(50);
    if((cmd == VIBMODE_U0) ||
       (cmd == VIBMODE_U1) ||
       (cmd == VIBMODE_U2) ||
       (cmd == VIBMODE_U3) ||
       (cmd == VIBMODE_U4)){
       
       data1 = cmd;
       cmd = Vibrate_Mode_ON;
       data2 = data1 + cmd;
       //Serial2.write(cmd);
       //Serial2.write(data1);
     } else if((cmd == VIBDUR_1) ||
               (cmd == VIBDUR_2) ||
               (cmd == VIBDUR_3) ||
               (cmd == VIBDUR_4) ||
               (cmd == VIBDUR_5)){
       data1 = cmd;
       cmd = Set_Vibration_Time;
       data2 = data1 + cmd;                
     } /*else if(SDB_Dispense_START){
       if (CurProf.vibrationEnabled == 0){ //vibration Mode OFF
        data1 = 0x77;
       } else {                            //vibration mode ON
        data1 = 0x88;
       }
       data2 = cmd + data1;
     }*/
     else {
       data1 = 0x01;
       data2 = cmd + data1;
       //Serial2.write(cmd);
       //Serial2.write(data1);
     }
    
    //delay(100);
    Serial2.write(Serial_SOT);
    delay(50);
    Serial2.write(cmd);
    delay(50);
    Serial2.write(data1);
    delay(100);
    Serial2.write(data2);
    //delay(50);
    Serial2.write(Serial_EOT);

    // Serial Debugging
    Serial.print("Send Command:     0x");
    Serial.print(Serial_SOT, HEX);
    Serial.print(" 0x");
    Serial.print(cmd, HEX);
    Serial.print(" 0x");
    Serial.print(data1, HEX);
    Serial.print(" 0x");
    Serial.print(data2, HEX);
    Serial.print(" 0x");
    Serial.println(Serial_EOT, HEX);
}

Response receiveResponse()
{
    Serial.println("ReceiveResponse started...");
    Response resp;
    Response mirror;
    resp.command = 0xFF;
    resp.data1 = 0x00;
    resp.data2 = 0x00;
    resp.sot = 0x00;
    int trap_cntr = 0;
    pausestop_flag = false;
    ret_flag = 0;
//    if (Serial2.available() >= 5)
//    {
    while(Serial2.available() < 5){
      delay(100);
      trap_cntr++;
      if (trap_cntr == 200){
        err_flag = 4;
        trap_cntr = 0;
        ret_flag=PAUSE;
        pausestop_flag=TRUE;
        Serial.println("Error 4: Acknowledge Error");        
        break;
      }
      //Serial.println("Waiting for mirror acknowledge...");
    }
    mirror.sot     = Serial2.read();
    mirror.command = Serial2.read();
    mirror.data1   = Serial2.read();
    mirror.data2   = Serial2.read();
    mirror.eot     = Serial2.read();
    Serial.print("Mirrored: ");
    Serial.print(mirror.sot, HEX);
    Serial.print(" ");
    Serial.print(mirror.command, HEX);
    Serial.print(" ");
    Serial.print(mirror.data1, HEX);
    Serial.print(" ");
    Serial.print(mirror.data2, HEX);
    Serial.print(" ");
    Serial.println(mirror.eot, HEX);

  if ((mirror.command == 0x65) || //Motor Vibration Duration Setting (1 - 5)
      (mirror.command == 0x23)) //Motor Vibration Pause Time Setting (0 - 5)
  {
    if (mirror.sot == Serial_SOT)
    {
      resp.sot = mirror.sot;
      resp.command = mirror.command;
      Serial.print("0x65 / 0x23 resp.command: ");
      Serial.println(resp.command, HEX);            
      resp.data1 = mirror.data1;
      Serial.print("0x65 / 0x23 resp.data1: ");
      Serial.println(resp.data1, HEX);            
      resp.data2 = mirror.data2;
      Serial.print("0x65 / 0x23 resp.data2: ");
      Serial.println(resp.data2, HEX);            
      resp.eot = mirror.eot;
      Serial.print("0x65 / 0x23 resp.eot: ");
      Serial.println(resp.eot, HEX);
    }
  }
  else {
    while(Serial2.available() < 5){
      delay(100);
      if(!pausestop_flag)
      {
        ret_flag = GetKeyPressed();
        if(ret_flag==STOP || ret_flag==PAUSE) 
        {
          pausestop_flag=TRUE;
          Dprint("P_S","\n");
          Serial.println("On_response: P_S");
        }
      }
      trap_cntr++;
      if (trap_cntr == 200){
        err_flag = 5;
        trap_cntr = 0;
        ret_flag=PAUSE;
        pausestop_flag=TRUE;
        Serial.println("Error 5: Response Error");
        break;
      }
      //Serial.println("Waiting for response...");
    }      
    resp.sot = Serial2.read();
    Serial.print("resp.sot: ");
    Serial.println(resp.sot, HEX);
    if (resp.sot == Serial_SOT)
    {
      resp.command = Serial2.read();
      Serial.print("resp.command: ");
      Serial.println(resp.command, HEX);            
      resp.data1 = Serial2.read();
      Serial.print("resp.data1: ");
      Serial.println(resp.data1, HEX);            
      resp.data2 = Serial2.read();
      Serial.print("resp.data2: ");
      Serial.println(resp.data2, HEX);            
      resp.eot = Serial2.read();
      Serial.print("resp.eot: ");
      Serial.println(resp.eot, HEX);
    }
  }
//    }
//    else
//   {
//      Serial.print("Response not 5 characters.");
//    }
    Serial.println("ReceiveResponse Ended...");
    return resp;
}

Response handle_uart_command(Command cmd)
{
    flushSerialBuffer(Serial2);

    // Send the command
    // sendCommand(cmd);
    // delay(100); // Add a short delay
    // sendCommand(SDB_Dispense_STOP);
    Response response = receiveResponse();

    if (response.command == 0x60 && response.data1 == 0x3C)
    {
        Serial.print("Received Command: 0x");
        Serial.print(Serial_SOT, HEX);
        Serial.print(" 0x");
        Serial.print(response.command, HEX);
        Serial.print(" 0x");
        Serial.print(response.data1, HEX);
        Serial.print(" 0x0");
        Serial.print(response.data2, HEX);
        Serial.print(" 0x");
        Serial.println(Serial_EOT, HEX);
        Serial.println("Command successful!");
    }
// Run_profile();

    return response;
}
