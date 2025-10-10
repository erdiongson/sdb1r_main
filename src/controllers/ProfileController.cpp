#include "ProfileController.h"
#include "../views/ProfileScreen.h"
#include "../views/common/Keyboards.h"
#include "../logic/Profile.h"
#include "../Constants.h"
#include "../../Config.h"

ProfileController::ProfileController(ControllerParams params)
  : BaseController(params), selectedProfileNum(0) {}

void ProfileController::on_start(Profile& profile) {
  ProfileParams params = {0, 0, CurProf, 0};
  draw_profile_screen(phost, params);
}

void ProfileController::on_interaction(const Interaction& interaction) {
  if (interaction.key_pressed == 0) return;

  static ProfileParams params = {0, 0, CurProf, 0};

  switch (interaction.key_pressed) {

    case TAG_PROFILE_BACK:
      start_next_controller(CONTROLLER_SETTINGS);
      break;

    case TAG_PROFILE_LOAD: {
      CurProfNum = selectedProfileNum;
      WriteCurIDEEPROM(selectedProfileNum);
      ReadProfileEEPROM(selectedProfileNum);
      
      // Show profile loaded dialog
      params.keypressed = 0;
      params.curprofnum = selectedProfileNum;
      params.dialog_code = DIALOG_PROFILE_LOADED;
      draw_profile_screen(phost, params);
      delay(2000);
      start_next_controller(CONTROLLER_SETTINGS);
      break;
    }

    case TAG_PROFILE_UP: {
      if (selectedProfileNum < MAX_PROFILES - 1) {
        ++selectedProfileNum;
      } else {
        selectedProfileNum = 0;
      }

      ReadProfileEEPROM(selectedProfileNum);
      params.keypressed = interaction.key_pressed;
      params.curprofnum = selectedProfileNum;
      params.dialog_code = 0;
      draw_profile_screen(phost, params);
      break;
    }

    case TAG_PROFILE_DOWN: {
      if (selectedProfileNum > 0) {
        --selectedProfileNum;
      } else {
        selectedProfileNum = MAX_PROFILES - 1;
      }
      ReadProfileEEPROM(selectedProfileNum);
      params.keypressed = interaction.key_pressed;
      params.curprofnum = selectedProfileNum;
      params.dialog_code = 0;
      draw_profile_screen(phost, params);
      break;
    }

    case TAG_PROFILE_CHANGE_PASSWORD: {
      char new_password_1[PASSWORD_MAX_LEN] = "";
      char new_password_2[PASSWORD_MAX_LEN] = "";
      
      get_keyboard_value(phost, new_password_1, "Enter New Password", FALSE);
      if (new_password_1[0] == 0) {
        params.keypressed = 0;
        params.curprofnum = selectedProfileNum;
        params.dialog_code = 0;
        draw_profile_screen(phost, params);
        break;
      }

      get_keyboard_value(phost, new_password_2, "Enter New Password again", FALSE);

      if (new_password_2[0] == 0) {
        params.keypressed = 0;
        params.curprofnum = selectedProfileNum;
        params.dialog_code = 0;
        draw_profile_screen(phost, params);
        break;
      }

      if (strcmp(new_password_1, new_password_2) != 0) {
        params.keypressed = 0;
        params.curprofnum = selectedProfileNum;
        params.dialog_code = DIALOG_ERROR_PASSWORD_MISMATCH;
        draw_profile_screen(phost, params);
        delay(2000);
        params.dialog_code = 0;
        draw_profile_screen(phost, params);
        break;
      }

      WritePassEEPROM(new_password_1);
      params.keypressed = 0;
      params.curprofnum = selectedProfileNum;
      params.dialog_code = DIALOG_PASSWORD_CHANGED;
      draw_profile_screen(phost, params);
      delay(2000);
      params.dialog_code = 0;
      draw_profile_screen(phost, params);
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
