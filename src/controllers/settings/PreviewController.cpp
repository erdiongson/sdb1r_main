#include "PreviewController.h"
#include "../../views/settings/PreviewScreen.h"

PreviewController::PreviewController(ControllerParams params)
    : BaseController(params),
      current_profile(nullptr),
      simulating(false),
      last_simulation_time(0),
      simulate_col(0),
      simulate_row(0) {}

void PreviewController::onStart() {
  Logger::log(F("PreviewController::on_start"));

  // Store reference to the current profile
  current_profile = &profile_manager.getCurrentProfile();

  // Initialize simulation state
  simulating = false;
  simulate_col = 0;
  simulate_row = 0;
  last_simulation_time = 0;

  // Load profile into simulation handler
  simulation_handler.loadProfile(*current_profile);
  simulation_handler.reset();

  // Parse skip positions from the current profile
  TrayHandler::Position skip_positions[MAX_SKIP_POSITIONS_TOTAL];
  simulation_handler.getSkipPositions(skip_positions);

  // Create preview screen parameters
  PreviewScreenParams params;
  params.grid_cols = current_profile->tube_no_x;
  params.grid_rows = current_profile->tube_no_y;
  params.staggered = current_profile->staggered;
  params.simulating = false;
  params.simulate_col = 0;
  params.simulate_row = 0;

  snprintf(preview_info_text, sizeof(preview_info_text), "Preview (Grid %dx%d)", params.grid_cols, params.grid_rows);
  params.info_text = preview_info_text;

  // Display the preview screen
  drawPreviewScreen(phost, skip_positions, params);
}

void PreviewController::onInteraction(const Interaction& interaction) {
  switch (interaction.key_pressed) {
    case TAG_CONFIG_PREVIEW_BACK:
      Logger::log(F("Button Pressed: PREVIEW BACK"));
      // Stop simulation when leaving preview
      simulating = false;
      simulate_col = 0;
      simulate_row = 0;
      startNextController(CONTROLLER_ADVANCED_SETTINGS);
      break;

    case TAG_PREVIEW_SIMULATE:
      Logger::log(F("Button Pressed: SIMULATE"));
      startSimulation();
      break;

    case TAG_PREVIEW_STOP:
      Logger::log(F("Button Pressed: STOP"));
      endSimulation();
      break;
    default:
      break;
  }
}

ControllerStepResult PreviewController::onStep() {
  // Handle simulation updates every SIMULATION_UPDATE_INTERVAL_MS
  if (simulating) {
    unsigned long current_time = millis();
    if (current_time - last_simulation_time >= SIMULATION_UPDATE_INTERVAL_MS) {
      last_simulation_time = current_time;
      stepSimulation();
    }
  }

  DispenserProcessResult result = dispenserHead.process();
  return ControllerStepResult(result.steppers == AXIS_STATE_RUNNING);
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
  TrayHandler::Position first_position = simulation_handler.reset();

  if (first_position.x == -1 || first_position.y == -1) {
    Logger::log(F("No valid positions found, ending simulation"));
    endSimulation();
    return;
  }

  simulate_col = first_position.x;
  simulate_row = first_position.y;

  // Redraw preview screen
  TrayHandler::Position skip_positions[MAX_SKIP_POSITIONS_TOTAL];
  simulation_handler.getSkipPositions(skip_positions);

  PreviewScreenParams params;
  params.grid_cols = current_profile->tube_no_x;
  params.grid_rows = current_profile->tube_no_y;
  params.simulating = simulating;
  params.simulate_col = simulate_col;
  params.simulate_row = simulate_row;
  params.staggered = current_profile->staggered;

  // Set info text for simulation
  snprintf(preview_info_text, sizeof(preview_info_text), "Position = %dx%d", simulate_col, simulate_row);
  params.info_text = preview_info_text;

  drawPreviewScreen(phost, skip_positions, params);
}

void PreviewController::endSimulation() {
  // Stop simulation
  simulating = false;
  simulate_col = 0;
  simulate_row = 0;

  // Redraw preview screen
  TrayHandler::Position skip_positions[MAX_SKIP_POSITIONS_TOTAL];
  simulation_handler.getSkipPositions(skip_positions);

  PreviewScreenParams params;
  params.grid_cols = current_profile->tube_no_x;
  params.grid_rows = current_profile->tube_no_y;
  params.simulating = simulating;
  params.simulate_col = simulate_col;
  params.simulate_row = simulate_row;
  params.staggered = current_profile->staggered;

  // Set info text for non-simulation
  snprintf(preview_info_text, sizeof(preview_info_text), "Preview (Grid %dx%d)", params.grid_cols, params.grid_rows);
  params.info_text = preview_info_text;

  drawPreviewScreen(phost, skip_positions, params);
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
  TrayHandler::Position skip_positions[MAX_SKIP_POSITIONS_TOTAL];
  simulation_handler.getSkipPositions(skip_positions);

  PreviewScreenParams params;
  params.grid_cols = current_profile->tube_no_x;
  params.grid_rows = current_profile->tube_no_y;
  params.simulating = simulating;
  params.simulate_col = simulate_col;
  params.simulate_row = simulate_row;
  params.staggered = current_profile->staggered;

  // Set info text for simulation
  snprintf(preview_info_text, sizeof(preview_info_text), "Position = %dx%d", simulate_col, simulate_row);
  params.info_text = preview_info_text;

  drawPreviewScreen(phost, skip_positions, params);
}
