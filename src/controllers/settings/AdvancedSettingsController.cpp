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

  while (true) {
    getKeyboardValue(phost, current_profile->skip_col, "Enter columns to skip", false);

    // Clean the input with bounds checking
    SkipUtils::CleanResult result = SkipUtils::clean(current_profile->skip_col, SkipUtils::COLUMN, dimensions);

    // If input was cleaned, update and loop again
    if (result.was_cleaned) {
      strncpy(current_profile->skip_col, result.cleaned, SKIP_STRING_LEN - 1);
      current_profile->skip_col[SKIP_STRING_LEN - 1] = '\0';
      Logger::log(F("Input was cleaned, showing keyboard again"));
    } else {
      // Input is clean, exit loop
      Logger::log(F("Input is clean"));
      break;
    }
  }
}

void AdvancedSettingsController::editSkipRow(Gpu_Hal_Context_t* phost) {
  // Create dimensions from profile
  TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);

  while (true) {
    getKeyboardValue(phost, current_profile->skip_row, "Enter rows to skip", false);

    // Clean the input with bounds checking
    SkipUtils::CleanResult result = SkipUtils::clean(current_profile->skip_row, SkipUtils::ROW, dimensions);

    // If input was cleaned, update and loop again
    if (result.was_cleaned) {
      strncpy(current_profile->skip_row, result.cleaned, SKIP_STRING_LEN - 1);
      current_profile->skip_row[SKIP_STRING_LEN - 1] = '\0';
      Logger::log(F("Input was cleaned, showing keyboard again"));
    } else {
      // Input is clean, exit loop
      Logger::log(F("Input is clean"));
      break;
    }
  }
}

void AdvancedSettingsController::editSkipIndividual(Gpu_Hal_Context_t* phost) {
  // Create dimensions from profile
  TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);

  while (true) {
    getKeyboardValue(phost, current_profile->skip_single_pos, "Enter positions to skip", false);

    // Clean the input with bounds checking
    Logger::log(F("Cleaning!"));
    SkipUtils::CleanResult result =
        SkipUtils::clean(current_profile->skip_single_pos, SkipUtils::INDIVIDUAL, dimensions);
    Logger::log(F("Cleaned!"));

    // If input was cleaned, update and loop again
    if (result.was_cleaned) {
      Logger::log(F("Actually cleaned!"));
      strncpy(current_profile->skip_single_pos, result.cleaned, SKIP_STRING_LEN - 1);
      current_profile->skip_single_pos[SKIP_STRING_LEN - 1] = '\0';
      Logger::log(F("Input was cleaned, showing keyboard again"));
    } else {
      // Input is clean, exit loop
      Logger::log(F("Nothing changed!"));
      break;
    }
  }
}
