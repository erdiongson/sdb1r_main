#include "AdvancedSettingsController.h"
#include "../../views/settings/AdvancedSettingsScreen.h"
#include "../../views/common/Keyboards.h"
#include "../../logic/TrayPositionHandler.h"
#include "../../logic/SkipUtils.h"

AdvancedSettingsController::AdvancedSettingsController(ControllerParams params) 
    : BaseController(params), current_profile(nullptr) {}

void AdvancedSettingsController::onStart() {
  Logger::log(F("MODE: Advanced Settings"));

  // Store reference to the current profile
  current_profile = &profile_manager.getCurrentProfile();

  // Load current skip strings into member variables
  profile_manager.getSkipStrings(skipCol, skipRow, skipSinglePos);

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
      if (verification.is_valid) {
        // Save skip strings to profile and go back
        profile_manager.setSkipStrings(skipCol, skipRow, skipSinglePos);
        startNextController(CONTROLLER_SETTINGS);
        return;
      }

      if (verification.errors.skip_count_exceeded) {
        drawScreen(DIALOG_ERROR_SKIP_COUNT_EXCEEDED);
      } else {
        drawScreen(DIALOG_ERROR_PARAMETER);
      }
      break;
    }

    case TAG_CONFIG_PREVIEW: {
      Logger::log(F("Button Pressed: PREVIEW"));
      SkipVerificationResult verification = verifyParameters();
      if (verification.is_valid) {
        // Save skip strings to profile and go to preview
        profile_manager.setSkipStrings(skipCol, skipRow, skipSinglePos);
        startNextController(CONTROLLER_PREVIEW);
        return;
      }
      if (verification.errors.skip_count_exceeded) {
        drawScreen(DIALOG_ERROR_SKIP_COUNT_EXCEEDED);
      } else {
        drawScreen(DIALOG_ERROR_PARAMETER);
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

void AdvancedSettingsController::editSkipColumn(Gpu_Hal_Context_t* phost) {
  // Create dimensions from profile
  TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);

  // Load current skipCol into input buffer
  strncpy(skipInputBuffer, skipCol, SKIP_STRING_LEN - 1);
  skipInputBuffer[SKIP_STRING_LEN - 1] = '\0';

  const char* errorMsg = NULL;
  while (true) {
    KeyboardResult kbResult = getKeyboardValue(phost, skipInputBuffer, "Enter columns to skip", false, SKIP_STRING_LEN, errorMsg);

    // Check if user pressed back
    if (kbResult.action == ACTION_BACK) return;

    // Clean the input with bounds checking
    SkipUtils::CleanResult result = SkipUtils::clean(skipInputBuffer, SkipUtils::COLUMN, dimensions);

    // Check if there was an error
    if (result.error_message[0] != '\0') {
      errorMsg = result.error_message;
      Logger::log("Error Message: " + String(result.error_message));
      // Use the cleaned text (capitalized) even with error
      if (result.cleaned[0] != '\0') {
        strncpy(skipInputBuffer, result.cleaned, SKIP_STRING_LEN - 1);
        skipInputBuffer[SKIP_STRING_LEN - 1] = '\0';
      }
      continue;
    }

    strncpy(skipCol, result.cleaned, SKIP_STRING_LEN - 1);
    Logger::log("New skipCol: " + String(skipCol));
    drawScreen();
    break;
  }
}

void AdvancedSettingsController::editSkipRow(Gpu_Hal_Context_t* phost) {
  // Create dimensions from profile
  TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);

  // Load current skipRow into input buffer
  strncpy(skipInputBuffer, skipRow, SKIP_STRING_LEN - 1);
  skipInputBuffer[SKIP_STRING_LEN - 1] = '\0';

  const char* errorMsg = NULL;
  while (true) {
    KeyboardResult kbResult = getKeyboardValue(phost, skipInputBuffer, "Enter rows to skip", false, SKIP_STRING_LEN, errorMsg);

    // Check if user pressed back
    if (kbResult.action == ACTION_BACK) return;

    // Clean the input with bounds checking
    SkipUtils::CleanResult result = SkipUtils::clean(skipInputBuffer, SkipUtils::ROW, dimensions);

    // Check if there was an error
    if (result.error_message[0] != '\0') {
      errorMsg = result.error_message;
      Logger::log("Error: " + String(result.error_message));
      // Use the cleaned text (capitalized) even with error
      if (result.cleaned[0] != '\0') {
        strncpy(skipInputBuffer, result.cleaned, SKIP_STRING_LEN - 1);
        skipInputBuffer[SKIP_STRING_LEN - 1] = '\0';
      }
      continue;
    }

    strncpy(skipRow, result.cleaned, SKIP_STRING_LEN - 1);
    drawScreen();
    break;
  }
}

void AdvancedSettingsController::editSkipIndividual(Gpu_Hal_Context_t* phost) {
  // Create dimensions from profile
  TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);

  // Load current skipSinglePos into input buffer
  strncpy(skipInputBuffer, skipSinglePos, SKIP_STRING_LEN - 1);
  skipInputBuffer[SKIP_STRING_LEN - 1] = '\0';

  const char* errorMsg = NULL;
  while (true) {
    Logger::log("Length of skip_single_pos: " + String(strlen(skipInputBuffer)));
    KeyboardResult kbResult = getKeyboardValue(phost, skipInputBuffer, "Enter positions to skip", false, SKIP_STRING_INDIVIDUAL_LEN, errorMsg);

    // Check if user pressed back
    if (kbResult.action == ACTION_BACK) return;

    // Clean the input with bounds checking
    SkipUtils::CleanResult result =
        SkipUtils::clean(skipInputBuffer, SkipUtils::INDIVIDUAL, dimensions, current_profile->staggered);

    // Check if there was an error
    if (result.error_message[0] != '\0') {
      errorMsg = result.error_message;
      Logger::log("Error: " + String(result.error_message));
      // Use the cleaned text (capitalized) even with error
      if (result.cleaned[0] != '\0') {
        strncpy(skipInputBuffer, result.cleaned, SKIP_STRING_LEN - 1);
        skipInputBuffer[SKIP_STRING_LEN - 1] = '\0';
      }
      continue;
    }

    strncpy(skipSinglePos, result.cleaned, SKIP_STRING_LEN - 1);
    drawScreen();
    break;
  }
}

// Verifies skip positions and returns validation result with error flags.
// @return SkipVerificationResult containing validity status and specific error flags.
SkipVerificationResult AdvancedSettingsController::verifyParameters() {
  SkipVerificationResult result;
  result.is_valid = true;
  result.errors = SkipErrors();  // Initialize all to false

  // Convert skip strings to positions to get actual count
  // Allow +1 to be returned, to detect if the user is trying to enter more than MAX_SKIP_POSITIONS

  SkipPosition tempPositions[MAX_SKIP_POSITIONS + 1];
  uint8_t skipCount = SkipUtils::convertFromStrings(skipCol, skipRow, skipSinglePos, tempPositions, MAX_SKIP_POSITIONS + 1);

  // Check if skip count exceeds maximum
  Logger::log("skipCount from buffers: " + String(skipCount) + ", MAX_SKIP_POSITIONS: " + String(MAX_SKIP_POSITIONS));
  if (skipCount > MAX_SKIP_POSITIONS) {
    result.is_valid = false;
    result.errors.skip_count_exceeded = true;
  }

  // Check skip position validity
  if (skipCount > 0) {
    TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);
    
    for (uint8_t i = 0; i < skipCount && i < MAX_SKIP_POSITIONS; i++) {
      const SkipPosition& skipPos = tempPositions[i];
      TrayHandler::Position pos(skipPos.x, skipPos.y);
      
      // Check if position is valid for current tray configuration using SkipUtils
      if (!SkipUtils::isValidSkipPosition(pos, dimensions, current_profile->staggered)) {
        result.is_valid = false;
        
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
void AdvancedSettingsController::drawScreen(int dialog_code) {
  SkipVerificationResult verification = verifyParameters();
  drawAdvancedSettingsScreen(phost, { *current_profile, skipCol, skipRow, skipSinglePos, verification.errors, dialog_code });
}
