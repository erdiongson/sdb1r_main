#include "DebugController.h"
#include "../../views/debug/DebugScreen.h"
#include "../../Constants.h"
#include "../../../Config.h"
#include "../../Utils.h"

DebugController::DebugController(ControllerParams params)
  : BaseController(params) {}

void DebugController::onStart() {
  Logger::log(F("DebugController::on_start"));
  drawDebugScreen(phost, {false, false});
}

void DebugController::onInteraction(const Interaction& interaction) {
  // Handle key presses
  switch (interaction.key_pressed) {

    case TAG_DEBUG_MOVE_TEST:
      Logger::log(F("Debug: Move Test button pressed"));
      startNextController(CONTROLLER_MOVE_TEST);
      break;

    case TAG_DEBUG_DISPENSER_TEST:
      Logger::log(F("Debug: Dispenser Test button pressed"));
      startNextController(CONTROLLER_DISPENSE_TEST);
      break;

    case TAG_DEBUG_BLANK_EEPROM: {
      Logger::log(F("Debug: Blank EEPROM button pressed"));
      profile_manager.blankEEPROM();
      drawDebugScreen(phost, {false, true});
      break;
    }

    case TAG_DEBUG_RESET_PROFILES: {
      Logger::log(F("Debug: Reset Profiles button pressed"));
      profile_manager.preLoadEEPROM();
      drawDebugScreen(phost, {false, true});
      break;
    }

    case TAG_DEBUG_BACK:
      Logger::log(F("Debug: Back button pressed"));
      startNextController(CONTROLLER_READY);
      break;

    default:
      break;
  }
}

ControllerStepResult DebugController::onStep() {
  DispenserProcessResult result = dispenserHead.process();
  return ControllerStepResult(result.steppers == AXIS_STATE_RUNNING);
}

int DebugController::getModeType() const {
  return CONTROLLER_DEBUG;
}
