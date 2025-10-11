#include "../../gpu/Platform.h"
#include "../../../Config.h"
#include "../../views/settings/SettingsScreen.h"
#include "../../views/common/Keyboards.h"
#include "../../logic/TrayPositionHandler.h"
#include "../../logic/SkipUtils.h"
#include "SettingsController.h"
#include "../../Utils.h"

SettingsController::SettingsController(ControllerParams params)
  : BaseController(params), current_profile(nullptr) {}

void SettingsController::onStart() {
  Serial.println(F("MODE: Config mode"));

  // Store reference to the current profile
  current_profile = &CurProf;

  // Display configuration screen
  drawSettingsScreen(phost);
}

void SettingsController::onInteraction(const Interaction& interaction) {
  if (interaction.key_pressed == 0) return;

  switch (interaction.key_pressed) {
    case TAG_CONFIG_HOME:  // Home button
      Serial.println(F("Button Pressed: HOME"));
      {
        char buf[PROFILE_NAME_MAX_LEN];
        float maxval = 0;
        bool error = FALSE;

        if (current_profile->tube_no_x == 0) maxval = MAXXMM - current_profile->tray_origin_x;
        else maxval = (MAXXMM - current_profile->tray_origin_x) / (current_profile->tube_no_x - 1);
        roundOneDecimal(&maxval);
        if (current_profile->pitch_x > maxval) error = TRUE;
        else {
          if (current_profile->tube_no_y == 0) maxval = MAXYMM - current_profile->tray_origin_y;
          else maxval = (MAXYMM - current_profile->tray_origin_y) / (current_profile->tube_no_y - 1);
          roundOneDecimal(&maxval);
          if (current_profile->pitch_y > maxval) error = TRUE;
        }

        if (error) {
          strcpy(buf, current_profile->profile_name);
          sprintf(current_profile->profile_name, "Error in entry");
          drawSettingsScreen(phost);
          delay(3000);
          strcpy(current_profile->profile_name, buf);
          drawSettingsScreen(phost);
          delay(3000);
        } else {
          startNextController(CONTROLLER_READY);
        }
      }
      break;

    case TAG_CONFIG_LOAD:  // Load (Config Screen)
      Serial.println(F("Button Pressed: LOAD"));
      startNextController(CONTROLLER_PROFILE);
      break;

    case TAG_CONFIG_SAVE: {
      Serial.println("Button Pressed: SAVE");
      Dprint("curprofnum=", CurProfNum);
      profileManager.writeCurIDEEPROM(CurProfNum);
      profileManager.writeProfileEEPROM(CurProfNum);
      
      // Show profile saved dialog
      SettingsScreenParams params = {DIALOG_PROFILE_SAVED};
      drawSettingsScreen(phost, params);
      delay(2000);
      params.dialog_code = 0;
      drawSettingsScreen(phost, params);
      break;
    }

    case TAG_CONFIG_PROFILE_NAME: {
      Serial.println(F("Button Pressed: PROFILE"));
        char buf[PROFILE_NAME_MAX_LEN];
        strcpy(buf, current_profile->profile_name);
        getKeyboardValue(phost, buf, "Enter Profile Name", FALSE);
        
        if (strcmp(buf, "debug") == 0) {
          startNextController(CONTROLLER_DEBUG);
          return;
        }

        strcpy(current_profile->profile_name, buf);
        drawSettingsScreen(phost);
      break;
    }

    case TAG_CONFIG_TUBES_X:  // Number of Columns
      {
        float maxval = (int)((MAXXMM - current_profile->tray_origin_x) / current_profile->pitch_x) + 1;
        if (maxval > MAXNUMX) maxval = MAXNUMX;

        int oldTubeNoX = current_profile->tube_no_x;
        current_profile->tube_no_x = getKeypadValue(&host, current_profile->tube_no_x, MINNUMX, MAXNUMX, FALSE);
        
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
          SkipUtils::CleanResult posResult = SkipUtils::clean(current_profile->skip_single_pos, SkipUtils::INDIVIDUAL, dimensions);
          if (posResult.was_cleaned) {
            strncpy(current_profile->skip_single_pos, posResult.cleaned, ROW_COL_MAX_LEN - 1);
            current_profile->skip_single_pos[ROW_COL_MAX_LEN - 1] = '\0';
          }
        }
        
        drawSettingsScreen(phost);
      }
      break;

    case TAG_CONFIG_TUBES_Y:  // Number of Rows
      {
        float maxval = (int)((MAXYMM - current_profile->tray_origin_y) / current_profile->pitch_y) + 1;

        if (maxval > MAXNUMY) maxval = MAXNUMY;
        int oldTubeNoY = current_profile->tube_no_y;
        current_profile->tube_no_y = getKeypadValue(&host, current_profile->tube_no_y, MINNUMY, MAXNUMY, FALSE);
        
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
          SkipUtils::CleanResult posResult = SkipUtils::clean(current_profile->skip_single_pos, SkipUtils::INDIVIDUAL, dimensions);
          if (posResult.was_cleaned) {
            strncpy(current_profile->skip_single_pos, posResult.cleaned, ROW_COL_MAX_LEN - 1);
            current_profile->skip_single_pos[ROW_COL_MAX_LEN - 1] = '\0';
          }
        }
        
        drawSettingsScreen(phost);
      }
      break;

    case TAG_CONFIG_PITCH_X:  //pitch row
      {
        float maxval;
        if (current_profile->tube_no_x == 0) maxval = MAXXMM - current_profile->tray_origin_x;
        else maxval = (MAXXMM - current_profile->tray_origin_x) / (current_profile->tube_no_x - 1);
        roundOneDecimal(&maxval);
        Dprint("max val=", maxval);
        if (maxval > MAXPITCHX) maxval = MAXPITCHX;
        current_profile->pitch_x = getKeypadValue(&host, current_profile->pitch_x, MINPITCHX, MAXPITCHX, TRUE);
        drawSettingsScreen(phost);
      }
      break;

    case TAG_CONFIG_PITCH_Y:  //pitch col
      {
        float maxval;
        if (current_profile->tube_no_y == 0) maxval = MAXYMM - current_profile->tray_origin_y;
        else maxval = (MAXYMM - current_profile->tray_origin_y) / (current_profile->tube_no_y - 1);
        roundOneDecimal(&maxval);
        if (maxval > MAXPITCHY) maxval = MAXPITCHY;
        Dprint("max val=", maxval);
        current_profile->pitch_y = getKeypadValue(&host, current_profile->pitch_y, MINPITCHY, MAXPITCHY, TRUE);
        drawSettingsScreen(phost);
      }
      break;

    case TAG_CONFIG_ORIGIN_X:  //origin row
      {
        float maxval;
        if (current_profile->tube_no_x == 0) maxval = MAXXMM;
        else maxval = MAXXMM - (current_profile->pitch_x * (current_profile->tube_no_x - 1));
        roundOneDecimal(&maxval);
        if (maxval > MAXORGX) maxval = MAXORGX;
        Dprint("max val=", maxval);

        current_profile->tray_origin_x = getKeypadValue(&host, current_profile->tray_origin_x, 0, MAXORGX, TRUE);
        drawSettingsScreen(phost);
      }
      break;

    case TAG_CONFIG_ORIGIN_Y:  //origin col
      {
        float maxval;
        if (current_profile->tube_no_y == 0) maxval = MAXYMM;
        else maxval = MAXYMM - (current_profile->pitch_y * (current_profile->tube_no_y - 1));
        roundOneDecimal(&maxval);
        if (maxval > MAXORGY) maxval = MAXORGY;
        Dprint("max val=", maxval);
        current_profile->tray_origin_y = getKeypadValue(&host, current_profile->tray_origin_y, 0, MAXORGY, TRUE);
        drawSettingsScreen(phost);
      }
      break;

    case TAG_NUM_CYCLE:
      current_profile->cycles = getKeypadValue(phost, current_profile->cycles, MINCYCLE, MAXCYCLE, FALSE);
      drawSettingsScreen(phost);
      break;

    case TAG_Z_DIP:
      Serial.println(F("Incrementing Z Dip"));
      current_profile->z_dip = getKeypadValue(phost, current_profile->z_dip, MINZDIP, MAXZDIP, TRUE);
      drawSettingsScreen(phost);
      break;

    case TAG_VIBRATION_LEVEL:
      Serial.println(F("Incrementing vibration level"));
      incrementVibrationLevel();
      drawSettingsScreen(phost);
      break;

    case TAG_VIBRATION_DURATION:
      Serial.println(F("Incrementing vibration duration"));
      incrementVibrationTime();
      drawSettingsScreen(phost);
      break;

    case TAG_PASSWORD_ENABLED:
      Serial.println(F("Toggle password enable"));
      current_profile->password_enabled = !current_profile->password_enabled;
      drawSettingsScreen(phost);
      break;

    case TAG_STAGGERED_TOGGLE:
      Serial.println(F("Toggle staggered mode"));
      current_profile->staggered = !current_profile->staggered;
      drawSkipScreen(phost);
      break;

    case TAG_SKIP_COLUMNS:
      Serial.println(F("Button Pressed: SKIP COLUMNS"));
      editSkipColumn(phost);
      drawSkipScreen(phost);
      break;

    case TAG_SKIP_ROWS:
      Serial.println(F("Button Pressed: SKIP ROWS"));
      editSkipRow(phost);
      drawSkipScreen(phost);
      break;

    case TAG_SKIP_SINGLE_POS:
      Serial.println(F("Button Pressed: SKIP SINGLE POSITION"));
      editSkipIndividual(phost);
      drawSkipScreen(phost);
      break;

    case TAG_ADV_PROF_BACK:  // Back button
      Serial.println(F("Button Pressed: BACK"));
      drawSettingsScreen(phost);
      break;

    case TAG_CONFIG_PREVIEW:
      Serial.println(F("Button Pressed: PREVIEW"));
      startNextController(CONTROLLER_PREVIEW);
      break;

      case TAG_ADVANCED:
        Serial.println(F("Button Pressed: ADVANCED"));
        drawSkipScreen(phost);
        break;

    default:
      break;
  }
}

ControllerStepResult SettingsController::onStep() {
  DispenserProcessResult result = dispenserHead.process();
  return ControllerStepResult(result.steppers, result.dispenser);
}

int SettingsController::getModeType() const {
  return CONTROLLER_SETTINGS;
}

void SettingsController::incrementVibrationLevel() {
  int next_level = current_profile->vibration_enabled + 1;
  if (next_level > 4) next_level = 0;
  current_profile->vibration_enabled = next_level;
}

void SettingsController::incrementVibrationTime() {
  int next_duration = current_profile->vibration_duration + 1;
  if (next_duration > 5) next_duration = 1;

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
      Serial.println(F("Input was cleaned, showing keyboard again"));
    } else {
      // Input is clean, exit loop
      Serial.println(F("Input is clean"));
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
      Serial.println(F("Input was cleaned, showing keyboard again"));
    } else {
      // Input is clean, exit loop
      Serial.println(F("Input is clean"));
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
    Serial.println(F("Cleaning!"));
    SkipUtils::CleanResult result = SkipUtils::clean(current_profile->skip_single_pos, SkipUtils::INDIVIDUAL, dimensions);
    Serial.println(F("Cleaned!"));
    
    // If input was cleaned, update and loop again
    if (result.was_cleaned) {
      Serial.println(F("Actually cleaned!"));
      strncpy(current_profile->skip_single_pos, result.cleaned, ROW_COL_MAX_LEN - 1);
      current_profile->skip_single_pos[ROW_COL_MAX_LEN - 1] = '\0';
      Serial.println(F("Input was cleaned, showing keyboard again"));
    } else {
      // Input is clean, exit loop
      Serial.println(F("Nothing changed!"));
      break;
    }
  }
}

