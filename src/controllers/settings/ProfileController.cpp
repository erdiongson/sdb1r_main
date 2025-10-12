#include "ProfileController.h"
#include "../../views/settings/ProfileScreen.h"
#include "../../views/common/Keyboards.h"

ProfileController::ProfileController(ControllerParams params)
  : BaseController(params), selected_profile_num(0) {}

void ProfileController::onStart() {
  ProfileParams params = {0, 0, profile_manager.getCurrentProfile(), 0};
  drawProfileScreen(phost, params);
}

void ProfileController::onInteraction(const Interaction& interaction) {
  if (interaction.key_pressed == 0) return;

  static ProfileParams params = {0, 0, profile_manager.getCurrentProfile(), 0};

  switch (interaction.key_pressed) {

    case TAG_PROFILE_BACK:
      startNextController(CONTROLLER_SETTINGS);
      break;

    case TAG_PROFILE_LOAD: {
      profile_manager.setCurrentProfileNum(selected_profile_num);
      profile_manager.writeCurIDEEPROM(selected_profile_num);
      profile_manager.readProfileEEPROM(selected_profile_num);
      
      // Show profile loaded dialog
      params.keypressed = 0;
      params.cur_prof_num = selected_profile_num;
      params.dialog_code = DIALOG_PROFILE_LOADED;
      drawProfileScreen(phost, params);
      delay(2000);
      startNextController(CONTROLLER_SETTINGS);
      break;
    }

    case TAG_PROFILE_UP: {
      if (selected_profile_num < MAX_PROFILES - 1) {
        ++selected_profile_num;
      } else {
        selected_profile_num = 0;
      }

      profile_manager.readProfileEEPROM(selected_profile_num);
      params.keypressed = interaction.key_pressed;
      params.cur_prof_num = selected_profile_num;
      params.dialog_code = 0;
      drawProfileScreen(phost, params);
      break;
    }

    case TAG_PROFILE_DOWN: {
      if (selected_profile_num > 0) {
        --selected_profile_num;
      } else {
        selected_profile_num = MAX_PROFILES - 1;
      }
      profile_manager.readProfileEEPROM(selected_profile_num);
      params.keypressed = interaction.key_pressed;
      params.cur_prof_num = selected_profile_num;
      params.dialog_code = 0;
      drawProfileScreen(phost, params);
      break;
    }

    case TAG_PROFILE_CHANGE_PASSWORD: {
      char new_password_1[PASSWORD_MAX_LEN] = "";
      char new_password_2[PASSWORD_MAX_LEN] = "";
      
      getKeyboardValue(phost, new_password_1, "Enter New Password", FALSE);
      if (new_password_1[0] == 0) {
        params.keypressed = 0;
        params.cur_prof_num = selected_profile_num;
        params.dialog_code = 0;
        drawProfileScreen(phost, params);
        break;
      }

      getKeyboardValue(phost, new_password_2, "Enter New Password again", FALSE);

      if (new_password_2[0] == 0) {
        params.keypressed = 0;
        params.cur_prof_num = selected_profile_num;
        params.dialog_code = 0;
        drawProfileScreen(phost, params);
        break;
      }

      if (strcmp(new_password_1, new_password_2) != 0) {
        params.keypressed = 0;
        params.cur_prof_num = selected_profile_num;
        params.dialog_code = DIALOG_ERROR_PASSWORD_MISMATCH;
        drawProfileScreen(phost, params);
        delay(2000);
        params.dialog_code = 0;
        drawProfileScreen(phost, params);
        break;
      }

      profile_manager.writePassEEPROM(new_password_1);
      params.keypressed = 0;
      params.cur_prof_num = selected_profile_num;
      params.dialog_code = DIALOG_PASSWORD_CHANGED;
      drawProfileScreen(phost, params);
      delay(2000);
      params.dialog_code = 0;
      drawProfileScreen(phost, params);
      break;
    }

    default:
      break;
  }
}

ControllerStepResult ProfileController::onStep() {
  return ControllerStepResult(false);
}

int ProfileController::getModeType() const {
  return CONTROLLER_PROFILE;
}
