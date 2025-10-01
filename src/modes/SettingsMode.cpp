#include "SettingsMode.h"
#include "../views/Config_Screen.h"
#include "../../Config.h"
#include "ModesCommon.h"

SettingsMode::SettingsMode(DispenserHead& head, Gpu_Hal_Context_t* host, ModeController* controller, ModeCompletionCallback callback)
  : BaseMode(head, host, controller, callback), currentProfile(nullptr), specialMode(false) {}

void SettingsMode::on_start(Profile& profile) {
  Serial.println("MODE: Settings mode");

  // Store reference to the current profile
  currentProfile = &profile;

  // Check if special mode is enabled
  specialMode = (digitalRead(Limit_S_y_MAX) == 0);

  // Reset password buffer
  Password[2][0] = 0;

  // Handle password protection if enabled
  if (currentProfile->passwordEnabled) {
    Keyboard(phost, Password[2], "Enter Password", FALSE);
    bool passwordValid = (strcmp(Password[1], Password[2]) == 0 || strcmp(Password[0], Password[2]) == 0);
    if (!passwordValid) {
      DisplayKeyboard(phost, 0, "Wrong Password", " ", FALSE, FALSE, FALSE);
      delay(2000);
      complete_with_next_mode(MODE_TYPE_HOME);
      return;
    }

    if (specialMode) {
      if (digitalRead(Limit_S_y_MAX) != 0) {
        BlankEEPROM();
      } else {
        specialMode = false;
      }
    }

    // Display configuration screen
    Config_Screen(phost);
  }
}

void SettingsMode::on_interaction(const Interaction& interaction) {
  if (interaction.key_pressed == 0) return;

  switch (interaction.key_pressed) {
    case KEY_CONFIG_HOME:  // Home button
      Serial.println("Button Pressed: HOME");
      {
        char buf[PROFILE_NAME_MAX_LEN];
        float maxval = 0;
        bool error = FALSE;

        if (currentProfile->Tube_No_x == 0) maxval = MAXXMM - currentProfile->trayOriginX;
        else maxval = (MAXXMM - currentProfile->trayOriginX) / (currentProfile->Tube_No_x - 1);
        Round1Dec(&maxval);
        if (currentProfile->pitch_x > maxval) error = TRUE;
        else {
          if (currentProfile->Tube_No_y == 0) maxval = MAXYMM - currentProfile->trayOriginY;
          else maxval = (MAXYMM - currentProfile->trayOriginY) / (currentProfile->Tube_No_y - 1);
          Round1Dec(&maxval);
          if (currentProfile->pitch_y > maxval) error = TRUE;
        }

        if (error) {
          strcpy(buf, currentProfile->profileName);
          sprintf(currentProfile->profileName, "Error in entry");
          Config_Screen(phost);
          delay(3000);
          strcpy(currentProfile->profileName, buf);
          Config_Screen(phost);
          delay(3000);
        } else {
          complete_with_next_mode(MODE_TYPE_HOME);
        }
      }
      break;

    case KEY_CONFIG_LOAD:  // Load (Config Screen)
      Serial.println("Button Pressed: LOAD");
      {
        int i;
        if (strcmp(currentProfile->profileName, "xqreset") == 0) {  //special mode to preload eeprom
          Dprint("write preset data to eeprom");
          if (CurProfNum == 0) {
            for (i = 1; i < MAX_PROFILES; i++) {
              sprintf(currentProfile->profileName, "%s %d", "Profile", i + 1);
              WriteProfileEEPROM(i);
            }
          } else PreLoadEEPROM();
          sprintf(currentProfile->profileName, "EEprom reseted");
          Config_Screen(phost);
          delay(3000);
          sprintf(currentProfile->profileName, "Profile 1 ");
          Config_Screen(phost);
        }
        if (strcmp(currentProfile->profileName, "xqver") == 0) {
          Dprint("show version");
          sprintf(currentProfile->profileName, "version : %s", FWVER);
          Config_Screen(phost);
          delay(3000);
          sprintf(currentProfile->profileName, "xqver ");
          Config_Screen(phost);
        }
        if (strcmp(currentProfile->profileName, "xqhome") == 0) {
          Dprint("home");
          Homing();
          Config_Screen(phost);
        }
        if (strcmp(currentProfile->profileName, "xqblank") == 0) {
          Dprint("blankeeprom");
          BlankEEPROM();
          sprintf(currentProfile->profileName, "EEprom blank");
          Config_Screen(phost);
          delay(3000);
          sprintf(currentProfile->profileName, "xqblank ");
          Config_Screen(phost);
          delay(3000);
        }
        if (strcmp(currentProfile->profileName, "xqsize-s") == 0) {
          Dprint("Change the size to small.");
          sprintf(currentProfile->profileName, "Size Change : SMALL");
          currentProfile->sizeFlag = 0;
          Config_Screen(phost);
          delay(3000);
          sprintf(currentProfile->profileName, "xqsize-s ");
          Config_Screen(phost);
        }
        Profile_Menu(&host);
        Config_Screen(phost);
      }
      break;

    case KEY_CONFIG_SAVE:
      Serial.println("Button Pressed: SAVE");
      {
        char buf[PROFILE_NAME_MAX_LEN];
        Dprint("curprofnum=", CurProfNum);
        WriteCurIDEEPROM(CurProfNum);
        WriteProfileEEPROM(CurProfNum);
        strcpy(buf, currentProfile->profileName);
        strcpy(currentProfile->profileName, "Profile saved");
        Config_Screen(phost);
        delay(3000);
        strcpy(currentProfile->profileName, buf);
      }
      break;

    case KEY_CONFIG_PROFILE:
      Serial.println("Button Pressed: PROFILE");
      Keyboard(phost, currentProfile->profileName, "Enter Profile Name", FALSE);
      Config_Screen(phost);
      break;

    case KEY_CONFIG_TUBES_X:  //no. of Tubes row
      {
        float maxval = (int)((MAXXMM - currentProfile->trayOriginX) / currentProfile->pitch_x) + 1;
        if (maxval > MAXNUMX) maxval = MAXNUMX;

        Dprint("max val=", maxval);
        currentProfile->Tube_No_x = Keypad(&host, currentProfile->Tube_No_x, MINNUMX, MAXNUMX, FALSE);
        Config_Screen(phost);
      }
      break;

    case KEY_CONFIG_TUBES_Y:  //no. of Tubes col
      {
        float maxval = (int)((MAXYMM - currentProfile->trayOriginY) / currentProfile->pitch_y) + 1;

        if (maxval > MAXNUMY) maxval = MAXNUMY;
        Dprint("max val=", maxval);
        currentProfile->Tube_No_y = Keypad(&host, currentProfile->Tube_No_y, MINNUMY, MAXNUMY, FALSE);
        Config_Screen(phost);
      }
      break;

    case KEY_CONFIG_PITCH_X:  //pitch row
      {
        float maxval;
        if (currentProfile->Tube_No_x == 0) maxval = MAXXMM - currentProfile->trayOriginX;
        else maxval = (MAXXMM - currentProfile->trayOriginX) / (currentProfile->Tube_No_x - 1);
        Round1Dec(&maxval);
        Dprint("max val=", maxval);
        if (maxval > MAXPITCHX) maxval = MAXPITCHX;
        currentProfile->pitch_x = Keypad(&host, currentProfile->pitch_x, MINPITCHX, MAXPITCHX, FALSE);
        Config_Screen(phost);
      }
      break;

    case KEY_CONFIG_PITCH_Y:  //pitch col
      {
        float maxval;
        if (currentProfile->Tube_No_y == 0) maxval = MAXYMM - currentProfile->trayOriginY;
        else maxval = (MAXYMM - currentProfile->trayOriginY) / (currentProfile->Tube_No_y - 1);
        Round1Dec(&maxval);
        if (maxval > MAXPITCHY) maxval = MAXPITCHY;
        Dprint("max val=", maxval);
        currentProfile->pitch_y = Keypad(&host, currentProfile->pitch_y, MINPITCHY, MAXPITCHY, FALSE);
        Config_Screen(phost);
      }
      break;

    case KEY_CONFIG_ORIGIN_X:  //origin row
      {
        float maxval;
        if (currentProfile->Tube_No_x == 0) maxval = MAXXMM;
        else maxval = MAXXMM - (currentProfile->pitch_x * (currentProfile->Tube_No_x - 1));
        Round1Dec(&maxval);
        if (maxval > MAXORGX) maxval = MAXORGX;
        Dprint("max val=", maxval);

        currentProfile->trayOriginX = Keypad(&host, currentProfile->trayOriginX, 0, MAXORGX, FALSE);
        Config_Screen(phost);
      }
      break;

    case KEY_CONFIG_ORIGIN_Y:  //origin col
      {
        float maxval;
        if (currentProfile->Tube_No_y == 0) maxval = MAXYMM;
        else maxval = MAXYMM - (currentProfile->pitch_y * (currentProfile->Tube_No_y - 1));
        Round1Dec(&maxval);
        if (maxval > MAXORGY) maxval = MAXORGY;
        Dprint("max val=", maxval);
        currentProfile->trayOriginY = Keypad(&host, currentProfile->trayOriginY, 0, MAXORGY, FALSE);
        Config_Screen(phost);
      }
      break;

    case ZDIP:
      Serial.println("Incrementing Z Dip");
      currentProfile->ZDip = Keypad(phost, currentProfile->ZDip, MINZDIP, MAXZDIP, FALSE);
      break;

    case VIBLVL:
      Serial.println("Incrementing vibration level");
      increment_vibration_level();
      break;

    case VIBDURATION:
      Serial.println("Incrementing vibration duration");
      increment_vibration_time();
      break;

    case PASSEN:
      Serial.println("Toggle password enable");
      currentProfile->passwordEnabled = !currentProfile->passwordEnabled;
      break;

    case SKIP_COLUMNS:
      Serial.println("Button Pressed: SKIP COLUMNS");
      Keyboard(phost, currentProfile->skipCol, "Enter columns to skip", FALSE);
      Skip_Screen(phost);
      break;

    case SKIP_ROWS:
      Serial.println("Button Pressed: SKIP ROWS");
      Keyboard(phost, currentProfile->skipRow, "Enter rows to skip", FALSE);
      Skip_Screen(phost);
      break;

    case SKIP_SINGLE_POS:
      Serial.println("Button Pressed: SKIP SINGLE POSITION");
      Keyboard(phost, currentProfile->skipSinglePos, "Enter positions to skip", FALSE);
      Skip_Screen(phost);
      break;

    case ADVPROF_BACK:  // Back button
      Serial.println("Button Pressed: BACK");
      break;

    case ADVPROF_SAVE:
      Serial.println("Button Pressed: SAVE");
      {
        char buf[PROFILE_NAME_MAX_LEN];
        Dprint("curprofnum=", CurProfNum);
        WriteCurIDEEPROM(CurProfNum);
        WriteProfileEEPROM(CurProfNum);
        strcpy(buf, currentProfile->profileName);
        strcpy(currentProfile->profileName, "Profile saved");
        Config_Screen(phost);
        delay(3000);
        strcpy(currentProfile->profileName, buf);
      }
      break;

      //case 246: // No button for Advanced Setting
      // Config_Screen(phost);
      // break;

      //case 247: // Yes button for Advanced Setting
      // Tray_Screen(phost, CurProf);
      // break;

    default:
      break;
  }
}

int SettingsMode::on_step() {
  return MODE_CONTINUE;
}

int SettingsMode::get_mode_type() const {
  return MODE_TYPE_SETTINGS;
}

void SettingsMode::increment_vibration_level() {
  int next_level = currentProfile->vibrationEnabled + 1;
  if (next_level > 4) next_level = 0;

  currentProfile->vibrationEnabled = next_level;
  dispenserHead.set_vibration_level(next_level);
}

void SettingsMode::increment_vibration_time() {
  int next_duration = currentProfile->vibrationDuration + 1;
  if (next_duration > 5) next_duration = 1;

  currentProfile->vibrationDuration = next_duration;
  dispenserHead.set_vibration_time(next_duration);
}
