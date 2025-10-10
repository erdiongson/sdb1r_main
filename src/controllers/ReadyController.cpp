#include "ReadyController.h"
#include "../views/MainScreen.h"
#include "../views/common/Keyboards.h"
#include "../Constants.h"
#include "../../Config.h"
#include "../Utils.h"

ReadyController::ReadyController(ControllerParams params)
  : BaseController(params) {}

void ReadyController::on_start(Profile& profile) {
  Dprint(F("ReadyController::on_start"));
  draw_main_screen(phost, MAINMENU);

  // Home the dispenser head
}

void ReadyController::on_interaction(const Interaction& interaction) {
  // Handle key presses
  switch (interaction.key_pressed) {

    case START:
      Dprint(F("ReadyController::on_interaction: Transitioning to start"));
      start_next_controller(CONTROLLER_RUN);
      return;

    case SETTING:
      // Handle password protection if enabled
      if (CurProf.passwordEnabled) {
        char currentPassword [PROFILE_NAME_MAX_LEN] = "";
        char inputPassword [PROFILE_NAME_MAX_LEN] = "";

        ReadPassEEPROM(currentPassword);
        if (strcmp(currentPassword, "") == 0) strcpy(currentPassword, INITIAL_PASSWORD);
        Serial.println("Current password:" + String(currentPassword));

        get_keyboard_value(phost, inputPassword, "Enter Password", FALSE);

        // Cancelled
        if (strcmp(inputPassword, "") == 0) {
          draw_main_screen(phost, MAINMENU);
          return;
        }

        // Attempted
        bool passwordValid = (strcmp(currentPassword, inputPassword) == 0 || strcmp(SUPER_PASSWORD, inputPassword) == 0);
        if (!passwordValid) {
          MainScreenParams params = {0, 0, 0, 0, DIALOG_ERROR_WRONG_PASSWORD};
          draw_main_screen(phost, MAINMENU, &params);
          delay(2000);
          draw_main_screen(phost, MAINMENU);
          return;
        }
      }

      Dprint(F("ReadyController::on_interaction: Transitioning to settings"));
      start_next_controller(CONTROLLER_SETTINGS);
      return;
    
    default:
      break;
  }

  // Handle PLC messages
  switch (interaction.plc_message_type) {
    case MSG_START:
      Dprint(F("ReadyController::on_interaction: Transitioning to start"));
      start_next_controller(CONTROLLER_RUN);
      break;
    
    case MSG_RAISE_Z:
      Dprint(F("ReadyController::on_interaction: Raising Z"));
      dispenserHead.z().moveBy(-interaction.plc_message_data * STEPS_PER_UNIT_Z);
      break;
    
    case MSG_LOWER_Z:
      Dprint(F("ReadyController::on_interaction: Lowering Z"));
      dispenserHead.z().moveBy(interaction.plc_message_data * STEPS_PER_UNIT_Z);
      break;
    
    default:
      break;
  }
}

ControllerStepResult ReadyController::on_step() {
  DispenserProcessResult result = dispenserHead.process();
  return ControllerStepResult(result.steppers, result.dispenser);
}

int ReadyController::get_mode_type() const {
  return CONTROLLER_READY;
}