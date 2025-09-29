#include "Actions.h"
#include "DispenserHead.h"
#include "../communication/DispenserHeadSerial.h"
#include "TrayPositionHandler.h"
#include "Axis.h"

struct StateControllerStepResult {
  int error_code;
  bool completed;
};

class StateController {
public:

  /**
   * Struct for serial communication configuration
   */
  struct SerialConfig {
    HardwareSerial &serial;
    SerialConfig(HardwareSerial &serial)
      : serial(serial) {}
  };

  /**
   * Constructor for StateController that creates its own DispenserHead and TrayPositionHandler instances
   * 
   * @param params All parameters for the dispenser head
   */
  StateController(const DispenserHeadParams &params)
    : dispenser_head(new DispenserHead(params)),
      tray_position_handler(new TrayHandler::TrayPositionHandler()) {}

  ~StateController() {
    delete dispenser_head;
    delete tray_position_handler;
    delete current_action;  // Clean up any remaining action
  }

  void start_homing(Profile &profile) {
    if (current_action) return;
    this->profile = profile;
    tray_position_handler->load_profile(profile);
    HomeAction *home_action = new HomeAction(*dispenser_head);
    start_action(*home_action);
    paused = false;
  }

  void start_dispensing(Profile &profile) {
    if (current_action) return;
    this->profile = profile;
    tray_position_handler->load_profile(profile);
    ZeroAction *zero_action = new ZeroAction(*dispenser_head);
    start_action(*zero_action);
    paused = false;
  }

  void start_action(Action &action) {
    String action_name;
    switch(action.type()) {
      case HOME_ACTION: action_name = "HOME"; break;
      case CLEAR_ACTION: action_name = "CLEAR"; break;
      case ZERO_ACTION: action_name = "ZERO"; break;
      case MOVE_ACTION: action_name = "MOVE"; break;
      case DISPENSE_ACTION: action_name = "DISPENSE"; break;
      default: action_name = "UNKNOWN"; break;
    }
    Serial.println("Starting action: " + action_name);
    current_action = &action;
    current_action->on_start();
  }

  void stop() {
    if (!current_action)
      return;
    delete current_action;
    current_action = nullptr;
  }

  bool is_paused() {
    return paused;
  }

  void pause() {
    paused = true;
  }

  void resume() {
    paused = false;
  }

  bool on_action_complete() {
    if (current_action->type() == HOME_ACTION) {
      delete current_action;
      current_action = nullptr;
      return true;
    } else if (current_action->type() == ZERO_ACTION) {
      // After zero, move to tray origin
      int offset_x = profile.trayOriginX;
      int offset_y = profile.trayOriginY;
      delete current_action;
      MoveAction *move_action = new MoveAction(*dispenser_head, offset_x, offset_y, MOVEMENT_OFFSET);
      start_action(*move_action);
    } else if (current_action->type() == MOVE_ACTION) {
      if (current_action->movement_purpose() == MOVEMENT_OFFSET) {
        // After move, dispense
        tray_position_handler->reset();
      }
      // Pass the Z dip value from the profile
      delete current_action;
      DispenseAction *dispense_action = new DispenseAction(*dispenser_head, profile.ZDip);
      start_action(*dispense_action);
    } else if (current_action->type() == DISPENSE_ACTION) {
      Serial.println("Dispensing completed!");
      // After dispense, home
      TrayHandler::PositionResult next_position = tray_position_handler->goToNextValidPosition();
      delete current_action;
      if (next_position.hasNext) {
        Serial.println("Has next");
        // Calculate the actual movement in steps based on pitch and steps per unit
        // The position returned is now the delta (change in position)
        long x_steps = static_cast<long>(-next_position.position.x * profile.pitch_x * STEPS_PER_UNIT_X);
        long y_steps = static_cast<long>(next_position.position.y * profile.pitch_y * STEPS_PER_UNIT_Y);

        // Create move action with calculated steps
        MoveAction *move_action = new MoveAction(*dispenser_head, x_steps, y_steps, MOVEMENT_NEXT_POSITION);
        start_action(*move_action);
      } else {
        Serial.println("No next");
        HomeAction *home_action = new HomeAction(*dispenser_head);
        start_action(*home_action);
      }
    }
    return false;
  }

  StateControllerStepResult on_step() {
    if (is_paused()) return { 0, false };

    // Process incoming messages from the dispenser head
    int error_code = dispenser_head->on_step();
    // If an error occurred, stop the current action
    if (error_code) {
      stop();
      return { error_code, false };
    }

    // If there is an action in progress, process it
    if (current_action) {
      bool action_completed = current_action->on_step();
      if (action_completed) {
        bool dispensing_completed = on_action_complete();
        if (dispensing_completed) {
          return { 0, true };
        }
      }
    }

    return { 0, false };
  }

  DispenserHead *get_dispenser_head() {
    return dispenser_head;
  }

private:
  DispenserHead *dispenser_head = nullptr;
  Action *current_action = nullptr;
  TrayHandler::TrayPositionHandler *tray_position_handler = nullptr;
  Profile profile;
  bool paused = false;
};