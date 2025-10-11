#include "DebugController.h"
#include "../../views/debug/DebugScreen.h"
#include "../../Constants.h"
#include "../../../Config.h"
#include "../../Utils.h"

DebugController::DebugController(ControllerParams params)
  : BaseController(params) {}

void DebugController::onStart() {
  Logger::log(F("DebugController::on_start"));
  DebugScreenParams params = {profile_manager.getCurrentProfile().size_flag, false, false};
  drawDebugScreen(phost, params);
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
      DebugScreenParams params = {profile_manager.getCurrentProfile().size_flag, true, false};
      drawDebugScreen(phost, params);
      break;
    }

    case TAG_DEBUG_RESET_PROFILES: {
      Logger::log(F("Debug: Reset Profiles button pressed"));
      profile_manager.preLoadEEPROM();
      DebugScreenParams params = {profile_manager.getCurrentProfile().size_flag, false, true};
      drawDebugScreen(phost, params);
      break;
    }

    case TAG_DEBUG_TOGGLE_SIZE: {
      Logger::log(F("Debug: Toggle Size button pressed"));
      Profile& profile = profile_manager.getCurrentProfile();
      profile.size_flag = profile.size_flag == 0 ? 1 : 0;
      DebugScreenParams params = {profile.size_flag, false, false};
      drawDebugScreen(phost, params);
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
  return ControllerStepResult(result.steppers, result.dispenser);
}

int DebugController::getModeType() const {
  return CONTROLLER_DEBUG;
}
