#include "StartupController.h"
#include "../views/HomeScreen.h"
#include "../Constants.h"
#include "../../Config.h"
#include "../Utils.h"
#include "../gpu/App_Common.h"

StartupController::StartupController(ControllerParams params)
  : BaseController(params) {}

void StartupController::on_start(Profile& profile) {
  Dprint(F("StartupController::on_start"));

  // Home the dispenser head
  dispenserHead.x().moveToMax();
  dispenserHead.y().moveToMin();
  dispenserHead.z().moveToMin();
}

void StartupController::on_interaction(const Interaction& interaction) {}

 ControllerStepResult StartupController::on_step() {
  DispenserProcessResult result = dispenserHead.process();
  if (result.steppers == AXIS_STATE_COMPLETE) {
    start_next_controller(CONTROLLER_HOME);
  }
  return ControllerStepResult(result.steppers, result.dispenser);
}

int StartupController::get_mode_type() const {
  return CONTROLLER_HOME;
}
