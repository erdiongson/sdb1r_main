#include "DebugController.h"
#include "../../views/debug/DebugScreen.h"
#include "../../Constants.h"
#include "../../../Config.h"
#include "../../Utils.h"

DebugController::DebugController(ControllerParams params)
  : BaseController(params) {}

void DebugController::onStart() {
  Dprint(F("DebugController::on_start"));
  DebugScreenParams params = {CurProf.size_flag, false, false};
  drawDebugScreen(phost, params);
}

void DebugController::onInteraction(const Interaction& interaction) {
  // Handle key presses
  switch (interaction.key_pressed) {

    case TAG_DEBUG_MOVE_TEST:
      Dprint(F("Debug: Move Test button pressed"));
      startNextController(CONTROLLER_MOVE_TEST);
      break;

    case TAG_DEBUG_DISPENSER_TEST:
      Dprint(F("Debug: Dispenser Test button pressed"));
      startNextController(CONTROLLER_DISPENSE_TEST);
      break;

    case TAG_DEBUG_BLANK_EEPROM: {
      Dprint(F("Debug: Blank EEPROM button pressed"));
      profileManager.blankEEPROM();
      DebugScreenParams params = {CurProf.size_flag, true, false};
      drawDebugScreen(phost, params);
      break;
    }

    case TAG_DEBUG_RESET_PROFILES: {
      Dprint(F("Debug: Reset Profiles button pressed"));
      profileManager.preLoadEEPROM();
      DebugScreenParams params = {CurProf.size_flag, false, true};
      drawDebugScreen(phost, params);
      break;
    }

    case TAG_DEBUG_TOGGLE_SIZE: {
      Dprint(F("Debug: Toggle Size button pressed"));
      CurProf.size_flag = CurProf.size_flag == 0 ? 1 : 0;
      DebugScreenParams params = {CurProf.size_flag, false, false};
      drawDebugScreen(phost, params);
      break;
    }

    case TAG_DEBUG_BACK:
      Dprint(F("Debug: Back button pressed"));
      startNextController(CONTROLLER_READY);
      break;

    default:
      break;
  }
}

ControllerStepResult DebugController::onStep() {
  DispenserProcessResult result = dispenserHead.process();
  return ControllerStepResult(result.steppers, result.dispenser);
}

int DebugController::getModeType() const {
  return CONTROLLER_DEBUG;
}
