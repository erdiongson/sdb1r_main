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
#include "src/Utils.h"

#include "src/hardware/Axis.h"
#include "src/hardware/DispenserHead.h"
#include "src/controllers/BaseController.h"
#include "src/controllers/HomeController.h"
#include "src/controllers/RunController.h"
#include "src/controllers/MoveTestController.h"
#include "src/controllers/DispenseTestController.h"
#include "src/controllers/ConfigController.h"
#include "src/controllers/ProfileController.h"
#include "src/controllers/StartupController.h"
#include "src/controllers/DebugController.h"

// Define placement new operator for Arduino (if not already available).
inline void* operator new(size_t size, void* ptr) { return ptr; }

Gpu_Hal_Context_t host, *phost;
Profile CurProf;        //current profile

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

// Calculate the maximum controller size at compile time.
constexpr size_t MAX_CONTROLLER_SIZE = MaxSize<
  HomeController,
  RunController,
  MoveTestController,
  DispenseTestController,
  ConfigController,
  ProfileController,
  StartupController,
  DebugController
>::value;

// Static buffer to hold any controller (aligned for proper object construction).
struct alignas(BaseController) ControllerBuffer {
  uint8_t data[MAX_CONTROLLER_SIZE];
} controllerBuffer;

// Pointer to the current controller being used.
BaseController* controller = nullptr;

// To track when to check for interactions
unsigned long lastInteractionCheck = 0;

// To track when to print memory stats
unsigned long lastMemoryPrint = 0;

// Prints the free memory available on the Arduino.
void printFreeMemory() {
  extern int __heap_start, *__brkval;
  int v;
  int freeMemory = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  
  Serial.print("Free memory: ");
  Serial.print(freeMemory);
  Serial.print(" bytes | Stack: 0x");
  Serial.print((int)&v, HEX);
  Serial.print(" | Heap: 0x");
  Serial.print(__brkval == 0 ? (int)&__heap_start : (int)__brkval, HEX);
  Serial.print(" | Uptime: ");
  Serial.print(millis() / 1000);
  Serial.println("s");
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
    case CONTROLLER_PROFILE:
      controller = new (controllerBuffer.data) ProfileController(params);
      break;
    case CONTROLLER_STARTUP:
      controller = new (controllerBuffer.data) StartupController(params);
      break;
    case CONTROLLER_DEBUG:
      controller = new (controllerBuffer.data) DebugController(params);
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

  Dprint("Firmware version :", FWVER);
  Serial.println("");

  Gpu_Hal_Wr8(phost, REG_TOUCH_SETTLE, 3);

  Serial.println("Loading profile..");
  CurProfNum = LoadProfile();

  Serial.print("Controller buffer size: ");
  Serial.print(MAX_CONTROLLER_SIZE);
  Serial.println(" bytes");
  
  Serial.println("Starting first controller..");
  start_next_controller(CONTROLLER_STARTUP);
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

    if (InteractionsHandler::getAllInteractions(interaction)) {
      controller->on_interaction(interaction);
    }
  }

  // Print free memory every 10 seconds
  if (currentTime - lastMemoryPrint >= 10000) {
    lastMemoryPrint = currentTime;
    printFreeMemory();
  }
}
