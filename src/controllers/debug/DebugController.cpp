#include "DebugController.h"
#include "../../views/debug/DebugScreen.h"
#include "../../Constants.h"
#include "../../../Config.h"
#include "../../Utils.h"

DebugController::DebugController(ControllerParams params)
  : BaseController(params) {}

void DebugController::on_start(Profile& profile) {
  Dprint(F("DebugController::on_start"));
  DebugScreenParams params = {CurProf.sizeFlag, false, false};
  draw_debug_screen(phost, params);
}

void DebugController::on_interaction(const Interaction& interaction) {
  // Handle key presses
  switch (interaction.key_pressed) {

    case TAG_DEBUG_MOVE_TEST:
      Dprint(F("Debug: Move Test button pressed"));
      start_next_controller(CONTROLLER_MOVE_TEST);
      break;

    case TAG_DEBUG_DISPENSER_TEST:
      Dprint(F("Debug: Dispenser Test button pressed"));
      start_next_controller(CONTROLLER_DISPENSE_TEST);
      break;

    case TAG_DEBUG_BLANK_EEPROM: {
      Dprint(F("Debug: Blank EEPROM button pressed"));
      BlankEEPROM();
      DebugScreenParams params = {CurProf.sizeFlag, true, false};
      draw_debug_screen(phost, params);
      break;
    }

    case TAG_DEBUG_RESET_PROFILES: {
      Dprint(F("Debug: Reset Profiles button pressed"));
      PreLoadEEPROM();
      DebugScreenParams params = {CurProf.sizeFlag, false, true};
      draw_debug_screen(phost, params);
      break;
    }

    case TAG_DEBUG_TOGGLE_SIZE: {
      Dprint(F("Debug: Toggle Size button pressed"));
      CurProf.sizeFlag = CurProf.sizeFlag == 0 ? 1 : 0;
      DebugScreenParams params = {CurProf.sizeFlag, false, false};
      draw_debug_screen(phost, params);
      break;
    }

    case TAG_DEBUG_BACK:
      Dprint(F("Debug: Back button pressed"));
      start_next_controller(CONTROLLER_HOME);
      break;

    default:
      break;
  }
}

ControllerStepResult DebugController::on_step() {
  DispenserProcessResult result = dispenserHead.process();
  return ControllerStepResult(result.steppers, result.dispenser);
}

int DebugController::get_mode_type() const {
  return CONTROLLER_DEBUG;
}
