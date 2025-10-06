#pragma once

#include "../hardware/DispenserHead.h"
#include "../../Profile.h"
#include "../gpu/App_Common.h"
#include "../logic/InteractionsHandler.h"

// Template to determine the maximum size needed for any set of types.
// @tparam T The first type to check.
// @tparam Rest The remaining types to check.
template<typename T, typename... Rest>
struct MaxSize {
  static constexpr size_t value = sizeof(T) > MaxSize<Rest...>::value ? sizeof(T) : MaxSize<Rest...>::value;
};

// Base case for MaxSize template recursion.
// @tparam T The single type to check.
template<typename T>
struct MaxSize<T> {
  static constexpr size_t value = sizeof(T);
};

// Callback type for controller completion.
// @param nextModeType The mode type to transition to.
typedef void (*ControllerCompletionCallback)(int nextModeType);

// Struct for mode step result containing state and error information.
struct ControllerStepResult {
  int steppers;
  int dispenser;

  ControllerStepResult(int steppers, int dispenser)
    : steppers(steppers), dispenser(dispenser) {}
};

// Struct for controller parameters.
struct ControllerParams {
  DispenserHead& head;
  Gpu_Hal_Context_t *host;
  ControllerCompletionCallback callback;
};

class BaseController {
public:
  DispenserHead& dispenserHead;
  Gpu_Hal_Context_t *phost;

protected:
  ControllerCompletionCallback completionCallback;

public:
  // Constructor that accepts a dispenser head reference, GPU HAL context, and callback
  BaseController(ControllerParams params)
    : dispenserHead(params.head), phost(params.host), completionCallback(params.callback) {}

  // Virtual destructor for proper cleanup in derived classes
  virtual ~BaseController() = default;

  // Pure virtual methods that must be implemented by derived classes
  virtual void on_start(Profile& profile) = 0;
  virtual void on_interaction(const Interaction& interaction) = 0;
  virtual ControllerStepResult on_step() = 0;  // Returns ControllerStepResult with stepper and dispenser state
  
  // Virtual method to get mode type
  virtual int get_mode_type() const = 0;

protected:
  // Helper method for modes to complete and transition to next mode
  void start_next_controller(int nextModeType) {
    if (completionCallback) {
      completionCallback(nextModeType);
    }
  }
};

