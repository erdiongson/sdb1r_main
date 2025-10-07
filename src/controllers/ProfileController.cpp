#include "ProfileController.h"
#include "../views/ProfileScreen.h"
#include "../Constants.h"
#include "../../Config.h"
#include "../../Profile.h"
#include "../gpu/App_Common.h"

ProfileController::ProfileController(ControllerParams params)
  : BaseController(params), selectedProfileNum(0) {}

void ProfileController::on_start(Profile& profile) {
  DisplayProfileMenu(0, 0, CurProf);
}

void ProfileController::on_interaction(const Interaction& interaction) {
  if (interaction.key_pressed == 0) return;

  switch (interaction.key_pressed) {

    case TAG_PROFILE_BACK:
      start_next_controller(CONTROLLER_CONFIG);
      break;

    case TAG_PROFILE_LOAD:
      CurProfNum = selectedProfileNum;
      WriteCurIDEEPROM(selectedProfileNum);
      ReadProfileEEPROM(selectedProfileNum);
      break;

    case TAG_PROFILE_UP:
      if (selectedProfileNum < MAX_PROFILES - 1) {
        ++selectedProfileNum;
      } else {
        selectedProfileNum = 0;
      }
      ReadProfileEEPROM(selectedProfileNum);
      DisplayProfileMenu(interaction.key_pressed, selectedProfileNum, CurProf);
      break;

    case TAG_PROFILE_DOWN:
      if (selectedProfileNum > 0) {
        --selectedProfileNum;
      } else {
        selectedProfileNum = MAX_PROFILES - 1;
      }
      ReadProfileEEPROM(selectedProfileNum);
      DisplayProfileMenu(interaction.key_pressed, selectedProfileNum, CurProf);
      break;

    case TAG_PROFILE_CHANGE_PASSWORD:
      char new_password_1[PASSWORD_MAX_LEN];
      char new_password_2[PASSWORD_MAX_LEN];
      
      Keyboard(phost, new_password_1, "Enter New Password", FALSE);
      if (new_password_1[0] == 0) {
        Keyboard(phost, "Error : No password entered", "Press Back to continue", FALSE);
        return;
      }

      Keyboard(phost, new_password_2, "Enter New Password again", FALSE);

      if (strcmp(new_password_1, new_password_2) != 0) {
        Keyboard(phost, "Error : Different password entered", "Press Back to continue", FALSE);
        return;
      }

      WritePassEEPROM(new_password_1);
      Keyboard(phost, "Password changed", "Press Back to continue", FALSE);
      break;

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
