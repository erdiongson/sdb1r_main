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

#include "src/ui/Platform.h"
#include "src/ui/App_Common.h"
#include <EEPROM.h>
#include "Config.h"
#include "SaveProfile.h"
#include "XY_Table.h"
#include "src/logic/DispenserHead.h"
#include "src/logic/ModeController.h"

Gpu_Hal_Context_t host, *phost;
Profile CurProf;        //current profile
ProfileMin SelectProf;  //current profile

uint8_t CurProfNum;  //current profile id

AxisParams xAxis(
  Motor_x_CLK, Motor_x_CW,
  Limit_S_x_MIN, Limit_S_x_MAX,
  motor_y_speed, motor_y_Acceleration);

AxisParams yAxis(
  Motor_y_CLK, Motor_y_CW,
  Limit_S_y_MIN, Limit_S_y_MAX,
  motor_y_speed, motor_y_Acceleration);

AxisParams zAxis(
  Motor_z_CLK, Motor_z_CW,
  Limit_S_z_MIN, Limit_S_z_MAX,
  motor_y_speed, motor_y_Acceleration);

DispenserHeadParams params = { xAxis, yAxis, zAxis, Serial2 };
DispenserHead dispenserHead(params);
ModeController modeController(dispenserHead);

uint16_t err_flag = 0;  //E1 = 1, E2 = 2;

#if DEBUG
char Password[4][PROFILE_NAME_MAX_LEN] = { "su",  //super password
                                           "in",  //current password
                                           "",    //user enter password to be check
                                           "" };  //user enter 2nd time password to be check


#else
char Password[4][PROFILE_NAME_MAX_LEN] = { "superXQ",   //super password
                                           "init1234",  //current password
                                           "",          //user enter password to be check
                                           "" };        //user enter 2nd time password to be check
#endif
bool SpecialMode;
volatile char OpMode = MANUAL_MODE;

#if DEBUG
void Dprint(char x) {
  Serial.print(x, HEX);
}
void Dprint(String x) {
  Serial.print(x);
}
void Dprint(String x, String y) {
  Serial.print(x);
  Serial.print(y);
}
void Dprint(String x, float y) {
  char buf[20];

  dtostrf(y, 3, 5, buf);
  Serial.print(x);
  Serial.println(buf);
}
void Dprint(String x, uint8_t y) {
  char buf[20];

  sprintf(buf, "%d", y);
  Serial.print(x);
  Serial.println(buf);
}

#else
void Dprint(char x) {}
void Dprint(String x) {}
void Dprint(String x, String y) {}
void Dprint(String x, uint8_t y) {}
void Dprint(String x, float y) {}

#endif

void GPIO_Setup() {
  Serial.begin(19200);
  pinMode(Motor_ON, OUTPUT);
  pinMode(Motor_UP, OUTPUT);
  pinMode(Motor_Dow, OUTPUT);
  pinMode(Vibrate, OUTPUT);
  pinMode(Limit_S_x_MIN, INPUT_PULLUP);
  pinMode(Limit_S_y_MIN, INPUT_PULLUP);
  pinMode(Limit_S_z_MIN, INPUT_PULLUP);
  pinMode(Limit_S_x_MAX, INPUT_PULLUP);
  pinMode(Limit_S_y_MAX, INPUT_PULLUP);
  pinMode(Limit_S_z_MAX, INPUT_PULLUP);
}

/**********************************************************************************************************
* @brief vibration_on()
* @details Command for changing the vibration levels from U0-U4.
* @param void
* @return void
**********************************************************************************************************/
void vibration_on() {
  bool success = false;

  // Cycle through vibration levels
  if (CurProf.vibrationEnabled == 0) {
    CurProf.vibrationEnabled = 1;
    success = dispenserHead.set_vibration_level(1);  // VIBMODE_U1
    if (!success) Serial.println("U1 Error");
  } else if (CurProf.vibrationEnabled == 1) {
    CurProf.vibrationEnabled = 2;
    success = dispenserHead.set_vibration_level(2);  // VIBMODE_U2
    if (!success) Serial.println("U2 Error");
  } else if (CurProf.vibrationEnabled == 2) {
    CurProf.vibrationEnabled = 3;
    success = dispenserHead.set_vibration_level(3);  // VIBMODE_U3
    if (!success) Serial.println("U3 Error");
  } else if (CurProf.vibrationEnabled == 3) {
    CurProf.vibrationEnabled = 4;
    success = dispenserHead.set_vibration_level(4);  // VIBMODE_U4
    if (!success) Serial.println("U4 Error");
  } else {
    CurProf.vibrationEnabled = 0;
    success = dispenserHead.set_vibration_level(0);  // VIBMODE_U0
    if (!success) Serial.println("U0 Error");
  }
}
/**********************************************************************************************************
* @brief vibration_time()
* @details Command for changing the vibration duration/time from 1 to 5 seconds.
* @param void
* @return void
**********************************************************************************************************/
void vibration_time() {
  bool success = false;

  // Cycle through vibration durations
  if (CurProf.vibrationDuration == 2) {
    CurProf.vibrationDuration = 3;
    success = dispenserHead.set_vibration_time(3);  // VIBDUR_3
  } else if (CurProf.vibrationDuration == 3) {
    CurProf.vibrationDuration = 4;
    success = dispenserHead.set_vibration_time(4);  // VIBDUR_4
  } else if (CurProf.vibrationDuration == 4) {
    CurProf.vibrationDuration = 5;
    success = dispenserHead.set_vibration_time(5);  // VIBDUR_5
  } else if (CurProf.vibrationDuration == 5) {
    CurProf.vibrationDuration = 1;
    success = dispenserHead.set_vibration_time(1);  // VIBDUR_1
  } else {
    CurProf.vibrationDuration = 2;
    success = dispenserHead.set_vibration_time(2);  // VIBDUR_2
  }

  if (!success) {
    Serial.println("Vib Time Error");
  }
}

bool Homing() {}

/*********************END OF FUNCTION DECLARATIONS*********************/

void setup() {
  int i;
  phost = &host;


  SpecialMode = FALSE;
  App_Common_Init(&host);  //* Init HW Hal */
  // App_Calibrate_Screen(&host); ///*Screen Calibration*//

  GPIO_Setup();

  Serial.begin(19200);   // Serial printing
  Serial2.begin(19200);  // UART for Arduino-PIC18 communications
  Serial3.begin(19200);  // UART for Arduino-PIC18 communications

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
  Dprint("Firmware version :", FWVER);

  Gpu_Hal_Wr8(phost, REG_TOUCH_SETTLE, 3);

  Serial.print("Setup Password[1]: ");
  Serial.println(Password[1]);
  Serial.print("Setup Password[2]: ");
  Serial.println(Password[2]);
  ReadPassEEPROM(Password[2]);
  Serial.print("ReadPassEEPROM Password[2]: ");
  Serial.println(Password[2]);

  if (strcmp(Password[1], Password[2]) == 0) {
    //20240226: erdiongson - Revised the password copying logic to Password[1]
    strcpy(Password[1], Password[2]);  //if eeprom is blank load preset value (locked to init1234)
    Serial.println("Initial Password used.");
  } else {
    //20240226: erdiongson - If the password changed, it will automatically save to Password[1]
    ReadPassEEPROM(Password[1]);  //if they changed the password other than init1234
    Serial.println("Password changed.");
  }
  Serial.print("New Password[1]: ");
  Serial.print(Password[1]);
  Serial.println();
  CurProfNum = LoadProfile();

  //Vibration Level Initialization
  // if (CurProf.vibrationEnabled == 0) {
  //   CurProf.vibrationEnabled = 4;
  // } else {
  //   CurProf.vibrationEnabled = CurProf.vibrationEnabled - 1;
  // }
  // vibration_on();

  //Vibration Duration Initialization
  // if (CurProf.vibrationDuration == 1) {
  //   CurProf.vibrationDuration = 5;
  // } else {
  //   CurProf.vibrationDuration = CurProf.vibrationDuration - 1;
  // }
  // vibration_time();

  // Home_Menu(&host, MAINMENU);
  // modeController.start_mode<MoveTestMode>(CurProf);
  // Move_Test_Screen(&host, {});
  modeController.start_mode<DispenseTestMode>(CurProf);
  Dispense_Test_Screen(&host, {});

  dispenserHead.z().setDisabled(true);
  dispenserHead.set_vibration_level(1);
  dispenserHead.set_vibration_time(1);
}

void loop() {
  dispenserHead.x().onStep();
  dispenserHead.y().onStep();
  dispenserHead.z().onStep();

  int result = modeController.on_step();
  
  // Only check limit switches and refresh screen if in MoveTestMode
  if (result == MODE_CONTINUE && modeController.getCurrentMode() != nullptr) {
    // Check if current mode is MoveTestMode
    if (modeController.getCurrentMode()->get_mode_type() == MODE_TYPE_MOVE_TEST) {
      // Check all limit switch states
      LimitSwitchStates limitStates = {
        dispenserHead.x().isAtMax(),    // x_max_limit
        dispenserHead.x().isAtMin(),    // x_min_limit
        dispenserHead.y().isAtMax(),    // y_max_limit
        dispenserHead.y().isAtMin(),    // y_min_limit
        dispenserHead.z().isAtMax(),    // z_max_limit
        dispenserHead.z().isAtMin()     // z_min_limit
      };
      
      // Refresh Move Test Screen with updated limit switch states
      Move_Test_Screen(&host, limitStates);
    }
  }

  if (result == MODE_COMPLETE) {
    Serial.println("MODE COMPLETE");
    Home_Menu(&host, MAINMENU);
  } else if (result != MODE_CONTINUE) {
    // Error
    Serial.println("MODE ERROR: " + String(result));
    Home_Menu(&host, MAINMENU);
  }

  // Handle PLC commands
  // PLCMessage message = getNewMessage();
  // if (message.type == MSG_START) {
  //   if (stateController.is_paused()) {
  //     stateController.resume();
  //   } else {
  //     stateController.start_dispensing(CurProf);
  //   }
  //   Home_Menu(&host, RUNMENU);
  // } else if (message.type == MSG_STOP) {
  //   stateController.stop();
  //   Home_Menu(&host, MAINMENU);
  // } else if (message.type == MSG_PAUSE) {
  //   stateController.pause();
  //   Home_Menu(&host, PAUSEMENU);
  // } else if (message.type == MSG_RAISE_Z) {
  //   stateController.get_dispenser_head()->z().moveBy(-STEPS_PER_UNIT_Z * 10);
  //   stateController.get_dispenser_head()->z().runUntilCompleteBlocking();
  // } else if (message.type == MSG_LOWER_Z) {
  //   stateController.get_dispenser_head()->z().moveBy(STEPS_PER_UNIT_Z * 10);
  //   stateController.get_dispenser_head()->z().runUntilCompleteBlocking();
  // }

  // Handle touch screen interactions
  switch (OpMode) {
    case (MANUAL_MODE):

      int touchButtonPressed = GetKeyPressed();
      if (touchButtonPressed == 0) return;  // Skip iteration if no interaction
      Serial.println("KeyPressed: " + String(touchButtonPressed));
      WaitKeyRelease();

      switch (touchButtonPressed) {
        case START:
          modeController.start_mode<DispenseMode>(CurProf);
          Home_Menu(&host, RUNMENU);
          break;

        case STOP:
          modeController.on_button_pressed(STOP);
          Home_Menu(&host, MAINMENU);
          break;

        case PAUSE:
          modeController.on_button_pressed(PAUSE);
          Home_Menu(&host, PAUSEMENU);
          break;

        case SETTING:
          Dprint("Enter Setting");
          if (digitalRead(Limit_S_y_MAX) == 0) SpecialMode = TRUE;
          else SpecialMode = FALSE;
          Password[2][0] = 0;
          WaitKeyRelease();
          if (CurProf.passwordEnabled) {
            Keyboard(phost, Password[2], "Enter Password", FALSE);
            if (strcmp(Password[1], Password[2]) == 0 || strcmp(Password[0], Password[2]) == 0) {
              if (SpecialMode) {
                if (digitalRead(Limit_S_y_MAX) != 0) {
                  BlankEEPROM();
                } else SpecialMode = FALSE;
              }
              DisplayConfig(phost);
              WaitKeyRelease();
              delay(100);
              Config_Settings(phost);
            } else {
              DisplayKeyboard(phost, 0, "Wrong Password", " ", FALSE, FALSE, FALSE);
              delay(2000);
            }
          } else {
            DisplayConfig(phost);
            WaitKeyRelease();
            delay(100);
            Config_Settings(phost);
          }
          Home_Menu(&host, MAINMENU);
          delay(100);
          break;
      }
      break;

    case (AUTO_MODE):
      break;
  }
}
