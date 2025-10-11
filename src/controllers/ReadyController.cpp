#include "ReadyController.h"
#include "../views/MainScreen.h"
#include "../logic/Profile.h"
#include "../Constants.h"
#include "../../Config.h"
#include "../Utils.h"

ReadyController::ReadyController(ControllerParams params)
  : BaseController(params) {}

void ReadyController::onStart(Profile& profile) {
  Dprint(F("ReadyController::on_start"));
  drawMainScreen(phost, MAINMENU);

  // Home the dispenser head
}

void ReadyController::onInteraction(const Interaction& interaction) {
  // Handle key presses
  switch (interaction.key_pressed) {

    case START:
      Dprint(F("ReadyController::on_interaction: Transitioning to start"));
      startNextController(CONTROLLER_RUN);
      return;

    case SETTING:
      // Handle password protection if enabled
      if (CurProf.passwordEnabled) {
        PasswordVerificationResult result = verifyPassword(phost);
        
        if (result == PASSWORD_CANCELLED) {
          drawMainScreen(phost, MAINMENU);
          return;
        }
        
        if (result == PASSWORD_INCORRECT) {
          MainScreenParams params = {0, 0, 0, 0, DIALOG_ERROR_WRONG_PASSWORD};
          drawMainScreen(phost, MAINMENU, &params);
          delay(2000);
          drawMainScreen(phost, MAINMENU);
          return;
        }
      }

      Dprint(F("ReadyController::on_interaction: Transitioning to settings"));
      startNextController(CONTROLLER_SETTINGS);
      return;
    
    default:
      break;
  }

  // Handle PLC messages
  switch (interaction.plc_message_type) {
    case MSG_START:
      Dprint(F("ReadyController::on_interaction: Transitioning to start"));
      startNextController(CONTROLLER_RUN);
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

ControllerStepResult ReadyController::onStep() {
  DispenserProcessResult result = dispenserHead.process();
  return ControllerStepResult(result.steppers, result.dispenser);
}

int ReadyController::getModeType() const {
  return CONTROLLER_READY;
}