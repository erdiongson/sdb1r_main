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
#include "src/logic/Profile.h"
#include <EEPROM.h>
#include "Config.h"
#include "src/Utils.h"

#include "src/hardware/Axis.h"
#include "src/hardware/DispenserHead.h"
#include "src/logic/ControllerManager.h"

Gpu_Hal_Context_t host, *phost;
ProfileManager profile_manager;

AxisParams axis_x(
  MOTOR_X_CLK, MOTOR_X_CW,
  LIMIT_S_X_MIN, LIMIT_S_X_MAX,
  MOTOR_X_SPEED, MOTOR_X_ACCELERATION);

AxisParams axis_y(
  MOTOR_Y_CLK, MOTOR_Y_CW,
  LIMIT_S_Y_MIN, LIMIT_S_Y_MAX,
  MOTOR_Y_SPEED, MOTOR_Y_ACCELERATION);

AxisParams axis_z(
  MOTOR_Z_CLK, MOTOR_z_CW,
  LIMIT_S_Z_MIN, LIMIT_S_Z_MAX,
  MOTOR_Y_SPEED, MOTOR_Y_ACCELERATION);

DispenserHeadParams params = { axis_x, axis_y, axis_z };
DispenserHead dispenser_head(params);

// Forward declaration of startNextController for callback
void startNextController(int nextControllerType);

// Controller manager instance
static uint8_t controller_manager_buffer[sizeof(ControllerManager)];
ControllerManager& controller_manager = *(new (controller_manager_buffer) ControllerManager(
  dispenser_head, &host, profile_manager, startNextController
));

// To track when to check for interactions
unsigned long last_interaction_check = 0;

// Transitions to the next controller based on the controller type.
// @param nextControllerType The type of controller to transition to.
void startNextController(int nextControllerType) {
  controller_manager.startNextController(nextControllerType);
}


void setup() {
  phost = &host;

  App_Common_Init(&host);  //* Init HW Hal */
  // App_Calibrate_Screen(&host); ///*Screen Calibration*//

  Serial.begin(19200);   // Serial printing
  Serial2.begin(19200);  // UART for Arduino-PIC18 communications
  Serial3.begin(19200);  // UART for PLC communication

  //20240906: erdiongson - Enable interrupt for UART 3 receive complete
  Logger::log("Setup Serial 3 as interrupt");
  UCSR3B |= (1 << RXCIE3);

  // Enable global interrupts
  Logger::log("Enable Global Interrupt");
  sei();

  // Gpu_Hal_Wr8(phost, REG_PWM_DUTY, 10); //brightness control

  Logger::log("Firmware version :", FWVER);
  Logger::log("");

  Gpu_Hal_Wr8(phost, REG_TOUCH_SETTLE, 3);

  Logger::log("Controller buffer size: ", String(ControllerManager::getMaxControllerSize()) + " bytes");
  
  Logger::log("Starting first controller..");
  startNextController(CONTROLLER_STARTUP);
}

Interaction interaction;

void loop() {
  unsigned long current_time = millis();

  // Call the mode's onStep() function
  // Responsible for stepper runs, and dispenser serial processing
  ControllerStepResult result = controller_manager.onStep();

  // Check for interactions only periodically
  // Includes touch screen presses, and PLC commands
  int check_interval = result.steppers == AXIS_STATE_RUNNING ? INTERACT_INTERVAL_AXIS_RUNNING : INTERACT_INTERVAL_AXIS_IDLE;

  if (current_time - last_interaction_check >= check_interval) {
    last_interaction_check = current_time;

    if (InteractionsHandler::getAllInteractions(interaction)) {
      controller_manager.onInteraction(interaction);
    }
  }

  // Print free memory every 10 seconds
  MemoryMonitor::printPeriodically(10000);
}
