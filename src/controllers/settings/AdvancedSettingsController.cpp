#include "AdvancedSettingsController.h"
#include "../../views/settings/SettingsScreen.h"
#include "../../views/common/Keyboards.h"
#include "../../logic/TrayPositionHandler.h"
#include "../../logic/SkipUtils.h"

AdvancedSettingsController::AdvancedSettingsController(ControllerParams params) 
    : BaseController(params), current_profile(nullptr) {}

void AdvancedSettingsController::onStart() {
  Logger::log(F("MODE: Advanced Settings"));

  // Store reference to the current profile
  current_profile = &profile_manager.getCurrentProfile();

  // Display skip screen
  drawSkipScreen(phost, *current_profile);
}

void AdvancedSettingsController::onInteraction(const Interaction& interaction) {
  if (interaction.key_pressed == 0) return;

  switch (interaction.key_pressed) {
    case TAG_STAGGERED_TOGGLE:
      Logger::log(F("Toggle staggered mode"));
      current_profile->staggered = !current_profile->staggered;
      drawSkipScreen(phost, *current_profile);
      break;

    case TAG_SKIP_COLUMNS:
      Logger::log(F("Button Pressed: SKIP COLUMNS"));
      editSkipColumn(phost);
      drawSkipScreen(phost, *current_profile);
      break;

    case TAG_SKIP_ROWS:
      Logger::log(F("Button Pressed: SKIP ROWS"));
      editSkipRow(phost);
      drawSkipScreen(phost, *current_profile);
      break;

    case TAG_SKIP_SINGLE_POS:
      Logger::log(F("Button Pressed: SKIP SINGLE POSITION"));
      editSkipIndividual(phost);
      drawSkipScreen(phost, *current_profile);
      break;

    case TAG_ADV_PROF_BACK:  // Back button
      Logger::log(F("Button Pressed: BACK"));
      startNextController(CONTROLLER_SETTINGS);
      break;

    case TAG_CONFIG_PREVIEW:  // Preview button
      Logger::log(F("Button Pressed: PREVIEW"));
      startNextController(CONTROLLER_PREVIEW);
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

  // Get current skip strings
  char skipCol[SKIP_STRING_LEN], skipRow[SKIP_STRING_LEN], skipSinglePos[SKIP_STRING_LEN];
  profile_manager.getSkipStrings(skipCol, skipRow, skipSinglePos);

  const char* errorMsg = NULL;
  while (true) {
    KeyboardResult kbResult = getKeyboardValue(phost, skipCol, "Enter columns to skip", false, SKIP_STRING_LEN, errorMsg);

    // Check if user pressed back
    if (kbResult.action == ACTION_BACK) return;

    // Clean the input with bounds checking
    SkipUtils::CleanResult result = SkipUtils::clean(skipCol, SkipUtils::COLUMN, dimensions);

    // Check if there was an error
    if (result.error_message[0] != '\0') {
      errorMsg = result.error_message;
      Logger::log("Error: " + String(result.error_message));
      // Use the cleaned text (capitalized) even with error
      if (result.cleaned[0] != '\0') {
        Serial.print("Controller: Copying cleaned text to skipCol: '");
        Serial.print(result.cleaned);
        Serial.println("'");
        strncpy(skipCol, result.cleaned, SKIP_STRING_LEN - 1);
        skipCol[SKIP_STRING_LEN - 1] = '\0';
        Serial.print("Controller: skipCol after copy: '");
        Serial.print(skipCol);
        Serial.println("'");
      }
      continue;
    }

    strncpy(skipCol, result.cleaned, SKIP_STRING_LEN - 1);
    break;
  }

  // Save back to profile
  profile_manager.setSkipStrings(skipCol, skipRow, skipSinglePos);
}

void AdvancedSettingsController::editSkipRow(Gpu_Hal_Context_t* phost) {
  // Create dimensions from profile
  TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);

  // Get current skip strings
  char skipCol[SKIP_STRING_LEN], skipRow[SKIP_STRING_LEN], skipSinglePos[SKIP_STRING_LEN];
  profile_manager.getSkipStrings(skipCol, skipRow, skipSinglePos);

  const char* errorMsg = NULL;
  while (true) {
    KeyboardResult kbResult = getKeyboardValue(phost, skipRow, "Enter rows to skip", false, SKIP_STRING_LEN, errorMsg);

    // Check if user pressed back
    if (kbResult.action == ACTION_BACK) return;

    // Clean the input with bounds checking
    SkipUtils::CleanResult result = SkipUtils::clean(skipRow, SkipUtils::ROW, dimensions);

    // Check if there was an error
    if (result.error_message[0] != '\0') {
      errorMsg = result.error_message;
      Logger::log("Error: " + String(result.error_message));
      // Use the cleaned text (capitalized) even with error
      if (result.cleaned[0] != '\0') {
        strncpy(skipRow, result.cleaned, SKIP_STRING_LEN - 1);
        skipRow[SKIP_STRING_LEN - 1] = '\0';
      }
      continue;
    }

    strncpy(skipRow, result.cleaned, SKIP_STRING_LEN - 1);
    break;
  }

  // Save back to profile
  profile_manager.setSkipStrings(skipCol, skipRow, skipSinglePos);
}

void AdvancedSettingsController::editSkipIndividual(Gpu_Hal_Context_t* phost) {
  // Create dimensions from profile
  TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);

  // Get current skip strings
  char skipCol[SKIP_STRING_LEN], skipRow[SKIP_STRING_LEN], skipSinglePos[SKIP_STRING_LEN];
  profile_manager.getSkipStrings(skipCol, skipRow, skipSinglePos);

  const char* errorMsg = NULL;
  while (true) {
    Logger::log("Length of skip_single_pos: " + String(strlen(skipSinglePos)));
    KeyboardResult kbResult = getKeyboardValue(phost, skipSinglePos, "Enter positions to skip", false, SKIP_STRING_LEN, errorMsg);

    // Check if user pressed back
    if (kbResult.action == ACTION_BACK) return;

    // Clean the input with bounds checking
    Logger::log(F("Cleaning!"));
    SkipUtils::CleanResult result =
        SkipUtils::clean(skipSinglePos, SkipUtils::INDIVIDUAL, dimensions, current_profile->staggered);
    Logger::log(F("Cleaned!"));

    // Check if there was an error
    if (result.error_message[0] != '\0') {
      errorMsg = result.error_message;
      Logger::log("Error: " + String(result.error_message));
      // Use the cleaned text (capitalized) even with error
      if (result.cleaned[0] != '\0') {
        strncpy(skipSinglePos, result.cleaned, SKIP_STRING_LEN - 1);
        skipSinglePos[SKIP_STRING_LEN - 1] = '\0';
      }
      continue;
    }

    strncpy(skipSinglePos, result.cleaned, SKIP_STRING_LEN - 1);
    break;
  }

  // Save back to profile
  profile_manager.setSkipStrings(skipCol, skipRow, skipSinglePos);
}
