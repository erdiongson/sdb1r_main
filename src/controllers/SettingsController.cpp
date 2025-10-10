#include "../gpu/Platform.h"
#include "../../Config.h"
#include "../views/SettingsScreen.h"
#include "../views/PreviewScreen.h"
#include "../views/common/Keyboards.h"
#include "../logic/SkipUtils.h"
#include "SettingsController.h"
#include "../Utils.h"

SettingsController::SettingsController(ControllerParams params)
  : BaseController(params), currentProfile(nullptr), 
    simulating(false), lastSimulationTime(0), simulateCol(0), simulateRow(0) {}

void SettingsController::on_start(Profile& profile) {
  Serial.println(F("MODE: Config mode"));

  // Store reference to the current profile
  currentProfile = &profile;

  // Initialize simulation state
  simulating = false;
  simulateCol = 0;
  simulateRow = 0;
  lastSimulationTime = 0;
  
  // Load profile into simulation handler
  simulationHandler.load_profile(*currentProfile);
  simulationHandler.reset();

  // Display configuration screen
  draw_settings_screen(phost);
}

void SettingsController::on_interaction(const Interaction& interaction) {
  if (interaction.key_pressed == 0) return;

  switch (interaction.key_pressed) {
    case TAG_CONFIG_HOME:  // Home button
      Serial.println(F("Button Pressed: HOME"));
      {
        char buf[PROFILE_NAME_MAX_LEN];
        float maxval = 0;
        bool error = FALSE;

        if (currentProfile->Tube_No_x == 0) maxval = MAXXMM - currentProfile->trayOriginX;
        else maxval = (MAXXMM - currentProfile->trayOriginX) / (currentProfile->Tube_No_x - 1);
        round_1_decimal(&maxval);
        if (currentProfile->pitch_x > maxval) error = TRUE;
        else {
          if (currentProfile->Tube_No_y == 0) maxval = MAXYMM - currentProfile->trayOriginY;
          else maxval = (MAXYMM - currentProfile->trayOriginY) / (currentProfile->Tube_No_y - 1);
          round_1_decimal(&maxval);
          if (currentProfile->pitch_y > maxval) error = TRUE;
        }

        if (error) {
          strcpy(buf, currentProfile->profileName);
          sprintf(currentProfile->profileName, "Error in entry");
          draw_settings_screen(phost);
          delay(3000);
          strcpy(currentProfile->profileName, buf);
          draw_settings_screen(phost);
          delay(3000);
        } else {
          start_next_controller(CONTROLLER_READY);
        }
      }
      break;

    case TAG_CONFIG_LOAD:  // Load (Config Screen)
      Serial.println(F("Button Pressed: LOAD"));
      start_next_controller(CONTROLLER_PROFILE);
      break;

    case TAG_CONFIG_SAVE: {
      Serial.println("Button Pressed: SAVE");
      Dprint("curprofnum=", CurProfNum);
      WriteCurIDEEPROM(CurProfNum);
      WriteProfileEEPROM(CurProfNum);
      
      // Show profile saved dialog
      SettingsScreenParams params = {DIALOG_PROFILE_SAVED};
      draw_settings_screen(phost, params);
      delay(2000);
      params.dialog_code = 0;
      draw_settings_screen(phost, params);
      break;
    }

    case TAG_CONFIG_PROFILE_NAME: {
      Serial.println(F("Button Pressed: PROFILE"));
        char buf[PROFILE_NAME_MAX_LEN];
        strcpy(buf, currentProfile->profileName);
        get_keyboard_value(phost, buf, "Enter Profile Name", FALSE);
        
        if (strcmp(buf, "debug") == 0) {
          start_next_controller(CONTROLLER_DEBUG);
          return;
        }

        strcpy(currentProfile->profileName, buf);
        draw_settings_screen(phost);
      break;
    }

    case TAG_CONFIG_TUBES_X:  // Number of Columns
      {
        float maxval = (int)((MAXXMM - currentProfile->trayOriginX) / currentProfile->pitch_x) + 1;
        if (maxval > MAXNUMX) maxval = MAXNUMX;

        int oldTubeNoX = currentProfile->Tube_No_x;
        currentProfile->Tube_No_x = get_keypad_value(&host, currentProfile->Tube_No_x, MINNUMX, MAXNUMX, FALSE);
        
        // If rows decreased, clean skip strings to remove out-of-bounds positions
        if (currentProfile->Tube_No_x < oldTubeNoX) {
          TrayHandler::Dimensions dimensions(currentProfile->Tube_No_x, currentProfile->Tube_No_y);
          
          // Clean skip rows
          SkipUtils::CleanResult rowResult = SkipUtils::clean(currentProfile->skipRow, SkipUtils::ROW, dimensions);
          if (rowResult.wasCleaned) {
            strncpy(currentProfile->skipRow, rowResult.cleaned, ROW_COL_MAX_LEN - 1);
            currentProfile->skipRow[ROW_COL_MAX_LEN - 1] = '\0';
          }
          
          // Clean skip individual positions
          SkipUtils::CleanResult posResult = SkipUtils::clean(currentProfile->skipSinglePos, SkipUtils::INDIVIDUAL, dimensions);
          if (posResult.wasCleaned) {
            strncpy(currentProfile->skipSinglePos, posResult.cleaned, ROW_COL_MAX_LEN - 1);
            currentProfile->skipSinglePos[ROW_COL_MAX_LEN - 1] = '\0';
          }
        }
        
        draw_settings_screen(phost);
      }
      break;

    case TAG_CONFIG_TUBES_Y:  // Number of Rows
      {
        float maxval = (int)((MAXYMM - currentProfile->trayOriginY) / currentProfile->pitch_y) + 1;

        if (maxval > MAXNUMY) maxval = MAXNUMY;
        int oldTubeNoY = currentProfile->Tube_No_y;
        currentProfile->Tube_No_y = get_keypad_value(&host, currentProfile->Tube_No_y, MINNUMY, MAXNUMY, FALSE);
        
        // If columns decreased, clean skip strings to remove out-of-bounds positions
        if (currentProfile->Tube_No_y < oldTubeNoY) {
          TrayHandler::Dimensions dimensions(currentProfile->Tube_No_x, currentProfile->Tube_No_y);
          
          // Clean skip columns
          SkipUtils::CleanResult colResult = SkipUtils::clean(currentProfile->skipCol, SkipUtils::COLUMN, dimensions);
          if (colResult.wasCleaned) {
            strncpy(currentProfile->skipCol, colResult.cleaned, ROW_COL_MAX_LEN - 1);
            currentProfile->skipCol[ROW_COL_MAX_LEN - 1] = '\0';
          }
          
          // Clean skip individual positions
          SkipUtils::CleanResult posResult = SkipUtils::clean(currentProfile->skipSinglePos, SkipUtils::INDIVIDUAL, dimensions);
          if (posResult.wasCleaned) {
            strncpy(currentProfile->skipSinglePos, posResult.cleaned, ROW_COL_MAX_LEN - 1);
            currentProfile->skipSinglePos[ROW_COL_MAX_LEN - 1] = '\0';
          }
        }
        
        draw_settings_screen(phost);
      }
      break;

    case TAG_CONFIG_PITCH_X:  //pitch row
      {
        float maxval;
        if (currentProfile->Tube_No_x == 0) maxval = MAXXMM - currentProfile->trayOriginX;
        else maxval = (MAXXMM - currentProfile->trayOriginX) / (currentProfile->Tube_No_x - 1);
        round_1_decimal(&maxval);
        Dprint("max val=", maxval);
        if (maxval > MAXPITCHX) maxval = MAXPITCHX;
        currentProfile->pitch_x = get_keypad_value(&host, currentProfile->pitch_x, MINPITCHX, MAXPITCHX, TRUE);
        draw_settings_screen(phost);
      }
      break;

    case TAG_CONFIG_PITCH_Y:  //pitch col
      {
        float maxval;
        if (currentProfile->Tube_No_y == 0) maxval = MAXYMM - currentProfile->trayOriginY;
        else maxval = (MAXYMM - currentProfile->trayOriginY) / (currentProfile->Tube_No_y - 1);
        round_1_decimal(&maxval);
        if (maxval > MAXPITCHY) maxval = MAXPITCHY;
        Dprint("max val=", maxval);
        currentProfile->pitch_y = get_keypad_value(&host, currentProfile->pitch_y, MINPITCHY, MAXPITCHY, TRUE);
        draw_settings_screen(phost);
      }
      break;

    case TAG_CONFIG_ORIGIN_X:  //origin row
      {
        float maxval;
        if (currentProfile->Tube_No_x == 0) maxval = MAXXMM;
        else maxval = MAXXMM - (currentProfile->pitch_x * (currentProfile->Tube_No_x - 1));
        round_1_decimal(&maxval);
        if (maxval > MAXORGX) maxval = MAXORGX;
        Dprint("max val=", maxval);

        currentProfile->trayOriginX = get_keypad_value(&host, currentProfile->trayOriginX, 0, MAXORGX, TRUE);
        draw_settings_screen(phost);
      }
      break;

    case TAG_CONFIG_ORIGIN_Y:  //origin col
      {
        float maxval;
        if (currentProfile->Tube_No_y == 0) maxval = MAXYMM;
        else maxval = MAXYMM - (currentProfile->pitch_y * (currentProfile->Tube_No_y - 1));
        round_1_decimal(&maxval);
        if (maxval > MAXORGY) maxval = MAXORGY;
        Dprint("max val=", maxval);
        currentProfile->trayOriginY = get_keypad_value(&host, currentProfile->trayOriginY, 0, MAXORGY, TRUE);
        draw_settings_screen(phost);
      }
      break;

    case TAG_NUM_CYCLE:
      currentProfile->Cycles = get_keypad_value(phost, currentProfile->Cycles, MINCYCLE, MAXCYCLE, FALSE);
      draw_settings_screen(phost);
      break;

    case TAG_Z_DIP:
      Serial.println(F("Incrementing Z Dip"));
      currentProfile->ZDip = get_keypad_value(phost, currentProfile->ZDip, MINZDIP, MAXZDIP, TRUE);
      draw_settings_screen(phost);
      break;

    case TAG_VIBRATION_LEVEL:
      Serial.println(F("Incrementing vibration level"));
      increment_vibration_level();
      draw_settings_screen(phost);
      break;

    case TAG_VIBRATION_DURATION:
      Serial.println(F("Incrementing vibration duration"));
      increment_vibration_time();
      draw_settings_screen(phost);
      break;

    case TAG_PASSWORD_ENABLED:
      Serial.println(F("Toggle password enable"));
      currentProfile->passwordEnabled = !currentProfile->passwordEnabled;
      draw_settings_screen(phost);
      break;

    case TAG_STAGGERED_TOGGLE:
      Serial.println(F("Toggle staggered mode"));
      currentProfile->staggered = !currentProfile->staggered;
      draw_skip_screen(phost);
      break;

    case TAG_SKIP_COLUMNS:
      Serial.println(F("Button Pressed: SKIP COLUMNS"));
      editSkipColumn(phost);
      draw_skip_screen(phost);
      break;

    case TAG_SKIP_ROWS:
      Serial.println(F("Button Pressed: SKIP ROWS"));
      editSkipRow(phost);
      draw_skip_screen(phost);
      break;

    case TAG_SKIP_SINGLE_POS:
      Serial.println(F("Button Pressed: SKIP SINGLE POSITION"));
      editSkipIndividual(phost);
      draw_skip_screen(phost);
      break;

    case TAG_ADV_PROF_BACK:  // Back button
      Serial.println(F("Button Pressed: BACK"));
      draw_settings_screen(phost);
      break;

    case TAG_CONFIG_PREVIEW:
      Serial.println(F("Button Pressed: PREVIEW"));
      {
        // Parse skip positions from the current profile
        TrayHandler::TrayPositionHandler tempHandler;
        tempHandler.load_profile(*currentProfile);
        
        // Get the skip positions
        TrayHandler::Position skipPositions[MAX_POSITIONS];
        tempHandler.getSkipPositions(skipPositions);
       
        // Create preview screen parameters
        PreviewScreenParams params;
        params.gridCols = currentProfile->Tube_No_x;
        params.gridRows = currentProfile->Tube_No_y;
        params.staggered = currentProfile->staggered;
        params.simulating = false;
        params.simulateCol = 0;
        params.simulateRow = 0;

        snprintf(previewInfoText, sizeof(previewInfoText), "Preview (Grid %dx%d)", params.gridCols, params.gridRows);
        params.infoText = previewInfoText;
        
        // Display the preview screen
        draw_preview_screen(phost, skipPositions, params);
      }
      break;

    case TAG_CONFIG_PREVIEW_BACK:
      Serial.println(F("Button Pressed: PREVIEW BACK"));
      // Stop simulation when leaving preview
      simulating = false;
      simulateCol = 0;
      simulateRow = 0;
      draw_skip_screen(phost);
      break;

    case TAG_PREVIEW_SIMULATE:
      Serial.println(F("Button Pressed: SIMULATE"));
      start_simulation();
      break;

    case TAG_PREVIEW_STOP:
      Serial.println(F("Button Pressed: STOP"));
      end_simulation();
      break;

      case TAG_ADVANCED:
        Serial.println(F("Button Pressed: ADVANCED"));
        draw_skip_screen(phost);
        break;

    default:
      break;
  }
}

ControllerStepResult SettingsController::on_step() {
  // Handle simulation updates every 600 ms
  if (simulating) {
    unsigned long currentTime = millis();
    if (currentTime - lastSimulationTime >= 600) {
      lastSimulationTime = currentTime;
      step_simulation();
    }
  }
  
  DispenserProcessResult result = dispenserHead.process();
  return ControllerStepResult(result.steppers, result.dispenser);
}

int SettingsController::get_mode_type() const {
  return CONTROLLER_SETTINGS;
}

void SettingsController::increment_vibration_level() {
  int next_level = currentProfile->vibrationEnabled + 1;
  if (next_level > 4) next_level = 0;

  currentProfile->vibrationEnabled = next_level;
}

void SettingsController::increment_vibration_time() {
  int next_duration = currentProfile->vibrationDuration + 1;
  if (next_duration > 5) next_duration = 1;

  currentProfile->vibrationDuration = next_duration;
}

void SettingsController::editSkipColumn(Gpu_Hal_Context_t* phost) {
  // Create dimensions from profile
  TrayHandler::Dimensions dimensions(currentProfile->Tube_No_x, currentProfile->Tube_No_y);
  
  while (true) {
    get_keyboard_value(phost, currentProfile->skipCol, "Enter columns to skip", FALSE);
    
    // Clean the input with bounds checking
    SkipUtils::CleanResult result = SkipUtils::clean(currentProfile->skipCol, SkipUtils::COLUMN, dimensions);
    
    // If input was cleaned, update and loop again
    if (result.wasCleaned) {
      strncpy(currentProfile->skipCol, result.cleaned, ROW_COL_MAX_LEN - 1);
      currentProfile->skipCol[ROW_COL_MAX_LEN - 1] = '\0';
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
  TrayHandler::Dimensions dimensions(currentProfile->Tube_No_x, currentProfile->Tube_No_y);
  
  while (true) {
    get_keyboard_value(phost, currentProfile->skipRow, "Enter rows to skip", FALSE);
    
    // Clean the input with bounds checking
    SkipUtils::CleanResult result = SkipUtils::clean(currentProfile->skipRow, SkipUtils::ROW, dimensions);
    
    // If input was cleaned, update and loop again
    if (result.wasCleaned) {
      strncpy(currentProfile->skipRow, result.cleaned, ROW_COL_MAX_LEN - 1);
      currentProfile->skipRow[ROW_COL_MAX_LEN - 1] = '\0';
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
  TrayHandler::Dimensions dimensions(currentProfile->Tube_No_x, currentProfile->Tube_No_y);
  
  while (true) {
    get_keyboard_value(phost, currentProfile->skipSinglePos, "Enter positions to skip", FALSE);
    
    // Clean the input with bounds checking
    Serial.println(F("Cleaning!"));
    SkipUtils::CleanResult result = SkipUtils::clean(currentProfile->skipSinglePos, SkipUtils::INDIVIDUAL, dimensions);
    Serial.println(F("Cleaned!"));
    
    // If input was cleaned, update and loop again
    if (result.wasCleaned) {
      Serial.println(F("Actually cleaned!"));
      strncpy(currentProfile->skipSinglePos, result.cleaned, ROW_COL_MAX_LEN - 1);
      currentProfile->skipSinglePos[ROW_COL_MAX_LEN - 1] = '\0';
      Serial.println(F("Input was cleaned, showing keyboard again"));
    } else {
      // Input is clean, exit loop
      Serial.println(F("Nothing changed!"));
      break;
    }
  }
}

void SettingsController::start_simulation() {
  // Start simulation
  simulating = true;
  lastSimulationTime = millis();
  
  // Reset handler and get first position
  simulationHandler.load_profile(*currentProfile);
  TrayHandler::Position firstPosition = simulationHandler.reset();

  if (firstPosition.x == -1 || firstPosition.y == -1) {
    Serial.println(F("No valid positions found, ending simulation"));
    end_simulation();
    return;
  }

  simulateCol = firstPosition.x;
  simulateRow = firstPosition.y;
  
  // Redraw preview screen
  TrayHandler::Position skipPositions[MAX_POSITIONS];
  simulationHandler.getSkipPositions(skipPositions);
  
  PreviewScreenParams params;
  params.gridCols = currentProfile->Tube_No_x;
  params.gridRows = currentProfile->Tube_No_y;
  params.simulating = simulating;
  params.simulateCol = simulateCol;
  params.simulateRow = simulateRow;
  params.staggered = currentProfile->staggered;
  
  // Set info text for simulation
  snprintf(previewInfoText, sizeof(previewInfoText), "Position = %dx%d", simulateCol, simulateRow);
  params.infoText = previewInfoText;
  
  draw_preview_screen(phost, skipPositions, params);
}

void SettingsController::end_simulation() {
  // Stop simulation
  simulating = false;
  simulateCol = 0;
  simulateRow = 0;
  
  // Redraw preview screen
  TrayHandler::Position skipPositions[MAX_POSITIONS];
  simulationHandler.getSkipPositions(skipPositions);
  
  PreviewScreenParams params;
  params.gridCols = currentProfile->Tube_No_x;
  params.gridRows = currentProfile->Tube_No_y;
  params.simulating = simulating;
  params.simulateCol = simulateCol;
  params.simulateRow = simulateRow;
  params.staggered = currentProfile->staggered;
  
  // Set info text for non-simulation
  snprintf(previewInfoText, sizeof(previewInfoText), "Preview (Grid %dx%d)", params.gridCols, params.gridRows);
  params.infoText = previewInfoText;
  
  draw_preview_screen(phost, skipPositions, params);
}

void SettingsController::step_simulation() {
  // Get next position
  TrayHandler::PositionResult result = simulationHandler.goToNextValidPosition();
  
  if (result.hasNext) {
    simulateCol = result.position.x;
    simulateRow = result.position.y;
  } else {
    end_simulation();
    return;
  }
  
  // Redraw preview screen with updated position
  TrayHandler::Position skipPositions[MAX_POSITIONS];
  simulationHandler.getSkipPositions(skipPositions);
  
  PreviewScreenParams params;
  params.gridCols = currentProfile->Tube_No_x;
  params.gridRows = currentProfile->Tube_No_y;
  params.simulating = simulating;
  params.simulateCol = simulateCol;
  params.simulateRow = simulateRow;
  params.staggered = currentProfile->staggered;
  
  // Set info text for simulation
  snprintf(previewInfoText, sizeof(previewInfoText), "Position = %dx%d", simulateCol, simulateRow);
  params.infoText = previewInfoText;
  
  draw_preview_screen(phost, skipPositions, params);
}

