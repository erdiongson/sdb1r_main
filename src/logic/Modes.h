#include "DispenserHead.h"
#include "TrayPositionHandler.h"
#include "../../SaveProfile.h"
#include "../ui/Platform.h"

#define MODE_COMPLETE 999
#define MODE_CONTINUE 0

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
};