#include "DispenserHead.h"
#include "TrayPositionHandler.h"
#include "../../SaveProfile.h"
#include "../ui/Platform.h"
#include "../ui/Move_Test_Screen.h"
#include "../ui/Dispense_Test_Screen.h"

#define MODE_COMPLETE 999
#define MODE_CONTINUE 0

// Mode type constants
#define MODE_TYPE_HOME 1
#define MODE_TYPE_DISPENSE 2
#define MODE_TYPE_MOVE_TEST 3
#define MODE_TYPE_DISPENSE_TEST 4

class Mode {
public:
  DispenserHead& dispenserHead;

public:
  // Constructor that accepts a dispenser head reference
  Mode(DispenserHead& head)
    : dispenserHead(head) {}

  // Virtual destructor for proper cleanup in derived classes
  virtual ~Mode() = default;

  // Pure virtual methods that must be implemented by derived classes
  virtual void on_start(Profile& profile) = 0;
  virtual void on_button_pressed(int button) = 0;
  virtual int on_step() = 0;  // Returns error code or MODE_COMPLETE
  
  // Virtual method to get mode type
  virtual int get_mode_type() const = 0;
};


class HomeMode : public Mode {
public:
  HomeMode(DispenserHead& head)
    : Mode(head) {}

  void on_start(Profile& profile) override {
    Serial.println("MODE: Home mode");
    dispenserHead.x().moveToMax();
    dispenserHead.y().moveToMin();
    dispenserHead.z().moveToMin();
  }

  void on_button_pressed(int button) override {}

  int on_step() override {
    if (dispenserHead.x().isComplete() && dispenserHead.y().isComplete() && dispenserHead.z().isComplete()) {
      Serial.println("MODE: Home mode complete");
      return MODE_COMPLETE;
    }
    return MODE_CONTINUE;
  }
  
  int get_mode_type() const override { return MODE_TYPE_HOME; }
};


class DispenseMode : public Mode {
private:
  bool paused = false;

  enum Stage {
    IDLE_STAGE,
    ZERO_STAGE,
    OFFSET_STAGE,
    LOWER_HEAD_STAGE,
    START_DISPENSE_STAGE,
    WAIT_DISPENSE_STAGE,
    RAISE_HEAD_STAGE,
    MOVE_STAGE,
    HOME_STAGE
  };

  int stage = IDLE_STAGE;

  Profile profile;
  TrayHandler::TrayPositionHandler trayHandler;

  int target_x = 0;
  int target_y = 0;

public:
  // Constructor that passes dispenser head reference to base class
  DispenseMode(DispenserHead& head)
    : Mode(head) {}

  void on_start(Profile& profile) override {
    Serial.println("MODE: Setting profile on trayhandler");
    trayHandler.load_profile(profile);
    trayHandler.reset();

    Serial.println("MODE: Setting profile");
    this->profile = profile;

    paused = false;

    start_stage(ZERO_STAGE);
  }

  void on_button_pressed(int button) override {
    if (button == PAUSE) {
      Serial.println("MODE: Paused");
      paused = true;
      dispenserHead.x().stop();
      dispenserHead.y().stop();
      dispenserHead.z().stop();
    } else if (button == START) {
      Serial.println("MODE: Resumed");
      paused = false;
      start_stage(stage);
    }
  }

  int on_step() override {
    if (paused) return MODE_CONTINUE;

    switch (stage) {
      case ZERO_STAGE:
        if (dispenserHead.x().isComplete() && dispenserHead.y().isComplete() && dispenserHead.z().isComplete()) {
          dispenserHead.x().reset();
          dispenserHead.y().reset();
          dispenserHead.z().reset();
          start_stage(OFFSET_STAGE);
        }
        break;
      case OFFSET_STAGE:
        if (dispenserHead.x().isComplete() && dispenserHead.y().isComplete()) {
          start_stage(LOWER_HEAD_STAGE);
        }
        break;
      case LOWER_HEAD_STAGE:
        if (dispenserHead.z().isComplete()) {
          start_stage(START_DISPENSE_STAGE);
        }
        break;
      case START_DISPENSE_STAGE:
        if (dispenserHead.get_state() == DispenserHead::COMPLETED) {
          dispenserHead.send_dispense(1);
          start_stage(WAIT_DISPENSE_STAGE);
        }
        break;
      case WAIT_DISPENSE_STAGE:
        if (dispenserHead.get_state() == DispenserHead::COMPLETED) {
          start_stage(RAISE_HEAD_STAGE);
        }
        break;
      case RAISE_HEAD_STAGE:
        if (dispenserHead.z().isComplete()) {
          TrayHandler::PositionResult result = trayHandler.goToNextValidPosition();
          if (result.hasNext) {
            target_x = (profile.trayOriginX + (result.position.x * profile.pitch_x)) * -STEPS_PER_UNIT_X;
            target_y = (profile.trayOriginY + (result.position.y * profile.pitch_y)) * STEPS_PER_UNIT_Y;
            start_stage(MOVE_STAGE);
          } else {
            start_stage(HOME_STAGE);
          }
        }
        break;
      case MOVE_STAGE:
        if (dispenserHead.x().isComplete() && dispenserHead.y().isComplete()) {
          start_stage(HOME_STAGE);
        }
        break;
      case HOME_STAGE:
        if (dispenserHead.x().isComplete() && dispenserHead.y().isComplete() && dispenserHead.z().isComplete()) {
          stage = IDLE_STAGE;
          return MODE_COMPLETE;
        }
        break;
      default:
        break;
    }
    return MODE_CONTINUE;
  }

  void start_stage(int newStage) {
    Serial.println("MODE: Setting stage: " + String(newStage));
    switch (newStage) {
      case ZERO_STAGE:
        this->stage = ZERO_STAGE;
        dispenserHead.x().moveToMin();
        dispenserHead.y().moveToMin();
        dispenserHead.z().moveToMin();
        break;
      case OFFSET_STAGE:
        this->stage = OFFSET_STAGE;
        dispenserHead.x().moveTo(STEPS_PER_UNIT_X * profile.trayOriginX);
        dispenserHead.y().moveTo(STEPS_PER_UNIT_Y * profile.trayOriginY);
        break;
      case LOWER_HEAD_STAGE:
        this->stage = LOWER_HEAD_STAGE;
        dispenserHead.z().moveTo(STEPS_PER_UNIT_Z * profile.ZDip);
        break;
      case START_DISPENSE_STAGE:
        this->stage = START_DISPENSE_STAGE;
        dispenserHead.send_dispense(1);
        break;
      case WAIT_DISPENSE_STAGE:
        this->stage = WAIT_DISPENSE_STAGE;
        break;
      case RAISE_HEAD_STAGE:
        this->stage = RAISE_HEAD_STAGE;
        dispenserHead.z().moveTo(0);
        break;
      case MOVE_STAGE:
        this->stage = MOVE_STAGE;
        dispenserHead.x().moveTo(STEPS_PER_UNIT_X * target_x);
        dispenserHead.y().moveTo(STEPS_PER_UNIT_Y * target_y);
        break;
      case HOME_STAGE:
        this->stage = HOME_STAGE;
        dispenserHead.x().moveToMax();
        dispenserHead.y().moveToMin();
        dispenserHead.z().moveToMin();
        break;
      default:
        break;
    }
  }
  
  int get_mode_type() const override { return MODE_TYPE_DISPENSE; }
};


class MoveTestMode : public Mode {
private:
  bool back = false;

public:
  MoveTestMode(DispenserHead& head)
    : Mode(head) {}

  void on_start(Profile& profile) override {
    Serial.println("MODE: Move test mode");
    dispenserHead.x().moveToMax();
    dispenserHead.y().moveToMin();
    dispenserHead.z().moveToMin();
  }

  void on_button_pressed(int button) override {
    if (button == BACK) {
      back = true;
    }
    if (button == TAG_MOVE_UP) {
      Serial.println("MODE: Move up");
      dispenserHead.y().moveBy(STEPS_PER_UNIT_Y * 5 * 100);
    }
    if (button == TAG_MOVE_DOWN) {
      Serial.println("MODE: Move down");
      dispenserHead.y().moveBy(-STEPS_PER_UNIT_Y * 5 * 100);
    }
    if (button == TAG_MOVE_LEFT) {
      Serial.println("MODE: Move left");
      dispenserHead.x().moveBy(STEPS_PER_UNIT_X * 5 * 100);
    }
    if (button == TAG_MOVE_RIGHT) {
      Serial.println("MODE: Move right");
      dispenserHead.x().moveBy(-STEPS_PER_UNIT_X * 5 * 100);
    }
    if (button == TAG_Z_UP) {
      Serial.println("MODE: Move z up");
      dispenserHead.z().moveBy(-STEPS_PER_UNIT_Z * 1 * 100);
    }
    if (button == TAG_Z_DOWN) {
      Serial.println("MODE: Move z down");
      dispenserHead.z().moveBy(STEPS_PER_UNIT_Z * 1 * 100);
    }
  }

  int on_step() override {
    if (back) {
      return MODE_COMPLETE;
    }
    return MODE_CONTINUE;
  }
  
  int get_mode_type() const override { return MODE_TYPE_MOVE_TEST; }
};


class DispenseTestMode : public Mode {
private:
  bool back = false;
  uint8_t current_vibration_level = 2;  // Default to U2 (medium)
  uint8_t current_vibration_time = 2;   // Default to 2 seconds

public:
  DispenseTestMode(DispenserHead& head)
    : Mode(head) {}

  void on_start(Profile& profile) override {
    Serial.println("MODE: Dispense test mode");
    // Initialize to home position
    dispenserHead.x().moveToMax();
    dispenserHead.y().moveToMin();
    dispenserHead.z().moveToMin();
  }

  void on_button_pressed(int button) override {
    if (button == TAG_DISPENSE_BACK) {
      back = true;
    }
    
    // Dispense button
    if (button == TAG_DISPENSE) {
      Serial.println("MODE: Dispense test - setting vibration and dispensing");
      // Set vibration level and time, then dispense
      if (dispenserHead.set_vibration_level(current_vibration_level)) {
        Serial.println("MODE: Vibration level set to " + String(current_vibration_level));
        if (dispenserHead.set_vibration_time(current_vibration_time)) {
          Serial.println("MODE: Vibration time set to " + String(current_vibration_time) + "s");
          dispenserHead.send_dispense(1);
        }
      }
    }
    
    // Vibration level buttons (U0-U4)
    if (button == TAG_VIB_U0) {
      current_vibration_level = 0;
      Serial.println("MODE: Vibration level set to U0");
    }
    if (button == TAG_VIB_U1) {
      current_vibration_level = 1;
      Serial.println("MODE: Vibration level set to U1");
    }
    if (button == TAG_VIB_U2) {
      current_vibration_level = 2;
      Serial.println("MODE: Vibration level set to U2");
    }
    if (button == TAG_VIB_U3) {
      current_vibration_level = 3;
      Serial.println("MODE: Vibration level set to U3");
    }
    if (button == TAG_VIB_U4) {
      current_vibration_level = 4;
      Serial.println("MODE: Vibration level set to U4");
    }
    
    // Vibration time buttons (1-5s)
    if (button == TAG_VIB_TIME_1) {
      current_vibration_time = 1;
      Serial.println("MODE: Vibration time set to 1s");
    }
    if (button == TAG_VIB_TIME_2) {
      current_vibration_time = 2;
      Serial.println("MODE: Vibration time set to 2s");
    }
    if (button == TAG_VIB_TIME_3) {
      current_vibration_time = 3;
      Serial.println("MODE: Vibration time set to 3s");
    }
    if (button == TAG_VIB_TIME_4) {
      current_vibration_time = 4;
      Serial.println("MODE: Vibration time set to 4s");
    }
    if (button == TAG_VIB_TIME_5) {
      current_vibration_time = 5;
      Serial.println("MODE: Vibration time set to 5s");
    }
  }

  int on_step() override {
    if (back) {
      return MODE_COMPLETE;
    }
    return MODE_CONTINUE;
  }
  
  int get_mode_type() const override { return MODE_TYPE_DISPENSE_TEST; }
  
  // Getter methods for current settings (used by UI)
  uint8_t get_current_vibration_level() const { return current_vibration_level; }
  uint8_t get_current_vibration_time() const { return current_vibration_time; }
};