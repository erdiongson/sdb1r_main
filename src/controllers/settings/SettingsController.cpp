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
    case TAG_CONFIG_HOME: {
      VerificationResult verification = verifyParameters();
      if (!verification.is_valid) {
        drawScreen(verification.dialog_code);
      } else {
        startNextController(CONTROLLER_READY);
      }
      break;
    }

    case TAG_CONFIG_LOAD: 
      startNextController(CONTROLLER_PROFILE);
      break;

    case TAG_CONFIG_SAVE: {
      VerificationResult verification = verifyParameters();
      if (!verification.is_valid) {
        drawScreen(verification.dialog_code);
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

      if (strcmp_P(buf, DEBUG_MODE_KEYWORD) == 0) {
        startNextController(CONTROLLER_DEBUG);
        return;
      }

      strcpy(current_profile->profile_name, buf);
      drawScreen();
      break;
    }

    case TAG_CONFIG_TUBES_X:
    {
      float maxval = (int)((TRAY_X_MAX - current_profile->tray_origin_x) / current_profile->pitch_x) + 1;
      if (maxval > TUBES_X_MAX) maxval = TUBES_X_MAX;

      int oldTubeNoX = current_profile->tube_no_x;
      current_profile->tube_no_x = getKeypadValue(&host, current_profile->tube_no_x, TUBES_X_MIN, TUBES_X_MAX, FALSE);

      // If rows decreased, clean skip strings to remove out-of-bounds positions
      if (current_profile->tube_no_x < oldTubeNoX) {
        TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);

        // Get current skip strings
        char skip_col[SKIP_STRING_LEN], skip_row[SKIP_STRING_LEN], skip_single_pos[SKIP_STRING_LEN];
        profile_manager.getSkipStrings(skip_col, skip_row, skip_single_pos);

        // Clean skip rows
        SkipUtils::CleanResult rowResult = SkipUtils::clean(skip_row, SkipUtils::ROW, dimensions);
        if (rowResult.was_cleaned) {
          strncpy(skip_row, rowResult.cleaned, SKIP_STRING_LEN - 1);
          skip_row[SKIP_STRING_LEN - 1] = '\0';
        }

        // Clean skip individual positions
        SkipUtils::CleanResult posResult =
            SkipUtils::clean(skip_single_pos, SkipUtils::INDIVIDUAL, dimensions);
        if (posResult.was_cleaned) {
          strncpy(skip_single_pos, posResult.cleaned, SKIP_STRING_LEN - 1);
          skip_single_pos[SKIP_STRING_LEN - 1] = '\0';
        }

        // Save back to profile
        profile_manager.setSkipStrings(skip_col, skip_row, skip_single_pos);
      }

      drawScreen();
    } break;

    case TAG_CONFIG_TUBES_Y: 
    {
      current_profile->tube_no_y = getKeypadValue(&host, current_profile->tube_no_y, TUBES_Y_MIN, TUBES_Y_MAX, FALSE);
      drawScreen();
    } break;

    case TAG_CONFIG_PITCH_X: 
    {
      current_profile->pitch_x = getKeypadValue(&host, current_profile->pitch_x, PITCH_X_MIN, PITCH_X_MAX, true);
      drawScreen();
    } break;

    case TAG_CONFIG_PITCH_Y: 
    {
      current_profile->pitch_y = getKeypadValue(&host, current_profile->pitch_y, PITCH_Y_MIN, PITCH_Y_MAX, true);
      drawScreen();
    } break;

    case TAG_CONFIG_ORIGIN_X:  
    {
      current_profile->tray_origin_x = getKeypadValue(&host, current_profile->tray_origin_x, ORIGIN_X_MIN, ORIGIN_X_MAX, true);
      drawScreen();
    } break;

    case TAG_CONFIG_ORIGIN_Y: 
    {
      current_profile->tray_origin_y = getKeypadValue(&host, current_profile->tray_origin_y, ORIGIN_Y_MIN, ORIGIN_Y_MAX, true);
      drawScreen();
    } break;

    case TAG_NUM_CYCLE:
      current_profile->cycles = getKeypadValue(phost, current_profile->cycles, CYCLES_MIN, CYCLES_MAX, false);
      drawScreen();
      break;

    case TAG_Z_DIP:
      current_profile->z_dip = getKeypadValue(phost, current_profile->z_dip, Z_DIP_MIN, Z_DIP_MAX, true);
      drawScreen();
      break;

    case TAG_VIBRATION_LEVEL:
      incrementVibrationLevel();
      drawScreen();
      break;

    case TAG_VIBRATION_DURATION:
      incrementVibrationTime();
      drawScreen();
      break;

    case TAG_PASSWORD_ENABLED:
      current_profile->password_enabled = !current_profile->password_enabled;
      drawScreen();
      break;

    case TAG_ADVANCED:
      startNextController(CONTROLLER_ADVANCED_SETTINGS);
      break;

    case TAG_CONTINUE:
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
  result.dialog_code = 0;

  // Internal flags to track error types
  bool has_parameter_error = false;
  bool has_dimension_error = false;
  bool has_skip_error = false;

  // Check parameter range violations
  if (current_profile->tube_no_x < TUBES_X_MIN || current_profile->tube_no_x > TUBES_X_MAX) {
    result.is_valid = false;
    has_parameter_error = true;
    result.errors.tubes_x = true;
  }

  if (current_profile->tube_no_y < TUBES_Y_MIN || current_profile->tube_no_y > TUBES_Y_MAX) {
    result.is_valid = false;
    has_parameter_error = true;
    result.errors.tubes_y = true;
  }

  if (current_profile->pitch_x < PITCH_X_MIN || current_profile->pitch_x > PITCH_X_MAX) {
    result.is_valid = false;
    has_parameter_error = true;
    result.errors.pitch_x = true;
  }

  if (current_profile->pitch_y < PITCH_Y_MIN || current_profile->pitch_y > PITCH_Y_MAX) {
    result.is_valid = false;
    has_parameter_error = true;
    result.errors.pitch_y = true;
  }

  if (current_profile->tray_origin_x < ORIGIN_X_MIN || current_profile->tray_origin_x > ORIGIN_X_MAX) {
    result.is_valid = false;
    has_parameter_error = true;
    result.errors.origin_x = true;
  }

  if (current_profile->tray_origin_y < ORIGIN_Y_MIN || current_profile->tray_origin_y > ORIGIN_Y_MAX) {
    result.is_valid = false;
    has_parameter_error = true;
    result.errors.origin_y = true;
  }

  if (current_profile->cycles < CYCLES_MIN || current_profile->cycles > CYCLES_MAX) {
    result.is_valid = false;
    has_parameter_error = true;
    result.errors.cycles = true;
  }

  if (current_profile->z_dip < Z_DIP_MIN || current_profile->z_dip > Z_DIP_MAX) {
    result.is_valid = false;
    has_parameter_error = true;
    result.errors.z_dip = true;
  }

  // Check dimension boundary violations
  // Validate X-axis: check if tubes exceed tray boundary
  if (current_profile->tray_origin_x + (current_profile->pitch_x * current_profile->tube_no_x) > TRAY_X_MAX) {
    result.is_valid = false;
    has_dimension_error = true;
    result.errors.tubes_x = true;
    result.errors.pitch_x = true;
    result.errors.origin_x = true;
  }

  // Validate Y-axis: check if tubes exceed tray boundary
  if (current_profile->tray_origin_y + (current_profile->pitch_y * current_profile->tube_no_y) > TRAY_Y_MAX) {
    result.is_valid = false;
    has_dimension_error = true;
    result.errors.tubes_y = true;
    result.errors.pitch_y = true;
    result.errors.origin_y = true;
  }

  // Check skip position validity
  if (current_profile->skip_count > 0) {
    TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);
    
    for (uint8_t i = 0; i < current_profile->skip_count && i < MAX_SKIP_POSITIONS; i++) {
      const SkipPosition& skipPos = current_profile->skip_positions[i];
      TrayHandler::Position pos(skipPos.x, skipPos.y);
      
      // Check if position is valid for current tray configuration using SkipUtils
      if (!SkipUtils::isValidSkipPosition(pos, dimensions, current_profile->staggered)) {
        result.is_valid = false;
        has_skip_error = true;
        break;  // Found at least one invalid skip position
      }
    }
  }

  // Determine dialog code based on priority: parameter > dimension > skip
  if (!result.is_valid) {
    if (has_parameter_error) {
      result.dialog_code = DIALOG_ERROR_PARAMETER;
    } else if (has_dimension_error) {
      result.dialog_code = DIALOG_ERROR_DIMENSION;
    } else if (has_skip_error) {
      result.dialog_code = DIALOG_ERROR_SKIP_VALUES;
    }
  }

  return result;
}

// Helper method to draw the settings screen with current profile and errors.
// @param dialog_code Optional dialog code to display (default 0).
void SettingsController::drawScreen(int dialog_code) {
  VerificationResult verification = verifyParameters();
  drawSettingsScreen(phost, { *current_profile, verification.errors, dialog_code });
}

