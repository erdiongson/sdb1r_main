#pragma once

#include "../controllers/BaseController.h"
#include "../controllers/ReadyController.h"
#include "../controllers/RunController.h"
#include "../controllers/StartupController.h"
#include "../controllers/HomingController.h"
#include "../controllers/debug/DebugController.h"
#include "../controllers/debug/MoveTestController.h"
#include "../controllers/debug/DispenseTestController.h"
#include "../controllers/debug/StepperTestController.h"
#include "../controllers/settings/PreviewController.h"
#include "../controllers/settings/SettingsController.h"
#include "../controllers/settings/ProfileController.h"
#include "../controllers/settings/AdvancedSettingsController.h"
#include "../hardware/DispenserHead.h"
#include "../logic/Profile.h"
#include "../gpu/App_Common.h"
#include "../Constants.h"

// Define placement new operator for Arduino (if not already available).
inline void* operator new(size_t size, void* ptr) {
  return ptr;
}

// Manages controller lifecycle and transitions between different controllers.
class ControllerManager {
 private:
  // Calculate the maximum controller size at compile time.
  static constexpr size_t MAX_CONTROLLER_SIZE =
      MaxSize<ReadyController, RunController, MoveTestController, DispenseTestController, SettingsController,
              ProfileController, StartupController, DebugController, PreviewController,
              AdvancedSettingsController>::value;

  // Static buffer to hold any controller (aligned for proper object construction).
  struct alignas(BaseController) ControllerBuffer {
    uint8_t data[MAX_CONTROLLER_SIZE];
  } controllerBuffer;

  // Pointer to the current controller being used.
  BaseController* controller;

  // References to dependencies
  DispenserHead& dispenserHead;
  Gpu_Hal_Context_t* phost;
  ProfileManager& profile_manager;
  ControllerCompletionCallback callback;

 public:
  // Constructor that initializes the controller manager with required dependencies.
  // @param dispenserHead Reference to the dispenser head hardware.
  // @param phost Pointer to the GPU HAL context.
  // @param profile_manager Reference to the profile manager.
  // @param callback Callback function to handle controller transitions.
  ControllerManager(DispenserHead& dispenserHead, Gpu_Hal_Context_t* phost, ProfileManager& profile_manager,
                    ControllerCompletionCallback callback)
      : controller(nullptr),
        dispenserHead(dispenserHead),
        phost(phost),
        profile_manager(profile_manager),
        callback(callback) {}

  // Destructor that cleans up the current controller.
  ~ControllerManager() {
    if (controller != nullptr) {
      controller->~BaseController();
      controller = nullptr;
    }
  }

  // Transitions to the next controller based on the controller type.
  // @param next_controller_type The type of controller to transition to.
  void startNextController(int next_controller_type) {
    // Destroy the current controller if it exists (call destructor)
    if (controller != nullptr) {
      controller->~BaseController();
      controller = nullptr;
    }

    // Create controller parameters
    ControllerParams params = { dispenserHead, phost, callback, profile_manager };

    // Create the new controller in the static buffer using placement new
    switch (next_controller_type) {
      case CONTROLLER_READY:
        controller = new (controllerBuffer.data) ReadyController(params);
        break;
      case CONTROLLER_HOMING:
        controller = new (controllerBuffer.data) HomingController(params);
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
      case CONTROLLER_SETTINGS:
        controller = new (controllerBuffer.data) SettingsController(params);
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
      case CONTROLLER_PREVIEW:
        controller = new (controllerBuffer.data) PreviewController(params);
        break;
      case CONTROLLER_ADVANCED_SETTINGS:
        controller = new (controllerBuffer.data) AdvancedSettingsController(params);
        break;
      case CONTROLLER_STEPPER_TEST:
        controller = new (controllerBuffer.data) StepperTestController(params);
        break;
      default:
        Logger::log("Unknown controller type: ", (uint8_t)next_controller_type);
        return;
    }

    // Start the new controller
    if (controller != nullptr) {
      controller->onStart();
    }
  }

  // Executes the current controller's step function.
  // @return The controller step result, or a default result if no controller is active.
  ControllerStepResult onStep() {
    if (controller != nullptr) {
      return controller->onStep();
    }
    return ControllerStepResult(false);
  }

  // Handles an interaction event by delegating to the current controller.
  // @param interaction The interaction event to process.
  void onInteraction(const Interaction& interaction) {
    if (controller != nullptr) {
      controller->onInteraction(interaction);
    }
  }

  // Gets the current active controller.
  // @return Pointer to the current controller, or nullptr if no controller is active.
  BaseController* getCurrentController() { return controller; }

  // Gets the maximum controller buffer size.
  // @return The size in bytes of the largest controller.
  static constexpr size_t getMaxControllerSize() { return MAX_CONTROLLER_SIZE; }
};
