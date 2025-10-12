#pragma once

#include "../hardware/DispenserHead.h"
#include "../logic/Profile.h"
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

// Struct for mode step result indicating if steppers moved.
struct ControllerStepResult {
  bool stepper_moved;

  ControllerStepResult(bool stepper_moved)
    : stepper_moved(stepper_moved) {}
};

// Struct for controller parameters.
struct ControllerParams {
  DispenserHead& head;
  Gpu_Hal_Context_t *host;
  ControllerCompletionCallback callback;
  ProfileManager& profile_manager;
};

class BaseController {
public:
  DispenserHead& dispenserHead;
  Gpu_Hal_Context_t *phost;
  ProfileManager& profile_manager;

protected:
  ControllerCompletionCallback completionCallback;

public:
  // Constructor that accepts a dispenser head reference, GPU HAL context, callback, and profile manager
  BaseController(ControllerParams params)
    : dispenserHead(params.head), phost(params.host), completionCallback(params.callback), profile_manager(params.profile_manager) {}

  // Virtual destructor for proper cleanup in derived classes
  virtual ~BaseController() = default;

  // Pure virtual methods that must be implemented by derived classes
  virtual void onStart() = 0;
  virtual void onInteraction(const Interaction& interaction) = 0;
  virtual ControllerStepResult onStep() = 0;  // Returns ControllerStepResult indicating if steppers moved
  
  // Virtual method to get mode type
  virtual int getModeType() const = 0;

protected:
  // Helper method for modes to complete and transition to next mode
  void startNextController(int nextModeType) {
    if (completionCallback) {
      completionCallback(nextModeType);
    }
  }
};

