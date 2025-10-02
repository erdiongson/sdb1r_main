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

#include "src/modes/ModesCommon.h"
#include "src/gpu/Platform.h"
#include "src/gpu/App_Common.h"
#include <EEPROM.h>
#include "Config.h"
#include "Profile.h"
#include "XY_Table.h"

#include "src/hardware/Axis.h"
#include "src/hardware/DispenserHead.h"
#include "src/logic/ModeController.h"


Gpu_Hal_Context_t host, *phost;
Profile CurProf;        //current profile
ProfileMin SelectProf;  //current profile

uint8_t CurProfNum;  //current profile id

AxisParams xAxis(
  Motor_x_CLK, Motor_x_CW,
  Limit_S_x_MIN, Limit_S_x_MAX,
  motor_x_speed, motor_x_Acceleration);

AxisParams yAxis(
  Motor_y_CLK, Motor_y_CW,
  Limit_S_y_MIN, Limit_S_y_MAX,
  motor_y_speed, motor_y_Acceleration);

AxisParams zAxis(
  Motor_z_CLK, Motor_z_CW,
  Limit_S_z_MIN, Limit_S_z_MAX,
  motor_y_speed, motor_y_Acceleration);

DispenserHeadParams params = { xAxis, yAxis, zAxis };
DispenserHead dispenserHead(params);
ModeController modeController(dispenserHead);
InteractionsHandler interactionsHandler;

uint16_t err_flag = 0;  //E1 = 1, E2 = 2;
static uint32_t loopIndex = 0;

// Frequency tracking variables
unsigned long xAxisStepCount = 0;
unsigned long modeControllerStepCount = 0;
unsigned long lastFrequencyReport = 0;
const unsigned long FREQUENCY_REPORT_INTERVAL = 10000; // Report every 10 seconds (in milliseconds)

// Timing variables for loop operations
unsigned long lastInteractionCheck = 0;
unsigned long lastModeStep = 0;
const unsigned long INTERACTION_CHECK_INTERVAL = 100; // Check interactions every 1000ms
const unsigned long MODE_STEP_INTERVAL = 3000; // Run mode step every 5000ms (5s)

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

/**
 * Sets up and handles the password validation and initialization
 * Checks if the password in EEPROM matches the default, and handles accordingly
 */
void setupPasswordHandling() {
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
}


void setup() {
  phost = &host;

  SpecialMode = FALSE;
  App_Common_Init(&host);  //* Init HW Hal */
  // App_Calibrate_Screen(&host); ///*Screen Calibration*//

  Serial.begin(19200);   // Serial printing
  Serial2.begin(19200);  // UART for Arduino-PIC18 communications
  Serial3.begin(19200);  // UART for Arduino-PIC18 communications

  //20240906: erdiongson - Enable interrupt for UART 3 receive complete
  Serial.print("Setup Serial 3 as interrupt");
  UCSR3B |= (1 << RXCIE3);

  // Enable global interrupts
  Serial.print("Enable Global Interrupt");
  sei();

  // Gpu_Hal_Wr8(phost, REG_PWM_DUTY, 10);//brightness control

  Logo_XQ_trans(&host);
  Dprint("Firmware version :", FWVER);

  Gpu_Hal_Wr8(phost, REG_TOUCH_SETTLE, 3);

  setupPasswordHandling();

  CurProfNum = LoadProfile();

  modeController.start_mode(MODE_TYPE_HOME, CurProf, phost);

  dispenserHead.z().setDisabled(true);
  // dispenserHead.set_vibration_level(1);
  // dispenserHead.set_vibration_time(1);
  
  // Initialize frequency tracking
  lastFrequencyReport = millis();
}

void reportFrequencies() {
  unsigned long currentTime = millis();
  unsigned long timeDiff = currentTime - lastFrequencyReport;
  
  if (timeDiff >= FREQUENCY_REPORT_INTERVAL) {
    float timeInSeconds = timeDiff / 1000.0;
    
    Serial.println("=== Frequency Report ===");
    Serial.println("X-Axis onStep frequency: " + String(xAxisStepCount / timeInSeconds, 2) + " Hz");
    Serial.println("ModeController on_step frequency: " + String(modeControllerStepCount / timeInSeconds, 2) + " Hz");
    Serial.println("Time period: " + String(timeInSeconds, 2) + " seconds");
    Serial.println("========================");
    
    // Reset counters and timestamp
    xAxisStepCount = 0;
    modeControllerStepCount = 0;
    lastFrequencyReport = currentTime;
  }
}

Interaction interaction;

void loop() {
  // Track dispenser head onStep calls - run as frequently as possible
  dispenserHead.x().onStep();
  dispenserHead.y().onStep();
  // dispenserHead.z().onStep();
  xAxisStepCount++;
  
  
  unsigned long currentTime = millis();
  
  // Check for interactions every 1000ms
  if (currentTime - lastInteractionCheck >= INTERACTION_CHECK_INTERVAL) {
    if (interactionsHandler.getInteractionFast(interaction)) {
      modeController.on_interaction(interaction);
    }
    lastInteractionCheck = currentTime;
  }

  // // Run mode step every 5s
  if (currentTime - lastModeStep >= MODE_STEP_INTERVAL) {
    // Serial.println("MODE: on_step");
    
    // Track modeController on_step calls
    int result = modeController.on_step();
    
    if (result != 0) {
      Serial.println("Result: " + String(result));
    }
    if (result == MODE_COMPLETE) {
      modeController.start_mode(MODE_TYPE_HOME, CurProf, phost);
    }
    lastModeStep = currentTime;
  }

  
  // Report frequencies periodically
  // reportFrequencies();
}
