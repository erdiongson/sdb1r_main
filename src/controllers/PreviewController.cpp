#include "PreviewController.h"
#include "../views/PreviewScreen.h"
#include "../Constants.h"
#include "../../Config.h"
#include "../Utils.h"

PreviewController::PreviewController(ControllerParams params)
  : BaseController(params), currentProfile(nullptr), 
    simulating(false), lastSimulationTime(0), simulateCol(0), simulateRow(0) {}

void PreviewController::on_start(Profile& profile) {
  Dprint(F("PreviewController::on_start"));
  
  // Store reference to the current profile
  currentProfile = &profile;

  // Initialize simulation state
  simulating = false;
  simulateCol = 0;
  simulateRow = 0;
  lastSimulationTime = 0;
  
  // Load profile into simulation handler
  simulationHandler.load_profile(*currentProfile);
  simulationHandler.reset();

  // Parse skip positions from the current profile
  TrayHandler::Position skipPositions[MAX_POSITIONS];
  simulationHandler.getSkipPositions(skipPositions);
 
  // Create preview screen parameters
  PreviewScreenParams params;
  params.gridCols = currentProfile->Tube_No_x;
  params.gridRows = currentProfile->Tube_No_y;
  params.staggered = currentProfile->staggered;
  params.simulating = false;
  params.simulateCol = 0;
  params.simulateRow = 0;

  snprintf(previewInfoText, sizeof(previewInfoText), "Preview (Grid %dx%d)", params.gridCols, params.gridRows);
  params.infoText = previewInfoText;
  
  // Display the preview screen
  draw_preview_screen(phost, skipPositions, params);
}

void PreviewController::on_interaction(const Interaction& interaction) {
  switch (interaction.key_pressed) {
    case TAG_CONFIG_PREVIEW_BACK:
      Serial.println(F("Button Pressed: PREVIEW BACK"));
      // Stop simulation when leaving preview
      simulating = false;
      simulateCol = 0;
      simulateRow = 0;
      start_next_controller(CONTROLLER_SETTINGS);
      break;

    case TAG_PREVIEW_SIMULATE:
      Serial.println(F("Button Pressed: SIMULATE"));
      start_simulation();
      break;

    case TAG_PREVIEW_STOP:
      Serial.println(F("Button Pressed: STOP"));
      end_simulation();
      break;

    default:
      break;
  }
}

ControllerStepResult PreviewController::on_step() {
  // Handle simulation updates every 600 ms
  if (simulating) {
    unsigned long currentTime = millis();
    if (currentTime - lastSimulationTime >= 600) {
      lastSimulationTime = currentTime;
      step_simulation();
    }
  }
  
  DispenserProcessResult result = dispenserHead.process();
  return ControllerStepResult(result.steppers, result.dispenser);
}

int PreviewController::get_mode_type() const {
  return CONTROLLER_PREVIEW;
}

void PreviewController::start_simulation() {
  // Start simulation
  simulating = true;
  lastSimulationTime = millis();
  
  // Reset handler and get first position
  simulationHandler.load_profile(*currentProfile);
  TrayHandler::Position firstPosition = simulationHandler.reset();

  if (firstPosition.x == -1 || firstPosition.y == -1) {
    Serial.println(F("No valid positions found, ending simulation"));
    end_simulation();
    return;
  }

  simulateCol = firstPosition.x;
  simulateRow = firstPosition.y;
  
  // Redraw preview screen
  TrayHandler::Position skipPositions[MAX_POSITIONS];
  simulationHandler.getSkipPositions(skipPositions);
  
  PreviewScreenParams params;
  params.gridCols = currentProfile->Tube_No_x;
  params.gridRows = currentProfile->Tube_No_y;
  params.simulating = simulating;
  params.simulateCol = simulateCol;
  params.simulateRow = simulateRow;
  params.staggered = currentProfile->staggered;
  
  // Set info text for simulation
  snprintf(previewInfoText, sizeof(previewInfoText), "Position = %dx%d", simulateCol, simulateRow);
  params.infoText = previewInfoText;
  
  draw_preview_screen(phost, skipPositions, params);
}

void PreviewController::end_simulation() {
  // Stop simulation
  simulating = false;
  simulateCol = 0;
  simulateRow = 0;
  
  // Redraw preview screen
  TrayHandler::Position skipPositions[MAX_POSITIONS];
  simulationHandler.getSkipPositions(skipPositions);
  
  PreviewScreenParams params;
  params.gridCols = currentProfile->Tube_No_x;
  params.gridRows = currentProfile->Tube_No_y;
  params.simulating = simulating;
  params.simulateCol = simulateCol;
  params.simulateRow = simulateRow;
  params.staggered = currentProfile->staggered;
  
  // Set info text for non-simulation
  snprintf(previewInfoText, sizeof(previewInfoText), "Preview (Grid %dx%d)", params.gridCols, params.gridRows);
  params.infoText = previewInfoText;
  
  draw_preview_screen(phost, skipPositions, params);
}

void PreviewController::step_simulation() {
  // Get next position
  TrayHandler::PositionResult result = simulationHandler.goToNextValidPosition();
  
  if (result.hasNext) {
    simulateCol = result.position.x;
    simulateRow = result.position.y;
  } else {
    end_simulation();
    return;
  }
  
  // Redraw preview screen with updated position
  TrayHandler::Position skipPositions[MAX_POSITIONS];
  simulationHandler.getSkipPositions(skipPositions);
  
  PreviewScreenParams params;
  params.gridCols = currentProfile->Tube_No_x;
  params.gridRows = currentProfile->Tube_No_y;
  params.simulating = simulating;
  params.simulateCol = simulateCol;
  params.simulateRow = simulateRow;
  params.staggered = currentProfile->staggered;
  
  // Set info text for simulation
  snprintf(previewInfoText, sizeof(previewInfoText), "Position = %dx%d", simulateCol, simulateRow);
  params.infoText = previewInfoText;
  
  draw_preview_screen(phost, skipPositions, params);
}
