#include "SettingsController.h"
#include "../../views/settings/SettingsScreen.h"
#include "../../views/common/Keyboards.h"
#include "../../views/ViewCommon.h"
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
        uint8_t current_num = profile_manager.getCurrentProfileNum();
        profile_manager.writeCurIDEEPROM(current_num);
        profile_manager.writeProfileEEPROM(current_num);

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
      KeyboardResult kb_result = getKeyboardValue(phost, buf, PROGMEM_STR(F("Enter Profile Name")), false, PROFILE_NAME_MAX_LEN, NULL);

      if (kb_result.action == ACTION_BACK) {
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
      current_profile->tube_no_x = getKeypadValue(&host, current_profile->tube_no_x, TUBES_X_MIN, TUBES_X_MAX, FALSE);
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
  int next_level = current_profile->vibration_level + 1;
  if (next_level > VIBRATION_LEVEL_MAX) next_level = 0;
  current_profile->vibration_level = next_level;
}

void SettingsController::incrementVibrationTime() {
  int next_duration = current_profile->vibration_duration + 1;
  if (next_duration > VIBRATION_DURATION_MAX) next_duration = VIBRATION_DURATION_MIN;

  current_profile->vibration_duration = next_duration;
}

// Verifies all profile parameters and returns validation result with error flags.
// @return VerificationResult containing validity status and specific error flags.
 VerificationResult SettingsController::verifyParameters() {
  Logger::log(F("VP: Start"));
  // delay(20);
  
  VerificationResult result;
  result.is_valid = true;
  result.errors = InputErrors();  // Initialize all to false
  result.dialog_code = 0;

  Logger::log(F("VP: Init done"));
  // delay(20);

  // Internal flags to track error types
  bool has_parameter_error = false;
  bool has_dimension_error = false;
  bool has_skip_error = false;

  Logger::log(F("VP: Check tubes_x"));
  // delay(20);
  // Check parameter range violations
  if (current_profile->tube_no_x < TUBES_X_MIN || current_profile->tube_no_x > TUBES_X_MAX) {
    result.is_valid = false;
    has_parameter_error = true;
    result.errors.tubes_x = true;
  }

  Logger::log(F("VP: Check tubes_y"));
  // delay(20);
  if (current_profile->tube_no_y < TUBES_Y_MIN || current_profile->tube_no_y > TUBES_Y_MAX) {
    result.is_valid = false;
    has_parameter_error = true;
    result.errors.tubes_y = true;
  }

  Logger::log(F("VP: Check pitch_x"));
  // delay(20);
  if (current_profile->pitch_x < PITCH_X_MIN || current_profile->pitch_x > PITCH_X_MAX) {
    result.is_valid = false;
    has_parameter_error = true;
    result.errors.pitch_x = true;
  }

  Logger::log(F("VP: Check pitch_y"));
  // delay(20);
  if (current_profile->pitch_y < PITCH_Y_MIN || current_profile->pitch_y > PITCH_Y_MAX) {
    result.is_valid = false;
    has_parameter_error = true;
    result.errors.pitch_y = true;
  }

  Logger::log(F("VP: Check origin_x"));
  // delay(20);
  if (current_profile->tray_origin_x < ORIGIN_X_MIN || current_profile->tray_origin_x > ORIGIN_X_MAX) {
    result.is_valid = false;
    has_parameter_error = true;
    result.errors.origin_x = true;
  }

  Logger::log(F("VP: Check origin_y"));
  // delay(20);
  if (current_profile->tray_origin_y < ORIGIN_Y_MIN || current_profile->tray_origin_y > ORIGIN_Y_MAX) {
    result.is_valid = false;
    has_parameter_error = true;
    result.errors.origin_y = true;
  }

  Logger::log(F("VP: Check cycles"));
  // delay(20);
  if (current_profile->cycles < CYCLES_MIN || current_profile->cycles > CYCLES_MAX) {
    result.is_valid = false;
    has_parameter_error = true;
    result.errors.cycles = true;
  }

  Logger::log(F("VP: Check z_dip"));
  // delay(20);
  if (current_profile->z_dip < Z_DIP_MIN || current_profile->z_dip > Z_DIP_MAX) {
    result.is_valid = false;
    has_parameter_error = true;
    result.errors.z_dip = true;
  }

  Logger::log(F("VP: Check X boundary"));
  // delay(20);
  // Check dimension boundary violations
  // Validate X-axis: check if tubes exceed tray boundary
  if (current_profile->tray_origin_x + (current_profile->pitch_x * current_profile->tube_no_x) > TRAY_X_MAX) {
    result.is_valid = false;
    has_dimension_error = true;
    result.errors.tubes_x = true;
    result.errors.pitch_x = true;
    result.errors.origin_x = true;
  }

  Logger::log(F("VP: Check Y boundary"));
  // delay(20);
  // Validate Y-axis: check if tubes exceed tray boundary
  if (current_profile->tray_origin_y + (current_profile->pitch_y * current_profile->tube_no_y) > TRAY_Y_MAX) {
    result.is_valid = false;
    has_dimension_error = true;
    result.errors.tubes_y = true;
    result.errors.pitch_y = true;
    result.errors.origin_y = true;
  }

  Logger::log(F("VP: Check skip positions"));
  // delay(20);
  // Check skip position validity
  if (current_profile->skip_count > 0) {
    Logger::log(F("VP: Create dimensions"));
    // delay(20);
    TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);
    
    Logger::log(F("VP: Loop skip positions"));
    // delay(20);
    for (uint8_t i = 0; i < current_profile->skip_count && i < MAX_SKIP_POSITIONS_TOTAL; i++) {
      const SkipPosition& skip_pos = current_profile->skip_positions[i];
      TrayHandler::Position pos(skip_pos.x, skip_pos.y);
      
      // Check if position is valid for current tray configuration using SkipUtils
      if (!SkipUtils::isValidSkipPosition(pos, dimensions, current_profile->staggered)) {
        result.is_valid = false;
        has_skip_error = true;
        break;  // Found at least one invalid skip position
      }
    }
    Logger::log(F("VP: Skip loop done"));
    // delay(20);
  }

  Logger::log(F("VP: Determine dialog"));
  // delay(20);
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

  Logger::log(F("VP: Done"));
  // delay(20);
  return result;
}

// Helper method to draw the settings screen with current profile and errors.
// @param dialog_code Optional dialog code to display (default 0).
void SettingsController::drawScreen(uint8_t dialog_code) {
  VerificationResult verification = verifyParameters();
  drawSettingsScreen(phost, { *current_profile, verification.errors, dialog_code });
}

