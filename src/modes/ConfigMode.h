#pragma once

#include "BaseMode.h"
#include "../gpu/Platform.h"
#include "../logic/TrayPositionHandler.h"

// ConfigMode - Responsible for managing system settings.
class ConfigMode : public BaseMode {
public:
  // Constructor.
  // @param head Reference to the dispenser head to control.
  // @param host Pointer to GPU HAL context.
  // @param controller Pointer to the mode controller for callbacks.
  // @param callback Callback function for mode completion.
  ConfigMode(DispenserHead& head, Gpu_Hal_Context_t *host, ModeController* controller, ModeCompletionCallback callback);

  // Called when the mode is started.
  // @param profile The current profile settings.
  void on_start(Profile& profile) override;

  // Called when an interaction occurs.
  // @param interaction The interaction that occurred.
  void on_interaction(const Interaction& interaction) override;

  // Called on each step of the mode.
  // @return ModeStepResult with stepper and dispenser state.
  ModeStepResult on_step() override;
  
  // Returns the type of this mode.
  // @return MODE_TYPE_CONFIG.
  int get_mode_type() const override;

private:
  // Current profile reference
  Profile* currentProfile;
  
  // Flag to track if special mode is enabled
  bool specialMode;
  
  // Simulation state
  bool simulating;
  unsigned long lastSimulationTime;
  TrayHandler::TrayPositionHandler simulationHandler;
  int simulateCol;
  int simulateRow;
  char previewInfoText[50];  // Buffer for preview screen info text
  
  // Cycles through vibration levels from U0-U4.
  void increment_vibration_level();
  
  // Cycles through vibration durations from 1-5 seconds.
  void increment_vibration_time();
  
  // Edits skip column string, looping until input is clean.
  // @param phost Pointer to GPU HAL context.
  void editSkipColumn(Gpu_Hal_Context_t* phost);
  
  // Edits skip row string, looping until input is clean.
  // @param phost Pointer to GPU HAL context.
  void editSkipRow(Gpu_Hal_Context_t* phost);
  
  // Edits skip individual position string, looping until input is clean.
  // @param phost Pointer to GPU HAL context.
  void editSkipIndividual(Gpu_Hal_Context_t* phost);
  
  // Starts the simulation and displays the first position.
  void start_simulation();
  
  // Ends the simulation and resets state.
  void end_simulation();
  
  // Steps the simulation to the next position and redraws the preview.
  void step_simulation();
};
