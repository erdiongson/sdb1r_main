/* Author : XentiQ
* Date created - 2022.12.14 - XentiQ version
* Created by: soonhl
* 
* Author: erdiongson
* Date Created: 2023.12.22
* Version 2.03: i. Changed the super password from XQokay to superXQ
* 
* Date Created: 2024.02.26
* Version 2.03: ii. Revised the saved password logic
*/

#include "Platform.h"
#include "App_Common.h"
#include <EEPROM.h>
#include "Config.h"
#include "SaveProfile.h"
#include "XY_Table.h"
#include "UART.h"

Gpu_Hal_Context_t host, *phost;
Profile CurProf; //current profile
ProfileMin SelectProf; //current profile



uint8_t CurProfNum;//current profile id


AccelStepper stepper_x(1, Motor_x_CLK, Motor_x_CW);
AccelStepper stepper_y(1, Motor_y_CLK, Motor_y_CW);

//Profile profile; // Create a profile object
ActionState action;

volatile CommandState currentState = IDLE;
PauseState pauseState; //= {0, 0, 0, 0, 0, 0, 0, ActionState::MoveY, false, false};

uint8_t CurX;
uint8_t CurY;
uint16_t TotalTubeLeft;
uint16_t err_flag = 0; //E1 = 1, E2 = 2;


int currentCycle = 0;
#if DEBUG
char Password[4][PROFILE_NAME_MAX_LEN]={   "su",//super password
                      "in",//current password
                      "",//user enter password to be check
                      ""}; //user enter 2nd time password to be check


#else
char Password[4][PROFILE_NAME_MAX_LEN]={  "superXQ",//super password
                      "init1234",//current password
                      "",//user enter password to be check
                      ""}; //user enter 2nd time password to be check
#endif
bool SpecialMode;
volatile char OpMode = MANUAL_MODE;

//-----------------------------------------------johari-23092024 PLC-----------------------------------------------------------------------------------------------------------------------//
#define PLC_SERIAL Serial3

// Define the input and output byte sequences
 const byte expectedMessageStart[] = {0xEF, 0x30, 0x00, 0x30, 0xFE};
 const byte expectedMessageStop[] = {0xEF, 0x31, 0x00, 0x31, 0xFE};
 const byte expectedMessagePause[] = {0xEF, 0x32, 0x00, 0x32, 0xFE};
 const byte responseMessage[] = {0xEF, 0x17, 0x00, 0x17, 0xFE};
 const int messageLength = 5;
//---------------------------------------------------------------------------------------------------------------------johari-23092024 PLC------------------------------------------------// 

//20240906: erdiongson - for UART 3 ISR Protocol
char receivedByte;
volatile bool dataReceived = false;


//PAUSE/STOP UART HANDLER
bool pausestop_flag = FALSE;
uint8_t ret_flag=0;

#if DEBUG
void Dprint(char x)
{
	Serial.print(x,HEX);
}
void Dprint(String x)
{
	Serial.print(x);
}
void Dprint(String x, String y)
{
	Serial.print(x);
	Serial.print(y);
}
void Dprint(String x, float y)
{
	char buf[20];

	dtostrf(y,3,5,buf);
	Serial.print(x);
	Serial.println(buf);
}
void Dprint(String x, uint8_t y)
{
	char buf[20];

	sprintf(buf,"%d",y);
	Serial.print(x);
	Serial.println(buf);
}

#else
void Dprint(char x)
{}
void Dprint(String x)
{}
void Dprint(String x, String y)
{}
void Dprint(String x, uint8_t y)
{}
void Dprint(String x, float y)
{}

#endif

void GPIO_Setup()
{
  Serial.begin(19200);
  pinMode(Motor_ON, OUTPUT);
  pinMode(Motor_UP, OUTPUT);
  pinMode(Motor_Dow, OUTPUT);
  pinMode(Vibrate, OUTPUT);
  pinMode(Limit_S_x_MIN, INPUT_PULLUP);
  pinMode(Limit_S_y_MIN, INPUT_PULLUP);
  pinMode(Limit_S_x_MAX, INPUT_PULLUP);
  pinMode(Limit_S_y_MAX, INPUT_PULLUP);
}

void init_Motors()
{
   stepper_x.setSpeed(motor_x_speed);
  stepper_x.setMaxSpeed(motor_x_speed);
  stepper_x.setAcceleration(motor_x_Acceleration);

   stepper_y.setSpeed(motor_y_speed);
  stepper_y.setMaxSpeed(motor_y_speed);
  stepper_y.setAcceleration(motor_y_Acceleration);
  //stepper_x.setSpeed(10000);
  //stepper_y.setSpeed(10000);
}

bool Homing()
{
	bool successx=FALSE;
	bool successy=FALSE;
#if !DEBUG
  Serial.println("Homing Start...");
#else
	Dprint("Debugging Start...");
#endif
 
  stepper_x.moveTo(-40000);
#if !DEBUG
  while (digitalRead(Limit_S_x_MAX) != 0)
    stepper_x.run();
  delay(20);
  if(digitalRead(Limit_S_x_MAX)==0 && digitalRead(Limit_S_x_MIN)!=0) successx=TRUE;
#endif
  Serial.println(" limit x hit");
  stepper_x.stop();
  stepper_x.setCurrentPosition(0);
  stepper_x.setMaxSpeed(motor_y_speed);
  stepper_x.setAcceleration(motor_y_Acceleration);

  stepper_y.moveTo(-40000);
#if !DEBUG
  while (digitalRead(Limit_S_y_MIN) != 0)
    stepper_y.run();
  delay(20);
    if(digitalRead(Limit_S_y_MIN)==0 && digitalRead(Limit_S_y_MAX)!=0) successy=TRUE;
#endif
  Serial.println(" limit y hit");
  PLC_SERIAL.write(responseMessage, messageLength);//-------23092024--PLC----------------------------------------------johari------------------------------------------------  
  stepper_y.stop();
  stepper_y.setCurrentPosition(0);
  stepper_y.setMaxSpeed(motor_y_speed);
  stepper_y.setAcceleration(motor_y_Acceleration);

	return successx && successy;
  //bool complete = true; //----johari---20092024---------------------------------------------------------------------------------------------------------------
}

bool Zeroing()
{
	bool successx=FALSE;
	bool successy=FALSE;
#if !DEBUG
  Serial.println("Homing Start...");
#else
	Dprint("Debugging Start...");
#endif
 
  stepper_x.moveTo(40000);
#if !DEBUG
  while (digitalRead(Limit_S_x_MIN) != 0)
    stepper_x.run();
  delay(20);
  if(digitalRead(Limit_S_x_MIN)==0 && digitalRead(Limit_S_x_MAX)!=0) successx=TRUE;
#endif
  Serial.println(" limit x hit");
  stepper_x.stop();
  stepper_x.setCurrentPosition(0);
  stepper_x.setMaxSpeed(motor_y_speed);
  stepper_x.setAcceleration(motor_y_Acceleration);

  stepper_y.moveTo(-40000);
#if !DEBUG
  while (digitalRead(Limit_S_y_MIN) != 0)
    stepper_y.run();
  delay(20);
    if(digitalRead(Limit_S_y_MIN)==0 && digitalRead(Limit_S_y_MAX)!=0) successy=TRUE;
#endif
  Serial.println(" limit y hit");
  PLC_SERIAL.write(responseMessage, messageLength);//-------23092024--PLC----------------------------------------------johari------------------------------------------------  
  stepper_y.stop();
  stepper_y.setCurrentPosition(0);
  stepper_y.setMaxSpeed(motor_y_speed);
  stepper_y.setAcceleration(motor_y_Acceleration);

	return successx && successy;
  //bool complete = true; //----johari---20092024---------------------------------------------------------------------------------------------------------------
}

uint8_t runStepper_normal(AccelStepper &stepper, long distance, uint8_t limitPin)
{
	uint8_t ret=0;
	uint8_t localkeypressed=0;

  stepper.move(distance);
	Dprint("move motor","\n");
  //stepper.runToPosition();

  if (digitalRead(limitPin) == 0)
	{//stop motor
    ret=STOP;
    err_flag = 3;
	}
  if (PLC_SERIAL.available() >= messageLength)  //johari------23092024----------------------------------------------------------------------------------------------------------------
  {
    Serial.println("------------PLC Serial Message Available - STOP(runStepper_normal)------------");
    byte receivedMessage[messageLength];
    bool validMessageStop = true;
    bool validMessagePause = true;
    // Read the incoming bytes
    for (int i = 0; i < messageLength; i++)
    {
      receivedMessage[i] = PLC_SERIAL.read();
      if (receivedMessage[i] != expectedMessageStop[i])
      {
        validMessageStop = false; // If any byte doesn't match, it's not the expected message
      }
        if (receivedMessage[i] != expectedMessagePause[i])
      {
        validMessagePause = false; // If any byte doesn't match, it's not the expected message
      }
    }
    PLC_SERIAL.write(receivedMessage, messageLength);
    //if ((receivedMessage[1] != expectedMessageStop[1]) || (receivedMessage[1] != expectedMessagePause[1]))
    //{
      //validMessageStop = false; // If any byte doesn't match, it's not the expected message
    //}
    if (validMessageStop)
    {
     //delay(500);
     Serial.print("PLC Stop Button Pressed"); 
     ret=STOP;
     localkeypressed = STOP;
     Home_Menu(&host, MAINMENU);
     PLC_SERIAL.write(responseMessage, messageLength);//-------20092024--PLC-----------------------------------
    }
    if(validMessagePause)
    {
      Serial.println("PLC Pause Button Pressed.");
      ret=PAUSE;
      localkeypressed = PAUSE;
    }
  } // end--johari -------23092024--------------------------------------------------------------------------------------------------------------------------------------------------------  
  while ((stepper.distanceToGo() != 0) && (ret != STOP))
	{
	  if (digitalRead(limitPin) == 0)
	  {//stop motor
      ret=STOP;
      err_flag = 3;
	    break;
	  }
		if(localkeypressed==0) localkeypressed =Gpu_Hal_Rd8(phost, REG_TOUCH_TAG);
		if (localkeypressed==STOP || localkeypressed==PAUSE) break;
    stepper.run();
  }
  if (localkeypressed==STOP || localkeypressed==PAUSE)
  {
	  ret=localkeypressed;
	  if(localkeypressed==STOP) 
    {
      Home_Menu(&host, MAINMENU);
    }
    else Home_Menu(&host, PAUSEMENU);
		while (stepper.distanceToGo() != 0)//continue if motor not yet complete
		{	
		  if (digitalRead(limitPin) == 0)
		  {//stop motor
        ret=STOP;
        err_flag = 3;
		  	break;
		  }
		  stepper.run();
		}
		
  }
  stepper.stop();
  Dprint("motor_end","\n");
  Serial.println("END of runStepper_normal()--------------------");
  return ret;
}




uint8_t vibrateAndCheckPause()
{
	uint8_t i;
	bool pausestop=FALSE;
	uint8_t ret=0;

	Dprint("vib","\n");
	//sendCommand(Vibrate_Mode_ON);

	for(i=0;i<9;i++)
	{
	    if(!pausestop)
		{
			ret = GetKeyPressed();
			if(ret==STOP || ret==PAUSE) 
			{
				pausestop=TRUE;
				//Dprint("P_S","\n");
			}

	    }
    delay(100); 
		//delay(200); // Changed to 200 for 2 seconds vibration
	}
	Dprint("vib_end","\n");

	//sendCommand(Vibrate_Mode_OFF);

  return ret;
}

bool primeDispenserHead() {
  bool cont;
  for (int i = 0; i < PRIME_DISPENSE_NUM; i++) {
    cont = performVibrateAndDispenseOperations();
    if (cont == false) {
      return false;
    }
  }
  return true;
  // returns whether to continue
}

uint8_t dispenseAndCheckPause()
{
	uint8_t i;
	bool pausestop=FALSE;
	uint8_t ret=0;

	Dprint("dispen","\n");

    sendCommand(SDB_Dispense_START);
    //delay(100);
    while (receiveResponse().command != 0xF9)
    {
      //delay(100);
      if(!pausestop)
      {
        ret = GetKeyPressed();
        if(ret==STOP || ret==PAUSE) 
        {
          pausestop=TRUE;
          Dprint("P_S","\n");
          Serial.println("P_S");
        }
      }
      
      if(i>0) i--;
      else
      {
        Serial.println("Dispense Error");
        Serial.println(receiveResponse().command, HEX);
        Serial.println(receiveResponse().data1, HEX);
        Serial.println(receiveResponse().data2, HEX);  
        ret = STOP;
        err_flag = 1;
        return ret;
      }
      if (receiveResponse().command == 0xE1)
      {
        Serial.println("E1 Error");
        Serial.println(receiveResponse().command, HEX);
        Serial.println(receiveResponse().data1, HEX);
        Serial.println(receiveResponse().data2, HEX);
        ret = STOP;
        err_flag = 1;
        return ret;
      }
      else if (receiveResponse().command == 0xE2)
      {
        Serial.println("E2 Error");
        Serial.println(receiveResponse().command, HEX);
        Serial.println(receiveResponse().data1, HEX);
        Serial.println(receiveResponse().data2, HEX);
        ret = STOP;
        err_flag = 2;
        return ret;      
      }
      else
      {
        Serial.println("Dispense DONE");
        Serial.println(receiveResponse().command, HEX);
        Serial.println(receiveResponse().data1, HEX);
        Serial.println(receiveResponse().data2, HEX);
        err_flag = 0;
      }       
    }
    pausestop = pausestop_flag;
    ret = ret_flag;
      /*if(!pausestop)
      {
        ret = GetKeyPressed();
        if(ret==STOP || ret==PAUSE) 
        {
          pausestop=TRUE;
          Dprint("P_S","\n");
          Serial.println("P_S");
        }
      }*/
	Dprint("dispen_end","\n");
  return ret;//soon
}

bool PauseOperation(void)
{
	uint8_t localkeypressed = 0;
	bool ret=true;//return true if continue. return false if hardstop
  byte receivedMessage[messageLength];
  bool validMessage = false;
    
	Home_Menu(&host, PAUSEMENU);

	while (1)
	{
	  localkeypressed = GetKeyPressed();
    if (PLC_SERIAL.available() >= messageLength)
    {
      Serial.println("------------PLC Serial Message Available - START------------");    
    // Read the incoming bytes
      for (int i = 0; i < messageLength; i++)
      {
        receivedMessage[i] = PLC_SERIAL.read();
        if (receivedMessage[i] != expectedMessageStart[i])
        {
          validMessage = false; // If any byte doesn't match, it's not the expected message
        }
        else {
          validMessage = true;
        }
      }
    }
    if (validMessage)
    {
      localkeypressed = START;
    }
	  if(localkeypressed == START || localkeypressed == STOP ) 
	  {
		  if(localkeypressed == START ) 
		  {
		  	Home_Menu(&host, RUNMENU);
			break;
		  }
		  else 
		  {//when stop pressed
		  	Home_Menu(&host,MAINMENU);
			ret=FALSE;
			break;
		  }
	  }
	  
	}

	return ret;
}


void startProcess() 
{
	uint8_t temp;
  	int cycle;
	int direction;
	int x=0;
	int y=0;
	char buf[20];


	temp=offsetStepperPosition();
	if(temp!=0) 
	{
		if(temp==STOP)	return;
		else if(!PauseOperation()) return;
	}

    for (y =  0; y < CurProf.Tube_No_y; y++)
    {
    Serial.println("*************** Move Y Axis ***************");
		if (y!=0) temp=runStepper_normal(stepper_y, CurProf.pitch_y * STEPS_PER_UNIT_Y, Limit_S_y_MAX);
		CurY=y;
     	Home_Menu(&host, RUNMENU);
		if(temp!=0) 
	   	{
	   		if(temp==STOP)	return;
	   		else if(!PauseOperation()) return;
		}
    Serial.println("*************** Stop Y Axis ***************");
		if(CurProf.buttonStates[CurX][CurY])
		{
			for (cycle = 0; cycle < CurProf.Cycles; cycle++)
		  	{
        if (!performVibrateAndDispenseOperations()){
          Serial.println("*************** Stop Y Axis ***************");
          Serial.println("START DISPENSE");
          return;
        }
        
			}
		}

    	if(TotalTubeLeft>0) TotalTubeLeft--;
    	Home_Menu(&host, RUNMENU);
		for (x = 0; x < CurProf.Tube_No_x - 1; x++)
      	{
			
			Dprint("Cycle y  x","\n");
		  	Dprint(cycle);
		  	Dprint("	");
		  	Dprint(y);
		  	Dprint("  ");
		  	Dprint(x);
		  	Dprint("\n");
       
	         direction = y % 2 == 0 ? -1 : 1;
			if(direction==-1) CurX++;
			else 
			{
			if(CurX>0) CurX--;
			}
          	Home_Menu(&host, RUNMENU);
			

    	    temp=runStepper_normal(stepper_x, direction * CurProf.pitch_x * STEPS_PER_UNIT_X, Limit_S_x_MAX);
			if(temp!=0) 
			{
				if(temp==STOP)  return;
			 	else if(!PauseOperation()) return;
		 	}
			if(CurProf.buttonStates[CurX][CurY])
			{
				for (cycle = 0; cycle < CurProf.Cycles; cycle++)
				{
          //if (!performVibrateAndDispenseOperations()) return;
          if (!performVibrateAndDispenseOperations()){
            Serial.println("*************** Stop X Axis ***************");
            Serial.println("START DISPENSE");
            return;
          }
				}
			}
      if(TotalTubeLeft>0) TotalTubeLeft--;
          Home_Menu(&host, RUNMENU);

      }

      if (y == CurProf.Tube_No_y - 1 && cycle < CurProf.Cycles - 1)
	  {
	  	Homing();
	  }
	}
}


bool performVibrateAndDispenseOperations()
{
	bool_t ret=true; // return true if continue. return false if hardstop
	uint8_t pausestop=0;

	if(CurProf.vibrationEnabled) pausestop=vibrateAndCheckPause();

  if (PLC_SERIAL.available() >= messageLength)  //johari------23092024-----------------------------------------------------------------------------------------------------------------
  {
    Serial.println("------------PLC Serial Message Available - STOP(performVibrateAndDispenseOperations)------------");
    byte receivedMessage[messageLength];
    bool validMessageStop = true;
    bool validMessagePause = true;
    // Read the incoming bytes
    for (int i = 0; i < messageLength; i++)
    {
      receivedMessage[i] = PLC_SERIAL.read();
      if (receivedMessage[i] != expectedMessageStop[i])
      {
        validMessageStop = false; // If any byte doesn't match, it's not the expected message
      }
        if (receivedMessage[i] != expectedMessagePause[i])
      {
        validMessagePause = false; // If any byte doesn't match, it's not the expected message
      }
    }
    
    PLC_SERIAL.write(receivedMessage, messageLength);

    //if ((receivedMessage[1] != expectedMessageStop[1]) || (receivedMessage[1] != expectedMessagePause[1]))
    //{
      //validMessageStop = false; // If any byte doesn't match, it's not the expected message
    //}
    if (validMessageStop)
    {
      delay(500);
      Serial.print("PLC Stop Button Pressed"); 
      ret=FALSE;
      Home_Menu(&host, MAINMENU);
      PLC_SERIAL.write(responseMessage, messageLength);//-------20092024--PLC-----------------------------------
    }
    if(validMessagePause)
    {
      Serial.println("PLC Pause Button Pressed.");
      ret=PAUSE;
      pausestop = PAUSE;
    }
  } // end--johari -------23092024---------------------------------------------------------------------------------------------------------------------------------------------------------    

	if(pausestop==STOP) Home_Menu(&host,  MAINMENU);
	if(pausestop==PAUSE) if(!PauseOperation()) pausestop=STOP;
	if(pausestop!=STOP)
	{	
		pausestop=dispenseAndCheckPause();
		if(pausestop==STOP) Home_Menu(&host,  MAINMENU);
 		if(pausestop==PAUSE) if(!PauseOperation()) pausestop=STOP;
	}
	if(pausestop!=STOP)
	{
		if(CurProf.vibrationEnabled) pausestop=vibrateAndCheckPause();
		if(pausestop==STOP) Home_Menu(&host,  MAINMENU);
		if(pausestop==PAUSE ) if(!PauseOperation()) pausestop=STOP;
	}

	if(pausestop==STOP) ret=FALSE;//hardstop
  Serial.println("END of performVibrateAndDispenseOperation()--------------------");
	return ret;
}

/**********************************************************************************************************
* @brief offsetStepperPosition()
* @details Control the motor to go to Origin X and Origin Y Offset
* @param void
* @return uint8_t ret
**********************************************************************************************************/
uint8_t offsetStepperPosition(void)
{
	uint8_t keypressed=0;
	uint8_t ret=0;
	int xMoveValue;
	int yMoveValue;

  // Calculate the moveTo values for both steppers
  xMoveValue = CurProf.trayOriginX * STEPS_PER_UNIT_X;
  yMoveValue = CurProf.trayOriginY * STEPS_PER_UNIT_Y;

  	// Print the original and calculated offset values
	Serial.print("Tray Origin X: ");
	Serial.println(CurProf.trayOriginX);
	Serial.print("Tray Origin Y: ");
	Serial.println(CurProf.trayOriginY);

	Serial.print("Stepper X MoveTo Value: ");
	Serial.println(xMoveValue);
	Serial.print("Stepper Y MoveTo Value: ");
	Serial.println(yMoveValue);

  	// Use the calculated values to move the steppers
  	stepper_x.moveTo(-xMoveValue);
  	stepper_y.moveTo(yMoveValue);

	while (stepper_x.distanceToGo() != 0 || stepper_y.distanceToGo() != 0)
	{
    if (PLC_SERIAL.available() >= messageLength)  //johari------23092024----------------------------------------------------------------------------------------------------------------
    {
      Serial.println("------------PLC Serial Message Available - STOP(offsetStepperPosition)------------");
      byte receivedMessage[messageLength];
      bool validMessageStop = true;
      bool validMessagePause = true;
      // Read the incoming bytes
    for (int i = 0; i < messageLength; i++)
    {
      receivedMessage[i] = PLC_SERIAL.read();
      if (receivedMessage[i] != expectedMessageStop[i])
      {
        validMessageStop = false; // If any byte doesn't match, it's not the expected message
      }
        if (receivedMessage[i] != expectedMessagePause[i])
      {
        validMessagePause = false; // If any byte doesn't match, it's not the expected message
      }
    }
      PLC_SERIAL.write(receivedMessage, messageLength);
      //if ((receivedMessage[1] != expectedMessageStop[1]) || (receivedMessage[1] != expectedMessagePause[1]))
      //{
        //validMessageStop = false; // If any byte doesn't match, it's not the expected message
      //}
      if (validMessageStop)
      {
       delay(500);
       Serial.print("DATA STOP SENT SATU DUA"); 
       ret=FALSE;
       Home_Menu(&host, MAINMENU);
       PLC_SERIAL.write(responseMessage, messageLength);//-------20092024--PLC-----------------------------------
      }
      if(validMessagePause)
      {
        Serial.println("PLC Pause Button Pressed.");
        ret=PAUSE;
        keypressed = PAUSE;
      }
    } // end--johari -------23092024--------------------------------------------------------------------------------------------------------------------------------------------------------  
		if(keypressed==0) keypressed =Gpu_Hal_Rd8(phost, REG_TOUCH_TAG);
		if (keypressed==STOP || keypressed==PAUSE) break;
		stepper_x.run();
		stepper_y.run();
	}
	if (keypressed==STOP || keypressed==PAUSE)
	{
	    ret=keypressed;
		if(keypressed==STOP) 
		{
			Home_Menu(&host, MAINMENU);
		}
		else Home_Menu(&host, PAUSEMENU);
		while (stepper_x.distanceToGo() != 0 || stepper_y.distanceToGo() != 0)
		{	
			stepper_x.run();
			stepper_y.run();
		}
	}
  Serial.println("END of offsetStepperPosition()--------------------");
  return ret;
}

/**********************************************************************************************************
* @brief vibration_on()
* @details Command for changing the vibration levels from U0-U4.
* @param void
* @return void
**********************************************************************************************************/
void vibration_on() {
  int i;
  if (CurProf.vibrationEnabled == 0){
    CurProf.vibrationEnabled = 1;
//#ifndef TEST_LCD_ONLY    
    sendCommand(VIBMODE_U1);
    i=10;
    while (receiveResponse().data1 != VIBMODE_U1) //Changed for 1RC
    {
      delay(1000);
      if(i>0) i--;
      else
      {
        Serial.println("U1 Error");
        Serial.println(receiveResponse().command, HEX);
        Serial.println(receiveResponse().data1, HEX);
        Serial.println(receiveResponse().data2, HEX);  
      }    
    }
//#endif    
  }
  else if (CurProf.vibrationEnabled == 1){
    CurProf.vibrationEnabled = 2;
//#ifndef TEST_LCD_ONLY      
    sendCommand(VIBMODE_U2);
    i=10;
    while (receiveResponse().data1 != VIBMODE_U2) //Changed for 1RC
    {
      delay(1000);
      if(i>0) i--;
      else
      {
        Serial.println("U1 Error");
        Serial.println(receiveResponse().command, HEX);
        Serial.println(receiveResponse().data1, HEX);
        Serial.println(receiveResponse().data2, HEX);  
      }    
    }
//#endif    
  }
  else if (CurProf.vibrationEnabled == 2) {
    CurProf.vibrationEnabled = 3;
//#ifndef TEST_LCD_ONLY    
    sendCommand(VIBMODE_U3);
    i=10;
    while (receiveResponse().data1 != VIBMODE_U3) //Changed for 1RC
    {
      delay(1000);
      if(i>0) i--;
      else
      {
        Serial.println("U1 Error");
        Serial.println(receiveResponse().command, HEX);
        Serial.println(receiveResponse().data1, HEX);
        Serial.println(receiveResponse().data2, HEX);  
      }    
    }
//#endif    
  }
  else if (CurProf.vibrationEnabled == 3){
    CurProf.vibrationEnabled = 4;
//#ifndef TEST_LCD_ONLY      
    sendCommand(VIBMODE_U4);
    i=10;
    while (receiveResponse().data1 != VIBMODE_U4) //Changed for 1RC
    {
      delay(1000);
      if(i>0) i--;
      else
      {
        Serial.println("U1 Error");
        Serial.println(receiveResponse().command, HEX);
        Serial.println(receiveResponse().data1, HEX);
        Serial.println(receiveResponse().data2, HEX);  
      }    
    }
//#endif    
  }
  else {
    CurProf.vibrationEnabled = 0;
//#ifndef TEST_LCD_ONLY      
    sendCommand(VIBMODE_U0);
    i=10;
    while (receiveResponse().data1 != VIBMODE_U0) //Changed for 1RC
    {
      delay(1000);
      if(i>0) i--;
      else
      {
        Serial.println("U1 Error");
        Serial.println(receiveResponse().command, HEX);
        Serial.println(receiveResponse().data1, HEX);
        Serial.println(receiveResponse().data2, HEX);  
      }    
    }
//#endif    
  }
}
/**********************************************************************************************************
* @brief vibration_time()
* @details Command for changing the vibration duration/time from 1 to 5 seconds.
* @param void
* @return void
**********************************************************************************************************/
void vibration_time() {
  int i;
  int vibtime_err_flag = 0;
  if (CurProf.vibrationDuration == 2){
    CurProf.vibrationDuration = 3;
    sendCommand(VIBDUR_3);
    i=10;
    while (receiveResponse().data1 != VIBDUR_3) //Change for 1RC Application
    {
      if(i>0) i--;
      else{ vibtime_err_flag = 1;}    
    }
  } else if (CurProf.vibrationDuration == 3){
    CurProf.vibrationDuration = 4;
    sendCommand(VIBDUR_4);
    i=10;
    while (receiveResponse().data1 != VIBDUR_4) //Change for 1RC Application
    {
      if(i>0) i--;
      else{ vibtime_err_flag = 1;}    
    }   
  } else if (CurProf.vibrationDuration == 4){
    CurProf.vibrationDuration = 5;
    sendCommand(VIBDUR_5);
    i=10;
    while (receiveResponse().data1 != VIBDUR_5) //Change for 1RC Application
    {
      if(i>0) i--;
      else{ vibtime_err_flag = 1;}    
    }   
  } else if (CurProf.vibrationDuration == 5){
    CurProf.vibrationDuration = 1;
    sendCommand(VIBDUR_1);
    i=10;
    while (receiveResponse().data1 != VIBDUR_1) //Change for 1RC Application
    {
      if(i>0) i--;
      else{ vibtime_err_flag = 1;}    
    }   
  } else {
    CurProf.vibrationDuration = 2;
    sendCommand(VIBDUR_2);
    i=10;
    while (receiveResponse().data1 != VIBDUR_2) //Change for 1RC Application
    {
      if(i>0) i--;
      else{ vibtime_err_flag = 1;}    
    }    
  }
  if (vibtime_err_flag == 1){
    Serial.println("Vib Time Error");
    Serial.println(receiveResponse().command, HEX);
    Serial.println(receiveResponse().data1, HEX);
    Serial.println(receiveResponse().data2, HEX);  
  }
}
  
/*********************END OF FUNCTION DECLARATIONS*********************/

void setup()
{
	int i;
	phost = &host;


	SpecialMode=FALSE;
 	App_Common_Init(&host); //* Init HW Hal */
  //  App_Calibrate_Screen(&host); ///*Screen Calibration*//

 	GPIO_Setup();
 	init_Motors();

 	Serial.begin(19200);  // Serial printing
  Serial2.begin(19200); // UART for Arduino-PIC18 communications
  Serial3.begin(19200); // UART for Arduino-PIC18 communications

  Serial.print("Setup Serial 3 as interrupt");
  //20240906: erdiongson - Enable interrupt for UART 3 receive complete
  UCSR3B |= (1 << RXCIE3);

  Serial.print("Enable Global Interrupt");
  // Enable global interrupts
  sei();
  //END
  
	//Gpu_Hal_Wr8(phost, REG_PWM_DUTY, 10);//brightness control

  // handle_uart_command(Handshake);

  	Logo_XQ_trans(&host);
  	Dprint("Firmware version :",FWVER);

	Gpu_Hal_Wr8(phost,REG_TOUCH_SETTLE,3);
  // Disble/Enable handshaking
  	sendCommand(Handshake);

#if !DEBUG
	i=10;
    while (receiveResponse().command != 0x60)
	{
      	delay(100);
	  	if(i>0) i--;
	  	else
	  	{
			Home_Menu(&host, TXRXERROR);
		}
	
	 }
#endif 

  	//sendCommand(Vibrate_Mode_OFF);
  	//sendCommand(SDB_Dispense_STOP);
	
#if !DEBUG

    if( digitalRead(Limit_S_x_MIN) == 0)
    {
      Dprint("motor x out","\n");
      stepper_x.moveTo(-2000);
      //while (digitalRead(Limit_S_x_MIN) == 0)
      while (stepper_x.distanceToGo() != 0)
        stepper_x.run();
        delay(1000);
    }
    if( digitalRead(Limit_S_y_MIN) == 0)
    {
      Dprint("motor y out","\n");
      stepper_y.moveTo(2000);
      //while (digitalRead(Limit_S_x_MIN) == 0)
      while (stepper_y.distanceToGo() != 0)
        stepper_y.run();
        delay(1000);
    }
     
#endif


    if(!Homing())
	{
#if !DEBUG
		Home_Menu(&host, HOMEERROR);
		while(1);
#endif
	}
  Serial.print("Setup Password[1]: ");
  Serial.println(Password[1]);
  Serial.print("Setup Password[2]: ");
  Serial.println(Password[2]);
	ReadPassEEPROM(Password[2]);
  Serial.print("ReadPassEEPROM Password[2]: ");
  Serial.println(Password[2]);

	if(strcmp(Password[1],Password[2])==0)
	{
    //20240226: erdiongson - Revised the password copying logic to Password[1]
	  strcpy(Password[1],Password[2]);//if eeprom is blank load preset value (locked to init1234)
    Serial.println("Initial Password used.");
	}
  else
  {
    //20240226: erdiongson - If the password changed, it will automatically save to Password[1]
    ReadPassEEPROM(Password[1]); //if they changed the password other than init1234
    Serial.println("Password changed.");
  }
  Serial.print("New Password[1]: ");
  Serial.print(Password[1]);
  Serial.println();  
  	CurProfNum=LoadProfile();
    //Vibration Level Initialization
    if (CurProf.vibrationEnabled == 0)
    {
      CurProf.vibrationEnabled = 4;
    }
    else
    {
      CurProf.vibrationEnabled = CurProf.vibrationEnabled - 1;
    }
#if !DEBUG    
    vibration_on();
#endif    

    //Vibration Duration Initialization
    if (CurProf.vibrationDuration == 1)
    {
      CurProf.vibrationDuration = 5;
    }
    else
    {
      CurProf.vibrationDuration = CurProf.vibrationDuration - 1;
    }
    vibration_time();
  	Home_Menu(&host, MAINMENU);
}

void loop()
{
	uint8_t keypressed;
	char buf[PROFILE_NAME_MAX_LEN];
	int i;
	
	

  switch (OpMode){
    case (MANUAL_MODE):
      keypressed = GetKeyPressed(); // get button pressed

      if (keypressed!=0)
      {
        Home_Menu(&host,MAINMENU);
        switch(keypressed)
        {
          case START: Dprint("Cycle start","\n");
            CurX=0;
            CurY=0;
            TotalTubeLeft=(CurProf.Tube_No_x)*(CurProf.Tube_No_y);
            err_flag = 0;
            Home_Menu(&host, RUNMENU);
            WaitKeyRelease();
            Zeroing();
            primeDispenserHead();
            startProcess();
            Dprint("Cycle end","\n");
            Homing();
            //CurX=0;
            //CurY=0;
            Home_Menu(&host, MAINMENU);
            keypressed=0;
            break;
          case SETTING: // Setting button
            Dprint("Enter Setting");
            if(digitalRead(Limit_S_y_MAX)==0) SpecialMode=TRUE;
            else SpecialMode=FALSE;
            Password[2][0]=0;
            WaitKeyRelease();
            if (CurProf.passwordEnabled){
              Keyboard(phost,Password[2],"Enter Password",FALSE);
              if(strcmp(Password[1],Password[2])==0 || strcmp(Password[0],Password[2])==0)
              {
                if(SpecialMode) 
                {
                  if(digitalRead(Limit_S_y_MAX)!=0)
                  {
                    BlankEEPROM();
                  }
                  else SpecialMode=FALSE;
                }
                DisplayConfig(phost);
                WaitKeyRelease();
                delay(100);
                Config_Settings(phost);
              }
              else
              {
                DisplayKeyboard(phost,0,"Wrong Password"," ", FALSE, FALSE, FALSE);
                delay(2000);
              }
            } else {
                DisplayConfig(phost);
                WaitKeyRelease();
                delay(100);
                Config_Settings(phost);
            }
            Home_Menu(&host, MAINMENU);
            keypressed=0;
            delay(100);
            break;
          default :
            keypressed=0;
            break;
        }
      }//END if keypressed - case MANUAL_MODE
    case (AUTO_MODE):
      break;
  }//END switch (OpMode)
  //Gpu_Hal_Close(phost);
  //Gpu_Hal_DeInit();
//----------------------------------------------------------PLC-23092024--------------------------------------------------------------------------------------------------------------------
  
  if (PLC_SERIAL.available() >= messageLength)
   {
    Serial.println("------------PLC Serial Message Available - START------------");    
    byte receivedMessage[messageLength];
    bool validMessage = true;
    bool complete = false; 
       //uint8_t ret= 0; //johari---------17092024-------------------------------------
    // Read the incoming bytes
    for (int i = 0; i < messageLength; i++)
     {
      receivedMessage[i] = PLC_SERIAL.read();
      if (receivedMessage[i] != expectedMessageStart[i])
      {
        validMessage = false; // If any byte doesn't match, it's not the expected message
      }
     }
 
    if (validMessage)
     {
      
      CurX=0;
      CurY=0;
      TotalTubeLeft=(CurProf.Tube_No_x)*(CurProf.Tube_No_y);
      Home_Menu(&host, RUNMENU);
      WaitKeyRelease();
      Zeroing();
      primeDispenserHead();
      startProcess();
      Dprint("Cycle end","\n");
      Homing();   
      Home_Menu(&host, MAINMENU);
      keypressed=0;
      Serial.println("------------------------------------------------------------");
      Serial.print("DATA START START START"); 
//      if (complete=true)
//       {
//        PLC_SERIAL.write(responseMessage, messageLength);//-------23092024--PLC----------------------------------------------johari-------------------------------------------------
//        //complete = false; 
//        Serial.print("DATA KIRIM KIRIM KIRIM"); 
//        Serial.print(complete);
//       }
     }   
   }
//--------------------------------------johari-23092024---------------------------------------------------------------------------------------------------------------------------------
}
