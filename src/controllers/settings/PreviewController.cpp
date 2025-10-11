#include "PreviewController.h"
#include "../../views/settings/PreviewScreen.h"
#include "../../Constants.h"
#include "../../../Config.h"
#include "../../Utils.h"

PreviewController::PreviewController(ControllerParams params)
  : BaseController(params), current_profile(nullptr), 
    simulating(false), last_simulation_time(0), simulate_col(0), simulate_row(0) {}

void PreviewController::onStart(Profile& profile) {
  Dprint(F("PreviewController::on_start"));
  
  // Store reference to the current profile
  current_profile = &profile;

  // Initialize simulation state
  simulating = false;
  simulate_col = 0;
  simulate_row = 0;
  last_simulation_time = 0;
  
  // Load profile into simulation handler
  simulation_handler.loadProfile(*current_profile);
  simulation_handler.reset();

  // Parse skip positions from the current profile
  TrayHandler::Position skipPositions[MAX_POSITIONS];
  simulation_handler.getSkipPositions(skipPositions);
 
  // Create preview screen parameters
  PreviewScreenParams params;
  params.grid_cols = current_profile->Tube_No_x;
  params.grid_rows = current_profile->Tube_No_y;
  params.staggered = current_profile->staggered;
  params.simulating = false;
  params.simulate_col = 0;
  params.simulate_row = 0;

  snprintf(preview_info_text, sizeof(preview_info_text), "Preview (Grid %dx%d)", params.grid_cols, params.grid_rows);
  params.info_text = preview_info_text;
  
  // Display the preview screen
  drawPreviewScreen(phost, skipPositions, params);
}

void PreviewController::onInteraction(const Interaction& interaction) {
  switch (interaction.key_pressed) {
    case TAG_CONFIG_PREVIEW_BACK:
      Serial.println(F("Button Pressed: PREVIEW BACK"));
      // Stop simulation when leaving preview
      simulating = false;
      simulate_col = 0;
      simulate_row = 0;
      startNextController(CONTROLLER_SETTINGS);
      break;

    case TAG_PREVIEW_SIMULATE:
      Serial.println(F("Button Pressed: SIMULATE"));
      startSimulation();
      break;

    case TAG_PREVIEW_STOP:
      Serial.println(F("Button Pressed: STOP"));
      endSimulation();
      break;
    default:
      break;
  }
}

ControllerStepResult PreviewController::onStep() {
  // Handle simulation updates every 600 ms
  if (simulating) {
    unsigned long currentTime = millis();
    if (currentTime - last_simulation_time >= 600) {
      last_simulation_time = currentTime;
      stepSimulation();
    }
  }
  
  DispenserProcessResult result = dispenserHead.process();
  return ControllerStepResult(result.steppers, result.dispenser);
}

int PreviewController::getModeType() const {
  return CONTROLLER_PREVIEW;
}

void PreviewController::startSimulation() {
  // Start simulation
  simulating = true;
  last_simulation_time = millis();
  
  // Reset handler and get first position
  simulation_handler.loadProfile(*current_profile);
  TrayHandler::Position firstPosition = simulation_handler.reset();

  if (firstPosition.x == -1 || firstPosition.y == -1) {
    Serial.println(F("No valid positions found, ending simulation"));
    endSimulation();
    return;
  }

  simulate_col = firstPosition.x;
  simulate_row = firstPosition.y;
  
  // Redraw preview screen
  TrayHandler::Position skipPositions[MAX_POSITIONS];
  simulation_handler.getSkipPositions(skipPositions);
  
  PreviewScreenParams params;
  params.grid_cols = current_profile->Tube_No_x;
  params.grid_rows = current_profile->Tube_No_y;
  params.simulating = simulating;
  params.simulate_col = simulate_col;
  params.simulate_row = simulate_row;
  params.staggered = current_profile->staggered;
  
  // Set info text for simulation
  snprintf(preview_info_text, sizeof(preview_info_text), "Position = %dx%d", simulate_col, simulate_row);
  params.info_text = preview_info_text;
  
  drawPreviewScreen(phost, skipPositions, params);
}

void PreviewController::endSimulation() {
  // Stop simulation
  simulating = false;
  simulate_col = 0;
  simulate_row = 0;
  
  // Redraw preview screen
  TrayHandler::Position skipPositions[MAX_POSITIONS];
  simulation_handler.getSkipPositions(skipPositions);
  
  PreviewScreenParams params;
  params.grid_cols = current_profile->Tube_No_x;
  params.grid_rows = current_profile->Tube_No_y;
  params.simulating = simulating;
  params.simulate_col = simulate_col;
  params.simulate_row = simulate_row;
  params.staggered = current_profile->staggered;
  
  // Set info text for non-simulation
  snprintf(preview_info_text, sizeof(preview_info_text), "Preview (Grid %dx%d)", params.grid_cols, params.grid_rows);
  params.info_text = preview_info_text;
  
  drawPreviewScreen(phost, skipPositions, params);
}

void PreviewController::stepSimulation() {
  // Get next position
  TrayHandler::PositionResult result = simulation_handler.goToNextValidPosition();
  
  if (result.has_next) {
    simulate_col = result.position.x;
    simulate_row = result.position.y;
  } else {
    endSimulation();
    return;
  }
  
  // Redraw preview screen with updated position
  TrayHandler::Position skipPositions[MAX_POSITIONS];
  simulation_handler.getSkipPositions(skipPositions);
  
  PreviewScreenParams params;
  params.grid_cols = current_profile->Tube_No_x;
  params.grid_rows = current_profile->Tube_No_y;
  params.simulating = simulating;
  params.simulate_col = simulate_col;
  params.simulate_row = simulate_row;
  params.staggered = current_profile->staggered;
  
  // Set info text for simulation
  snprintf(preview_info_text, sizeof(preview_info_text), "Position = %dx%d", simulate_col, simulate_row);
  params.info_text = preview_info_text;
  
  drawPreviewScreen(phost, skipPositions, params);
}
