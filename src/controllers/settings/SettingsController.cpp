#include "SettingsController.h"
#include "../../views/settings/SettingsScreen.h"
#include "../../views/common/Keyboards.h"
#include "../../views/common/Dialogs.h"
#include "../../logic/TrayPositionHandler.h"
#include "../../logic/SkipUtils.h"

SettingsController::SettingsController(ControllerParams params) : BaseController(params), current_profile(nullptr) {}

void SettingsController::onStart() {
  Logger::log(F("MODE: Config mode"));

  // Store reference to the current profile
  current_profile = &profile_manager.getCurrentProfile();

  // Display configuration screen
  drawScreen();
}

void SettingsController::onInteraction(const Interaction& interaction) {
  if (interaction.key_pressed == 0) return;

  switch (interaction.key_pressed) {
    case TAG_CONFIG_HOME:  // Home button
      Logger::log(F("Button Pressed: HOME"));
      {
        VerificationResult verification = verifyParameters();
        if (!verification.is_valid) {
          // Show appropriate error dialog based on error type
          int dialog_code = verification.has_parameter_error ? DIALOG_ERROR_PARAMETER : DIALOG_ERROR_DIMENSION;
          drawScreen(dialog_code);
        } else {
          startNextController(CONTROLLER_READY);
        }
      }
      break;

    case TAG_CONFIG_LOAD:  // Load (Config Screen)
      Logger::log(F("Button Pressed: LOAD"));
      startNextController(CONTROLLER_PROFILE);
      break;

    case TAG_CONFIG_SAVE: {
      Logger::log(F("Button Pressed: SAVE"));
      VerificationResult verification = verifyParameters();
      if (!verification.is_valid) {
        // Show appropriate error dialog based on error type
        int dialog_code = verification.has_parameter_error ? DIALOG_ERROR_PARAMETER : DIALOG_ERROR_DIMENSION;
        drawScreen(dialog_code);
      } else {
        uint8_t currentNum = profile_manager.getCurrentProfileNum();
        profile_manager.writeCurIDEEPROM(currentNum);
        profile_manager.writeProfileEEPROM(currentNum);

        // Show profile saved dialog
        drawScreen(DIALOG_PROFILE_SAVED);
        delay(DIALOG_DISPLAY_DURATION_MS);
        drawScreen();
      }
      break;
    }

    case TAG_CONFIG_PROFILE_NAME: {
      Logger::log(F("Button Pressed: PROFILE"));
      char buf[PROFILE_NAME_MAX_LEN];
      strcpy(buf, current_profile->profile_name);
      KeyboardResult kbResult = getKeyboardValue(phost, buf, "Enter Profile Name", false, PROFILE_NAME_MAX_LEN, NULL);

      if (kbResult.action == ACTION_BACK) {
        drawScreen();
        break;
      }

      if (strcmp(buf, DEBUG_MODE_KEYWORD) == 0) {
        startNextController(CONTROLLER_DEBUG);
        return;
      }

      strcpy(current_profile->profile_name, buf);
      drawScreen();
      break;
    }

    case TAG_CONFIG_TUBES_X:  // Number of Columns
    {
      float maxval = (int)((TRAY_X_MAX - current_profile->tray_origin_x) / current_profile->pitch_x) + 1;
      if (maxval > TUBES_X_MAX) maxval = TUBES_X_MAX;

      int oldTubeNoX = current_profile->tube_no_x;
      current_profile->tube_no_x = getKeypadValue(&host, current_profile->tube_no_x, TUBES_X_MIN, TUBES_X_MAX, FALSE);

      // If rows decreased, clean skip strings to remove out-of-bounds positions
      if (current_profile->tube_no_x < oldTubeNoX) {
        TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);

        // Get current skip strings
        char skipCol[SKIP_STRING_LEN], skipRow[SKIP_STRING_LEN], skipSinglePos[SKIP_STRING_LEN];
        profile_manager.getSkipStrings(skipCol, skipRow, skipSinglePos);

        // Clean skip rows
        SkipUtils::CleanResult rowResult = SkipUtils::clean(skipRow, SkipUtils::ROW, dimensions);
        if (rowResult.was_cleaned) {
          strncpy(skipRow, rowResult.cleaned, SKIP_STRING_LEN - 1);
          skipRow[SKIP_STRING_LEN - 1] = '\0';
        }

        // Clean skip individual positions
        SkipUtils::CleanResult posResult =
            SkipUtils::clean(skipSinglePos, SkipUtils::INDIVIDUAL, dimensions);
        if (posResult.was_cleaned) {
          strncpy(skipSinglePos, posResult.cleaned, SKIP_STRING_LEN - 1);
          skipSinglePos[SKIP_STRING_LEN - 1] = '\0';
        }

        // Save back to profile
        profile_manager.setSkipStrings(skipCol, skipRow, skipSinglePos);
      }

      drawScreen();
    } break;

    case TAG_CONFIG_TUBES_Y:  // Number of Rows
    {
      float maxval = (int)((TRAY_Y_MAX - current_profile->tray_origin_y) / current_profile->pitch_y) + 1;

      if (maxval > TUBES_Y_MAX) maxval = TUBES_Y_MAX;
      int oldTubeNoY = current_profile->tube_no_y;
      current_profile->tube_no_y = getKeypadValue(&host, current_profile->tube_no_y, TUBES_Y_MIN, TUBES_Y_MAX, FALSE);

      // If columns decreased, clean skip strings to remove out-of-bounds positions
      if (current_profile->tube_no_y < oldTubeNoY) {
        TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);

        // Get current skip strings
        char skipCol[SKIP_STRING_LEN], skipRow[SKIP_STRING_LEN], skipSinglePos[SKIP_STRING_LEN];
        profile_manager.getSkipStrings(skipCol, skipRow, skipSinglePos);

        // Clean skip columns
        SkipUtils::CleanResult colResult = SkipUtils::clean(skipCol, SkipUtils::COLUMN, dimensions);
        if (colResult.was_cleaned) {
          strncpy(skipCol, colResult.cleaned, SKIP_STRING_LEN - 1);
          skipCol[SKIP_STRING_LEN - 1] = '\0';
        }

        // Clean skip individual positions
        SkipUtils::CleanResult posResult =
            SkipUtils::clean(skipSinglePos, SkipUtils::INDIVIDUAL, dimensions);
        if (posResult.was_cleaned) {
          strncpy(skipSinglePos, posResult.cleaned, SKIP_STRING_LEN - 1);
          skipSinglePos[SKIP_STRING_LEN - 1] = '\0';
        }

        // Save back to profile
        profile_manager.setSkipStrings(skipCol, skipRow, skipSinglePos);
      }

      drawScreen();
    } break;

    case TAG_CONFIG_PITCH_X:  // pitch row
    {
      float maxval;
      if (current_profile->tube_no_x == TUBES_X_MIN - 1)
        maxval = TRAY_X_MAX - current_profile->tray_origin_x;
      else
        maxval = (TRAY_X_MAX - current_profile->tray_origin_x) / (current_profile->tube_no_x - 1);
      roundOneDecimal(&maxval);
      if (maxval > PITCH_X_MAX) maxval = PITCH_X_MAX;
      current_profile->pitch_x = getKeypadValue(&host, current_profile->pitch_x, PITCH_X_MIN, PITCH_X_MAX, true);
      drawScreen();
    } break;

    case TAG_CONFIG_PITCH_Y:  // pitch col
    {
      float maxval;
      if (current_profile->tube_no_y == TUBES_Y_MIN - 1)
        maxval = TRAY_Y_MAX - current_profile->tray_origin_y;
      else
        maxval = (TRAY_Y_MAX - current_profile->tray_origin_y) / (current_profile->tube_no_y - 1);
      roundOneDecimal(&maxval);
      if (maxval > PITCH_Y_MAX) maxval = PITCH_Y_MAX;
      current_profile->pitch_y = getKeypadValue(&host, current_profile->pitch_y, PITCH_Y_MIN, PITCH_Y_MAX, true);
      drawScreen();
    } break;

    case TAG_CONFIG_ORIGIN_X:  // origin row
    {
      float maxval;
      if (current_profile->tube_no_x == TUBES_X_MIN - 1)
        maxval = TRAY_X_MAX;
      else
        maxval = TRAY_X_MAX - (current_profile->pitch_x * (current_profile->tube_no_x - 1));
      roundOneDecimal(&maxval);
      if (maxval > ORIGIN_X_MAX) maxval = ORIGIN_X_MAX;

      current_profile->tray_origin_x = getKeypadValue(&host, current_profile->tray_origin_x, 0, ORIGIN_X_MAX, true);
      drawScreen();
    } break;

    case TAG_CONFIG_ORIGIN_Y:  // origin col
    {
      float maxval;
      if (current_profile->tube_no_y == TUBES_Y_MIN - 1)
        maxval = TRAY_Y_MAX;
      else
        maxval = TRAY_Y_MAX - (current_profile->pitch_y * (current_profile->tube_no_y - 1));
      roundOneDecimal(&maxval);
      if (maxval > ORIGIN_Y_MAX) maxval = ORIGIN_Y_MAX;
      current_profile->tray_origin_y = getKeypadValue(&host, current_profile->tray_origin_y, 0, ORIGIN_Y_MAX, true);
      drawScreen();
    } break;

    case TAG_NUM_CYCLE:
      current_profile->cycles = getKeypadValue(phost, current_profile->cycles, CYCLES_MIN, CYCLES_MAX, false);
      drawScreen();
      break;

    case TAG_Z_DIP:
      Logger::log(F("Incrementing Z Dip"));
      current_profile->z_dip = getKeypadValue(phost, current_profile->z_dip, Z_DIP_MIN, Z_DIP_MAX, true);
      drawScreen();
      break;

    case TAG_VIBRATION_LEVEL:
      Logger::log(F("Incrementing vibration level"));
      incrementVibrationLevel();
      drawScreen();
      break;

    case TAG_VIBRATION_DURATION:
      Logger::log(F("Incrementing vibration duration"));
      incrementVibrationTime();
      drawScreen();
      break;

    case TAG_PASSWORD_ENABLED:
      Logger::log(F("Toggle password enable"));
      current_profile->password_enabled = !current_profile->password_enabled;
      drawScreen();
      break;

    case TAG_STAGGERED_TOGGLE:
    case TAG_SKIP_COLUMNS:
    case TAG_SKIP_ROWS:
    case TAG_SKIP_SINGLE_POS:
    case TAG_ADV_PROF_BACK:
      // These are handled by AdvancedSettingsController
      break;

    case TAG_ADVANCED:
      Logger::log(F("Button Pressed: ADVANCED"));
      startNextController(CONTROLLER_ADVANCED_SETTINGS);
      break;

    case TAG_CONTINUE:
      Logger::log(F("Button Pressed: CONTINUE"));
      drawScreen();
      break;

    default:
      break;
  }
}

ControllerStepResult SettingsController::onStep() {
  DispenserProcessResult result = dispenserHead.process();
  return ControllerStepResult(result.steppers == AXIS_STATE_RUNNING);
}

int SettingsController::getModeType() const {
  return CONTROLLER_SETTINGS;
}

void SettingsController::incrementVibrationLevel() {
  int next_level = current_profile->vibration_enabled + 1;
  if (next_level > VIBRATION_LEVEL_MAX) next_level = 0;
  current_profile->vibration_enabled = next_level;
}

void SettingsController::incrementVibrationTime() {
  int next_duration = current_profile->vibration_duration + 1;
  if (next_duration > VIBRATION_DURATION_MAX) next_duration = VIBRATION_DURATION_MIN;

  current_profile->vibration_duration = next_duration;
}

// Verifies all profile parameters and returns validation result with error flags.
// @return VerificationResult containing validity status and specific error flags.
VerificationResult SettingsController::verifyParameters() {
  VerificationResult result;
  result.is_valid = true;
  result.errors = InputErrors();  // Initialize all to false
  result.has_parameter_error = false;
  result.has_dimension_error = false;

  // Check parameter range violations
  if (current_profile->tube_no_x < TUBES_X_MIN || current_profile->tube_no_x > TUBES_X_MAX) {
    result.is_valid = false;
    result.has_parameter_error = true;
    result.errors.tubes_x = true;
  }

  if (current_profile->tube_no_y < TUBES_Y_MIN || current_profile->tube_no_y > TUBES_Y_MAX) {
    result.is_valid = false;
    result.has_parameter_error = true;
    result.errors.tubes_y = true;
  }

  if (current_profile->pitch_x < PITCH_X_MIN || current_profile->pitch_x > PITCH_X_MAX) {
    result.is_valid = false;
    result.has_parameter_error = true;
    result.errors.pitch_x = true;
  }

  if (current_profile->pitch_y < PITCH_Y_MIN || current_profile->pitch_y > PITCH_Y_MAX) {
    result.is_valid = false;
    result.has_parameter_error = true;
    result.errors.pitch_y = true;
  }

  if (current_profile->tray_origin_x < ORIGIN_X_MIN || current_profile->tray_origin_x > ORIGIN_X_MAX) {
    result.is_valid = false;
    result.has_parameter_error = true;
    result.errors.origin_x = true;
  }

  if (current_profile->tray_origin_y < ORIGIN_Y_MIN || current_profile->tray_origin_y > ORIGIN_Y_MAX) {
    result.is_valid = false;
    result.has_parameter_error = true;
    result.errors.origin_y = true;
  }

  if (current_profile->cycles < CYCLES_MIN || current_profile->cycles > CYCLES_MAX) {
    result.is_valid = false;
    result.has_parameter_error = true;
    result.errors.cycles = true;
  }

  if (current_profile->z_dip < Z_DIP_MIN || current_profile->z_dip > Z_DIP_MAX) {
    result.is_valid = false;
    result.has_parameter_error = true;
    result.errors.z_dip = true;
  }

  // Check dimension boundary violations
  // Validate X-axis: check if tubes exceed tray boundary
  if (current_profile->tray_origin_x + (current_profile->pitch_x * current_profile->tube_no_x) > TRAY_X_MAX) {
    result.is_valid = false;
    result.has_dimension_error = true;
    result.errors.tubes_x = true;
    result.errors.pitch_x = true;
    result.errors.origin_x = true;
  }

  // Validate Y-axis: check if tubes exceed tray boundary
  if (current_profile->tray_origin_y + (current_profile->pitch_y * current_profile->tube_no_y) > TRAY_Y_MAX) {
    result.is_valid = false;
    result.has_dimension_error = true;
    result.errors.tubes_y = true;
    result.errors.pitch_y = true;
    result.errors.origin_y = true;
  }

  return result;
}

// Helper method to draw the settings screen with current profile and errors.
// @param dialog_code Optional dialog code to display (default 0).
void SettingsController::drawScreen(int dialog_code) {
  VerificationResult verification = verifyParameters();
  drawSettingsScreen(phost, { *current_profile, verification.errors, dialog_code });
}

