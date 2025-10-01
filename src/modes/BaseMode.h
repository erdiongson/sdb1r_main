#pragma once

#include "../hardware/DispenserHead.h"
#include "../../Profile.h"
#include "ModesCommon.h"
#include "../gpu/App_Common.h"
#include "../logic/InteractionsHandler.h"

// Forward declaration to avoid circular dependency
class ModeController;

// Callback function type for mode completion
typedef void (ModeController::*ModeCompletionCallback)(int);

class BaseMode {
public:
  DispenserHead& dispenserHead;
  Gpu_Hal_Context_t *phost;

protected:
  ModeController* modeController;
  ModeCompletionCallback completionCallback;

public:
  // Constructor that accepts a dispenser head reference, GPU HAL context, and callback
  BaseMode(DispenserHead& head, Gpu_Hal_Context_t *host, ModeController* controller, ModeCompletionCallback callback)
    : dispenserHead(head), phost(host), modeController(controller), completionCallback(callback) {}

  // Virtual destructor for proper cleanup in derived classes
  virtual ~BaseMode() = default;

  // Pure virtual methods that must be implemented by derived classes
  virtual void on_start(Profile& profile) = 0;
  virtual void on_interaction(const Interaction& interaction) = 0;
  virtual int on_step() = 0;  // Returns error code or MODE_COMPLETE
  
  // Virtual method to get mode type
  virtual int get_mode_type() const = 0;

protected:
  // Helper method for modes to complete and transition to next mode
  void complete_with_next_mode(int nextModeType) {
    if (modeController && completionCallback) {
      (modeController->*completionCallback)(nextModeType);
    }
  }
};

