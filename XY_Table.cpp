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

#include "src/gpu/Platform.h"
#include "src/gpu/App_Common.h"
#include <EEPROM.h>
#include "Config.h"
#include "Profile.h"
#include "XY_Table.h"

#include "src/hardware/Axis.h"
#include "src/hardware/DispenserHead.h"
#include "src/controllers/BaseController.h"
#include "src/controllers/HomeController.h"
#include "src/controllers/RunController.h"
#include "src/controllers/MoveTestController.h"
#include "src/controllers/DispenseTestController.h"
#include "src/controllers/ConfigController.h"

// Define placement new operator for Arduino (if not already available).
inline void* operator new(size_t size, void* ptr) { return ptr; }

Gpu_Hal_Context_t host, *phost;
Profile CurProf;        //current profile
Profile SelectProf;  //current profile

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
InteractionsHandler interactionsHandler;

// Calculate the maximum controller size at compile time.
constexpr size_t MAX_CONTROLLER_SIZE = MaxSize<
  HomeController,
  RunController,
  MoveTestController,
  DispenseTestController,
  ConfigController
>::value;

// Static buffer to hold any controller (aligned for proper object construction).
struct alignas(BaseController) ControllerBuffer {
  uint8_t data[MAX_CONTROLLER_SIZE];
} controllerBuffer;

// Pointer to the current controller being used.
BaseController* controller = nullptr;

// To track when to check for interactions
unsigned long lastInteractionCheck = 0;

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

// Transitions to the next controller based on the controller type.
// @param nextControllerType The type of controller to transition to.
void start_next_controller(int nextControllerType) {
  // Destroy the current controller if it exists (call destructor)
  if (controller != nullptr) {
    controller->~BaseController();
    controller = nullptr;
  }

  // Create controller parameters
  ControllerParams params = { dispenserHead, phost, start_next_controller };

  // Create the new controller in the static buffer using placement new
  switch(nextControllerType) {
    case CONTROLLER_HOME:
      controller = new (controllerBuffer.data) HomeController(params);
      break;
    case CONTROLLER_RUN:
      controller = new (controllerBuffer.data) RunController(params);
      break;
    case CONTROLLER_MOVE_TEST:
      controller = new (controllerBuffer.data) MoveTestController(params);
      break;
    case CONTROLLER_DISPENSE_TEST:
      controller = new (controllerBuffer.data) DispenseTestController(params);
      break;
    case CONTROLLER_CONFIG:
      controller = new (controllerBuffer.data) ConfigController(params);
      break;
    default:
      Serial.print("Unknown controller type: ");
      Serial.println(nextControllerType);
      return;
  }

  // Start the new controller
  if (controller != nullptr) {
    controller->on_start(CurProf);
  }
}


void setup() {
  phost = &host;

  SpecialMode = FALSE;
  App_Common_Init(&host);  //* Init HW Hal */
  // App_Calibrate_Screen(&host); ///*Screen Calibration*//

  Serial.begin(19200);   // Serial printing
  Serial2.begin(19200);  // UART for Arduino-PIC18 communications
  Serial3.begin(19200);  // UART for PLC communication

  //20240906: erdiongson - Enable interrupt for UART 3 receive complete
  Serial.print("Setup Serial 3 as interrupt");
  UCSR3B |= (1 << RXCIE3);

  // Enable global interrupts
  Serial.println("Enable Global Interrupt");
  sei();

  // Gpu_Hal_Wr8(phost, REG_PWM_DUTY, 10); //brightness control

  Logo_XQ_trans(&host);
  Dprint("Firmware version :", FWVER);

  Gpu_Hal_Wr8(phost, REG_TOUCH_SETTLE, 3);

  setupPasswordHandling();

  Serial.println("Loading profile..");
  CurProfNum = LoadProfile();

  Serial.print("Controller buffer size: ");
  Serial.print(MAX_CONTROLLER_SIZE);
  Serial.println(" bytes");
  
  Serial.println("Starting first controller..");
  start_next_controller(CONTROLLER_HOME);

  dispenserHead.z().setDisabled(true);
}

Interaction interaction;

void loop() {
  unsigned long currentTime = millis();

  // Call the mode's on_step() function
  // Responsible for stepper runs, and dispenser serial processing
  ControllerStepResult result = controller->on_step();

  // Check for interactions only periodically
  // Includes touch screen presses, and PLC commands
  int checkInterval = result.steppers == AXIS_STATE_RUNNING ? INTERACT_INTERVAL_AXIS_RUNNING : INTERACT_INTERVAL_AXIS_IDLE;

  if (currentTime - lastInteractionCheck >= checkInterval) {
    lastInteractionCheck = currentTime;

    if (interactionsHandler.getInteractionFast(interaction)) {
      controller->on_interaction(interaction);
    }
  }
}
