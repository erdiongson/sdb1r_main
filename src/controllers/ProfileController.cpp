#include "ProfileController.h"
#include "../views/ProfileScreen.h"
#include "../views/Keyboards.h"
#include "../Constants.h"
#include "../../Config.h"
#include "../../Profile.h"
#include "../gpu/App_Common.h"

ProfileController::ProfileController(ControllerParams params)
  : BaseController(params), selectedProfileNum(0) {}

void ProfileController::on_start(Profile& profile) {
  ProfileParams params = {0, 0, CurProf, 0};
  draw_profile_screen(&params);
}

void ProfileController::on_interaction(const Interaction& interaction) {
  if (interaction.key_pressed == 0) return;

  switch (interaction.key_pressed) {

    case TAG_PROFILE_BACK:
      start_next_controller(CONTROLLER_CONFIG);
      break;

    case TAG_PROFILE_LOAD: {
      CurProfNum = selectedProfileNum;
      WriteCurIDEEPROM(selectedProfileNum);
      ReadProfileEEPROM(selectedProfileNum);
      
      // Show profile loaded dialog
      ProfileParams params = {0, selectedProfileNum, CurProf, DIALOG_PROFILE_LOADED};
      draw_profile_screen(&params);
      delay(2000);
      start_next_controller(CONTROLLER_CONFIG);
      break;
    }

    case TAG_PROFILE_UP: {
      if (selectedProfileNum < MAX_PROFILES - 1) {
        ++selectedProfileNum;
      } else {
        selectedProfileNum = 0;
      }

      ReadProfileEEPROM(selectedProfileNum);
      ProfileParams params1 = {interaction.key_pressed, selectedProfileNum, CurProf, 0};
      draw_profile_screen(&params1);
      break;
    }

    case TAG_PROFILE_DOWN: {
      if (selectedProfileNum > 0) {
        --selectedProfileNum;
      } else {
        selectedProfileNum = MAX_PROFILES - 1;
      }
      ReadProfileEEPROM(selectedProfileNum);
      ProfileParams params2 = {interaction.key_pressed, selectedProfileNum, CurProf, 0};
      draw_profile_screen(&params2);
      break;
    }

    case TAG_PROFILE_CHANGE_PASSWORD: {
      char new_password_1[PASSWORD_MAX_LEN] = "";
      char new_password_2[PASSWORD_MAX_LEN] = "";
      
      get_keyboard_value(phost, new_password_1, "Enter New Password", FALSE);
      if (new_password_1[0] == 0) {
        get_keyboard_value(phost, "Error : No password entered", "Press Back to continue", FALSE);
        ProfileParams params3 = {0, selectedProfileNum, CurProf, 0};
        draw_profile_screen(&params3);
        break;
      }

      get_keyboard_value(phost, new_password_2, "Enter New Password again", FALSE);

      if (strcmp(new_password_1, new_password_2) != 0) {
        get_keyboard_value(phost, "Error : Different password entered", "Press Back to continue", FALSE);
        Serial.println("DEFINING PARAM");
        delay(100);
        ProfileParams params4 = {0, selectedProfileNum, CurProf, 0};
        Serial.println("DRAWING SCREEN");
        delay(100);
        draw_profile_screen(&params4);
        Serial.println("SCREEN DRAWN");
        delay(100);
        break;
      }

      WritePassEEPROM(new_password_1);
      get_keyboard_value(phost, "Password changed", "Press Back to continue", FALSE);
      ProfileParams params5 = {0, selectedProfileNum, CurProf, 0};
      draw_profile_screen(&params5);
      break;
    }

    default:
      break;
  }
}

ControllerStepResult ProfileController::on_step() {
  return ControllerStepResult(-1, -1);
}

int ProfileController::get_mode_type() const {
  return CONTROLLER_PROFILE;
}
