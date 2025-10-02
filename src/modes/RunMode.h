#pragma once

#include "BaseMode.h"
#include "../logic/TrayPositionHandler.h"

// RunMode - Responsible for automated dispensing operation.
class RunMode : public BaseMode {
private:
  bool paused = false;

  enum Stage {
    IDLE_STAGE,
    ZERO_STAGE,
    OFFSET_STAGE,
    LOWER_HEAD_STAGE,
    START_DISPENSE_STAGE,
    WAIT_DISPENSE_STAGE,
    RAISE_HEAD_STAGE,
    MOVE_STAGE,
    HOME_STAGE
  };

  int stage = IDLE_STAGE;

  Profile profile;
  TrayHandler::TrayPositionHandler trayHandler;

  int target_x = 0;
  int target_y = 0;

public:
  // Constructor.
  // @param head Reference to the dispenser head to control.
  // @param host Pointer to GPU HAL context.
  // @param controller Pointer to the mode controller for callbacks.
  // @param callback Callback function for mode completion.
  RunMode(DispenserHead& head, Gpu_Hal_Context_t *host, ModeController* controller, ModeCompletionCallback callback);

  // Called when the mode is started.
  // @param profile The current profile settings.
  void on_start(Profile& profile) override;

  // Called when an interaction occurs.
  // @param interaction The interaction that occurred.
  void on_interaction(const Interaction& interaction) override;

  // Called on each step of the mode.
  // @return MODE_COMPLETE when dispensing is finished, MODE_CONTINUE otherwise.
  int on_step() override;
  
  // Returns the type of this mode.
  // @return MODE_TYPE_RUN.
  int get_mode_type() const override;

private:
  // Start a new dispensing stage.
  // @param newStage The stage to start.
  void start_stage(int newStage);
};
