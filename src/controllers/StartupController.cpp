#include "StartupController.h"
#include "../Constants.h"
#include "../../Config.h"
#include "../Utils.h"
#include "../views/LogoScreen.h"

StartupController::StartupController(ControllerParams params)
  : BaseController(params), dispenserHead(params.head) {
  }

void StartupController::onStart(Profile& profile) {
  Dprint(F("StartupController::on_start"));
  draw_logo_screen(phost, 0);

  // Check if dispenser is online and responding
  dispenserHead.send_handshake();
}

void StartupController::onInteraction(const Interaction& interaction) {}

 ControllerStepResult StartupController::onStep() {
  DispenserProcessResult result = dispenserHead.process();

  // Handle possible errors
  if (result.dispenser == DISPENSER_STATE_ERROR_ACK_ERROR) {
    draw_logo_screen(phost, DIALOG_ERROR_ACK_ERROR);
    stage = STAGE_ERROR;
    return ControllerStepResult(-1, -1);
  }
  if (result.dispenser == DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED) {
    draw_logo_screen(phost, DIALOG_ERROR_MARKER_NOT_DETECTED);
    stage = STAGE_ERROR;
    return ControllerStepResult(-1, -1);
  }
  if (result.dispenser == DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE) {
    draw_logo_screen(phost, DIALOG_ERROR_IR_SENSOR);
    stage = STAGE_ERROR;
    return ControllerStepResult(-1, -1);
  }

  if (stage == STAGE_HANDSHAKE && result.dispenser == DISPENSER_STATE_IDLING) {
    Dprint(F("Handshake acknowledged 👍"));
    startNextController(CONTROLLER_HOMING);
  }
  
  return ControllerStepResult(-1, -1);
}

int StartupController::getModeType() const {
  return CONTROLLER_STARTUP;
}
