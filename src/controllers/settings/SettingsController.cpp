#include "SettingsController.h"
#include "../../views/settings/SettingsScreen.h"
#include "../../views/common/Keyboards.h"
#include "../../logic/TrayPositionHandler.h"
#include "../../logic/SkipUtils.h"

SettingsController::SettingsController(ControllerParams params) : BaseController(params), current_profile(nullptr) {}

void SettingsController::onStart() {
  Logger::log(F("MODE: Config mode"));

  // Store reference to the current profile
  current_profile = &profile_manager.getCurrentProfile();

  // Display configuration screen
  drawSettingsScreen(phost, { *current_profile, 0 });
}

void SettingsController::onInteraction(const Interaction& interaction) {
  if (interaction.key_pressed == 0) return;

  switch (interaction.key_pressed) {
    case TAG_CONFIG_HOME:  // Home button
      Logger::log(F("Button Pressed: HOME"));
      {
        char buf[PROFILE_NAME_MAX_LEN];
        float maxval = 0;
        bool error = FALSE;

        if (current_profile->tube_no_x == 0)
          maxval = TRAY_X_MAX - current_profile->tray_origin_x;
        else
          maxval = (TRAY_X_MAX - current_profile->tray_origin_x) / (current_profile->tube_no_x - 1);
        roundOneDecimal(&maxval);
        if (current_profile->pitch_x > maxval)
          error = TRUE;
        else {
          if (current_profile->tube_no_y == 0)
            maxval = TRAY_Y_MAX - current_profile->tray_origin_y;
          else
            maxval = (TRAY_Y_MAX - current_profile->tray_origin_y) / (current_profile->tube_no_y - 1);
          roundOneDecimal(&maxval);
          if (current_profile->pitch_y > maxval) error = TRUE;
        }

        if (error) {
          strcpy(buf, current_profile->profile_name);
          sprintf(current_profile->profile_name, "Error in entry");
          drawSettingsScreen(phost, { *current_profile, 0 });
          delay(ERROR_DISPLAY_DURATION_MS);
          strcpy(current_profile->profile_name, buf);
          drawSettingsScreen(phost, { *current_profile, 0 });
          delay(ERROR_DISPLAY_DURATION_MS);
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
      Logger::log("Button Pressed: SAVE");
      uint8_t currentNum = profile_manager.getCurrentProfileNum();
      profile_manager.writeCurIDEEPROM(currentNum);
      profile_manager.writeProfileEEPROM(currentNum);

      // Show profile saved dialog
      drawSettingsScreen(phost, { *current_profile, DIALOG_PROFILE_SAVED });
      delay(DIALOG_DISPLAY_DURATION_MS);
      drawSettingsScreen(phost, { *current_profile, 0 });
      break;
    }

    case TAG_CONFIG_PROFILE_NAME: {
      Logger::log(F("Button Pressed: PROFILE"));
      char buf[PROFILE_NAME_MAX_LEN];
      strcpy(buf, current_profile->profile_name);
      getKeyboardValue(phost, buf, "Enter Profile Name", FALSE);

      if (strcmp(buf, "debug") == 0) {
        startNextController(CONTROLLER_DEBUG);
        return;
      }

      strcpy(current_profile->profile_name, buf);
      drawSettingsScreen(phost, { *current_profile, 0 });
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

        // Clean skip rows
        SkipUtils::CleanResult rowResult = SkipUtils::clean(current_profile->skip_row, SkipUtils::ROW, dimensions);
        if (rowResult.was_cleaned) {
          strncpy(current_profile->skip_row, rowResult.cleaned, ROW_COL_MAX_LEN - 1);
          current_profile->skip_row[ROW_COL_MAX_LEN - 1] = '\0';
        }

        // Clean skip individual positions
        SkipUtils::CleanResult posResult =
            SkipUtils::clean(current_profile->skip_single_pos, SkipUtils::INDIVIDUAL, dimensions);
        if (posResult.was_cleaned) {
          strncpy(current_profile->skip_single_pos, posResult.cleaned, ROW_COL_MAX_LEN - 1);
          current_profile->skip_single_pos[ROW_COL_MAX_LEN - 1] = '\0';
        }
      }

      drawSettingsScreen(phost, { *current_profile, 0 });
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

        // Clean skip columns
        SkipUtils::CleanResult colResult = SkipUtils::clean(current_profile->skip_col, SkipUtils::COLUMN, dimensions);
        if (colResult.was_cleaned) {
          strncpy(current_profile->skip_col, colResult.cleaned, ROW_COL_MAX_LEN - 1);
          current_profile->skip_col[ROW_COL_MAX_LEN - 1] = '\0';
        }

        // Clean skip individual positions
        SkipUtils::CleanResult posResult =
            SkipUtils::clean(current_profile->skip_single_pos, SkipUtils::INDIVIDUAL, dimensions);
        if (posResult.was_cleaned) {
          strncpy(current_profile->skip_single_pos, posResult.cleaned, ROW_COL_MAX_LEN - 1);
          current_profile->skip_single_pos[ROW_COL_MAX_LEN - 1] = '\0';
        }
      }

      drawSettingsScreen(phost, { *current_profile, 0 });
    } break;

    case TAG_CONFIG_PITCH_X:  // pitch row
    {
      float maxval;
      if (current_profile->tube_no_x == 0)
        maxval = TRAY_X_MAX - current_profile->tray_origin_x;
      else
        maxval = (TRAY_X_MAX - current_profile->tray_origin_x) / (current_profile->tube_no_x - 1);
      roundOneDecimal(&maxval);
      if (maxval > PITCH_X_MAX) maxval = PITCH_X_MAX;
      current_profile->pitch_x = getKeypadValue(&host, current_profile->pitch_x, PITCH_X_MIN, PITCH_X_MAX, TRUE);
      drawSettingsScreen(phost, { *current_profile, 0 });
    } break;

    case TAG_CONFIG_PITCH_Y:  // pitch col
    {
      float maxval;
      if (current_profile->tube_no_y == 0)
        maxval = TRAY_Y_MAX - current_profile->tray_origin_y;
      else
        maxval = (TRAY_Y_MAX - current_profile->tray_origin_y) / (current_profile->tube_no_y - 1);
      roundOneDecimal(&maxval);
      if (maxval > PITCH_Y_MAX) maxval = PITCH_Y_MAX;
      current_profile->pitch_y = getKeypadValue(&host, current_profile->pitch_y, PITCH_Y_MIN, PITCH_Y_MAX, TRUE);
      drawSettingsScreen(phost, { *current_profile, 0 });
    } break;

    case TAG_CONFIG_ORIGIN_X:  // origin row
    {
      float maxval;
      if (current_profile->tube_no_x == 0)
        maxval = TRAY_X_MAX;
      else
        maxval = TRAY_X_MAX - (current_profile->pitch_x * (current_profile->tube_no_x - 1));
      roundOneDecimal(&maxval);
      if (maxval > ORIGIN_X_MAX) maxval = ORIGIN_X_MAX;

      current_profile->tray_origin_x = getKeypadValue(&host, current_profile->tray_origin_x, 0, ORIGIN_X_MAX, TRUE);
      drawSettingsScreen(phost, { *current_profile, 0 });
    } break;

    case TAG_CONFIG_ORIGIN_Y:  // origin col
    {
      float maxval;
      if (current_profile->tube_no_y == 0)
        maxval = TRAY_Y_MAX;
      else
        maxval = TRAY_Y_MAX - (current_profile->pitch_y * (current_profile->tube_no_y - 1));
      roundOneDecimal(&maxval);
      if (maxval > ORIGIN_Y_MAX) maxval = ORIGIN_Y_MAX;
      current_profile->tray_origin_y = getKeypadValue(&host, current_profile->tray_origin_y, 0, ORIGIN_Y_MAX, TRUE);
      drawSettingsScreen(phost, { *current_profile, 0 });
    } break;

    case TAG_NUM_CYCLE:
      current_profile->cycles = getKeypadValue(phost, current_profile->cycles, CYCLES_MIN, CYCLES_MAX, FALSE);
      drawSettingsScreen(phost, { *current_profile, 0 });
      break;

    case TAG_Z_DIP:
      Logger::log(F("Incrementing Z Dip"));
      current_profile->z_dip = getKeypadValue(phost, current_profile->z_dip, Z_DIP_MIN, Z_DIP_MAX, TRUE);
      drawSettingsScreen(phost, { *current_profile, 0 });
      break;

    case TAG_VIBRATION_LEVEL:
      Logger::log(F("Incrementing vibration level"));
      incrementVibrationLevel();
      drawSettingsScreen(phost, { *current_profile, 0 });
      break;

    case TAG_VIBRATION_DURATION:
      Logger::log(F("Incrementing vibration duration"));
      incrementVibrationTime();
      drawSettingsScreen(phost, { *current_profile, 0 });
      break;

    case TAG_PASSWORD_ENABLED:
      Logger::log(F("Toggle password enable"));
      current_profile->password_enabled = !current_profile->password_enabled;
      drawSettingsScreen(phost, { *current_profile, 0 });
      break;

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
      drawSettingsScreen(phost, { *current_profile, 0 });
      break;

    case TAG_CONFIG_PREVIEW:
      Logger::log(F("Button Pressed: PREVIEW"));
      startNextController(CONTROLLER_PREVIEW);
      break;

    case TAG_ADVANCED:
      Logger::log(F("Button Pressed: ADVANCED"));
      drawSkipScreen(phost, *current_profile);
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

void SettingsController::editSkipColumn(Gpu_Hal_Context_t* phost) {
  // Create dimensions from profile
  TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);

  while (true) {
    getKeyboardValue(phost, current_profile->skip_col, "Enter columns to skip", FALSE);

    // Clean the input with bounds checking
    SkipUtils::CleanResult result = SkipUtils::clean(current_profile->skip_col, SkipUtils::COLUMN, dimensions);

    // If input was cleaned, update and loop again
    if (result.was_cleaned) {
      strncpy(current_profile->skip_col, result.cleaned, ROW_COL_MAX_LEN - 1);
      current_profile->skip_col[ROW_COL_MAX_LEN - 1] = '\0';
      Logger::log(F("Input was cleaned, showing keyboard again"));
    } else {
      // Input is clean, exit loop
      Logger::log(F("Input is clean"));
      break;
    }
  }
}

void SettingsController::editSkipRow(Gpu_Hal_Context_t* phost) {
  // Create dimensions from profile
  TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);

  while (true) {
    getKeyboardValue(phost, current_profile->skip_row, "Enter rows to skip", FALSE);

    // Clean the input with bounds checking
    SkipUtils::CleanResult result = SkipUtils::clean(current_profile->skip_row, SkipUtils::ROW, dimensions);

    // If input was cleaned, update and loop again
    if (result.was_cleaned) {
      strncpy(current_profile->skip_row, result.cleaned, ROW_COL_MAX_LEN - 1);
      current_profile->skip_row[ROW_COL_MAX_LEN - 1] = '\0';
      Logger::log(F("Input was cleaned, showing keyboard again"));
    } else {
      // Input is clean, exit loop
      Logger::log(F("Input is clean"));
      break;
    }
  }
}

void SettingsController::editSkipIndividual(Gpu_Hal_Context_t* phost) {
  // Create dimensions from profile
  TrayHandler::Dimensions dimensions(current_profile->tube_no_x, current_profile->tube_no_y);

  while (true) {
    getKeyboardValue(phost, current_profile->skip_single_pos, "Enter positions to skip", FALSE);

    // Clean the input with bounds checking
    Logger::log(F("Cleaning!"));
    SkipUtils::CleanResult result =
        SkipUtils::clean(current_profile->skip_single_pos, SkipUtils::INDIVIDUAL, dimensions);
    Logger::log(F("Cleaned!"));

    // If input was cleaned, update and loop again
    if (result.was_cleaned) {
      Logger::log(F("Actually cleaned!"));
      strncpy(current_profile->skip_single_pos, result.cleaned, ROW_COL_MAX_LEN - 1);
      current_profile->skip_single_pos[ROW_COL_MAX_LEN - 1] = '\0';
      Logger::log(F("Input was cleaned, showing keyboard again"));
    } else {
      // Input is clean, exit loop
      Logger::log(F("Nothing changed!"));
      break;
    }
  }
}
