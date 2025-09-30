#pragma once

#include "BaseMode.h"
#include "../TrayPositionHandler.h"

/**
 * DispenseMode - Responsible for automated dispensing operation
 * 
 * This mode handles the full dispensing cycle:
 * - Moving to home position
 * - Moving to tray positions
 * - Lowering/raising the dispenser head
 * - Triggering dispensing
 * - Moving through all valid tray positions
 */
class DispenseMode : public BaseMode {
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
  /**
   * Constructor
   * 
   * @param head Reference to the dispenser head to control
   * @param host Pointer to GPU HAL context
   */
  DispenseMode(DispenserHead& head, Gpu_Hal_Context_t *host);

  /**
   * Called when the mode is started
   * 
   * @param profile The current profile settings
   */
  void on_start(Profile& profile) override;

  /**
   * Called when an interaction occurs
   * 
   * @param interaction The interaction that occurred
   */
  void on_interaction(const Interaction& interaction) override;

  /**
   * Called on each step of the mode
   * 
   * @return MODE_COMPLETE when dispensing is finished, MODE_CONTINUE otherwise
   */
  int on_step() override;
  
  /**
   * Returns the type of this mode
   * 
   * @return MODE_TYPE_DISPENSE
   */
  int get_mode_type() const override;

private:
  /**
   * Start a new dispensing stage
   * 
   * @param newStage The stage to start
   */
  void start_stage(int newStage);
};
