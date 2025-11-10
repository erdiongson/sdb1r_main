#include "DebugController.h"
#include "../../views/debug/DebugScreen.h"
#include "../../views/debug/ConfigScreen.h"
#include "../../views/common/Keyboards.h"

DebugController::DebugController(ControllerParams params) : BaseController(params) {
  show_config_screen = false;
}

void DebugController::onStart() {
  Logger::log(F("DebugController::on_start"));
  show_config_screen = false;
  drawDebugScreen(phost, { false, false, false, 0 });
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

    case TAG_DEBUG_DIALOG_TEST: {
      Logger::log(F("Debug: Dialog Test button pressed"));
      // Get dialog code from user using keypad (0-11 range for existing dialogs)
      float dialog_num = getKeypadValue(phost, 0, 0, 20, false);
      int dialog_code = (int)dialog_num;
      // Redraw screen with the selected dialog
      drawDebugScreen(phost, { false, false, false, dialog_code });
      // Only delay if dialog_code is not 0
      if (dialog_code != 0) {
        delay(4000);
        drawDebugScreen(phost, { false, false, false, 0 });
      }
      break;
    }

    case TAG_DEBUG_STEPPER_TEST:
      Logger::log(F("Debug: Stepper Test button pressed"));
      startNextController(CONTROLLER_STEPPER_TEST);
      break;

    case TAG_DEBUG_BLANK_EEPROM: {
      Logger::log(F("Debug: Blank EEPROM button pressed"));
      profile_manager.blankEEPROM();
      drawDebugScreen(phost, { true, false, false, 0 });
      break;
    }

    case TAG_DEBUG_RESET_PROFILES: {
      Logger::log(F("Debug: Reset Profiles button pressed"));
      profile_manager.preLoadEEPROM();
      drawDebugScreen(phost, { false, true, false, 0 });
      break;
    }

    case TAG_DEBUG_USE_DEBUG_PROFILES: {
      Logger::log(F("Debug: Use Debug Profiles button pressed"));
      profile_manager.preLoadDebugEEPROM();
      drawDebugScreen(phost, { false, false, true, 0 });
      break;
    }

    case TAG_DEBUG_SHOW_CONFIG:
      Logger::log(F("Debug: Show Config button pressed"));
      show_config_screen = true;
      drawConfigScreen(phost);
      break;

    case TAG_CONFIG_BACK:
      Logger::log(F("Config: Back button pressed"));
      show_config_screen = false;
      drawDebugScreen(phost, { false, false, false, 0 });
      break;

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
