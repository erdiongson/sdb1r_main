#include "AdvancedSettingsController.h"
#include "../../views/settings/AdvancedSettingsScreen.h"
#include "../../views/common/Keyboards.h"
#include "../../views/ViewCommon.h"
#include "../../logic/TrayPositionHandler.h"
#include "../../logic/SkipUtils.h"

AdvancedSettingsController::AdvancedSettingsController(ControllerParams params) 
    : BaseController(params), current_profile(nullptr) {}

void AdvancedSettingsController::onStart() {
  Logger::log(F("MODE: Advanced Settings"));

  // Store reference to the current profile
  current_profile = &profile_manager.getCurrentProfile();

  // Load current skip strings into member variables
  profile_manager.getSkipStrings(g_skip_col_buffer, SKIP_STRING_LEN, g_skip_row_buffer, SKIP_STRING_LEN, g_skip_single_buffer, SKIP_STRING_LEN);

  // Display advanced settings screen
  drawScreen();
}

void AdvancedSettingsController::onInteraction(const Interaction& interaction) {
  if (interaction.key_pressed == 0) return;

  switch (interaction.key_pressed) {
    case TAG_STAGGERED_TOGGLE:
      Logger::log(F("Toggle staggered mode"));
      current_profile->staggered = !current_profile->staggered;
      drawScreen();
      break;

    case TAG_SKIP_COLUMNS:
      Logger::log(F("Button Pressed: SKIP COLUMNS"));
      editSkipColumn(phost);
      drawScreen();
      break;

    case TAG_SKIP_ROWS:
      Logger::log(F("Button Pressed: SKIP ROWS"));
      editSkipRow(phost);
      drawScreen();
      break;

    case TAG_SKIP_SINGLE_POS:
      Logger::log(F("Button Pressed: SKIP SINGLE POSITION"));
      editSkipIndividual(phost);
      drawScreen();
      break;

    case TAG_ADV_PROF_BACK: {
      Logger::log(F("Button Pressed: BACK"));
      SkipVerificationResult verification = verifyParameters();

      if (verification.errors.skip_count_exceeded) {
        drawScreen(DIALOG_ERROR_SKIP_COUNT_EXCEEDED);
      } else {
        // Save skip strings to profile and go back
        profile_manager.setSkipStrings(g_skip_col_buffer, g_skip_row_buffer, g_skip_single_buffer);
        startNextController(CONTROLLER_SETTINGS);
        return;
      }
      break;
    }

    case TAG_CONFIG_PREVIEW: {
      Logger::log(F("Button Pressed: PREVIEW"));
      SkipVerificationResult verification = verifyParameters();

      if (verification.errors.skip_cells || verification.errors.skip_cols || verification.errors.skip_rows) {
        drawScreen(DIALOG_ERROR_PARAMETER);
      } else if (verification.errors.skip_count_exceeded) {
        drawScreen(DIALOG_ERROR_SKIP_COUNT_EXCEEDED);
      } else {
        // Save skip strings to profile and go back
        profile_manager.setSkipStrings(g_skip_col_buffer, g_skip_row_buffer, g_skip_single_buffer);
        startNextController(CONTROLLER_PREVIEW);
        return;
      }
      break;
    }

    case TAG_CONTINUE:
      Logger::log(F("Button Pressed: CONTINUE"));
      drawScreen();
      break;

    default:
      break;
  }
}

ControllerStepResult AdvancedSettingsController::onStep() {
  DispenserProcessResult result = dispenserHead.process();
  return ControllerStepResult(result.steppers == AXIS_STATE_RUNNING);
}

int AdvancedSettingsController::getModeType() const {
  return CONTROLLER_ADVANCED_SETTINGS;
}

// Base function for editing skip strings with validation.
// @param phost GPU context.
// @param source Buffer to read from and write cleaned result to.
// @param prompt PROGMEM string for keyboard prompt.
// @param skip_type Type of skip (COLUMN, ROW, or INDIVIDUAL).
void AdvancedSettingsController::editSkipBase(Gpu_Hal_Context_t* phost, char* source, const char* prompt, SkipUtils::SkipType skip_type) {
  // Create dimensions from profile
  TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);

  // Load current value into intermediate buffer
  snprintf(g_intermediate_buffer, g_intermediate_buffer_size, "%s", source);
  g_error_buffer[0] = '\0';

  while (true) {
    KeyboardResult kb_result = getKeyboardValue(phost, g_intermediate_buffer, prompt, false, g_intermediate_buffer_size, g_error_buffer);

    // Check if user pressed back
    if (kb_result.action == ACTION_BACK) return;

    // Clean the input with bounds checking
    SkipUtils::CleanResult result = SkipUtils::clean({
      g_intermediate_buffer,
      g_intermediate_buffer_size,
      g_error_buffer,
      g_error_buffer_size,
      skip_type,
      dimensions,
      current_profile->staggered
    });

    // Check if there was an error
    if (g_error_buffer[0] != '\0') {
      Logger::log(F("Error: %s"), g_error_buffer);
      continue;
    }

    strncpy(source, g_intermediate_buffer, SKIP_STRING_LEN - 1);
    drawScreen();
    break;
  }
}

void AdvancedSettingsController::editSkipColumn(Gpu_Hal_Context_t* phost) {
  editSkipBase(phost, g_skip_col_buffer, PROGMEM_STR(F("Enter columns to skip")), SkipUtils::COLUMN);
}

void AdvancedSettingsController::editSkipRow(Gpu_Hal_Context_t* phost) {
  editSkipBase(phost, g_skip_row_buffer, PROGMEM_STR(F("Enter rows to skip")), SkipUtils::ROW);
}

void AdvancedSettingsController::editSkipIndividual(Gpu_Hal_Context_t* phost) {
  editSkipBase(phost, g_skip_single_buffer, PROGMEM_STR(F("Enter positions to skip")), SkipUtils::INDIVIDUAL);
}

// Verifies skip positions and returns validation result with error flags.
// @return SkipVerificationResult containing validity status and specific error flags.
SkipVerificationResult AdvancedSettingsController::verifyParameters() {
  SkipVerificationResult result;
  result.errors = SkipErrors();  // Initialize all to false

  // Convert skip strings to positions to get actual count
  // Return value is the "true" value, but buffer is truncated to MAX_SKIP_POSITIONS_TOTAL
  uint8_t skip_count = SkipUtils::convertFromStrings(g_skip_col_buffer, g_skip_row_buffer, g_skip_single_buffer, g_intermediate_skip_positions, MAX_SKIP_POSITIONS_TOTAL);

  // Check if skip count exceeds maximum
  if (skip_count > MAX_SKIP_POSITIONS_TOTAL) {
    result.errors.skip_count_exceeded = true;
  }

  // Check skip position validity
  if (skip_count > 0) {
    TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);
    
    for (uint8_t i = 0; i < skip_count && i < MAX_SKIP_POSITIONS_TOTAL; i++) {
      const SkipPosition& skip_pos = g_intermediate_skip_positions[i];
      TrayHandler::Position pos(skip_pos.x, skip_pos.y);
      
      // Check if position is valid for current tray configuration using SkipUtils
      if (!SkipUtils::isValidSkipPosition(pos, dimensions, current_profile->staggered)) {
        
        // Determine which type of skip is invalid
        if (pos.y == 0 && pos.x != 0) {
          // Column skip
          result.errors.skip_cols = true;
        } else if (pos.x == 0 && pos.y != 0) {
          // Row skip
          result.errors.skip_rows = true;
        } else if (pos.x != 0 && pos.y != 0) {
          // Individual position skip
          result.errors.skip_cells = true;
        }
      }
    }
  }

  return result;
}

// Helper method to draw the advanced settings screen with current profile and errors.
void AdvancedSettingsController::drawScreen(uint8_t dialog_code) {
  // Verify parameters to highlight errors when drawing the screen
  SkipVerificationResult verification = verifyParameters();
  drawAdvancedSettingsScreen(phost, { *current_profile, g_skip_col_buffer, g_skip_row_buffer, g_skip_single_buffer, verification.errors, dialog_code });
}
