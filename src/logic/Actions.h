#include "DispenserHead.h"

#define HOME_ACTION 1
#define CLEAR_ACTION 2
#define ZERO_ACTION 3
#define MOVE_ACTION 4
#define DISPENSE_ACTION 5

#define MOVEMENT_NEXT_POSITION 1
#define MOVEMENT_OFFSET 2

class Action {
public:
  // Constructor that accepts a StateController reference and an on_complete
  // callback
  Action(DispenserHead &dispenser_head_)
    : dispenser_head_(dispenser_head_) {}

  // Virtual destructor for proper cleanup in derived classes
  virtual ~Action() = default;

  // Type of action
  virtual int type() const = 0;

  // Purpose of move action
  int movement_purpose() const { 
    return 0; 
  }

  // Called when the action starts
  virtual void on_start() = 0;

  // Called on each step/update of the action
  // Default implementation updates all axes and checks for completion
  virtual bool on_step() {
    return step_axis();
  }

  bool step_axis() {
    if (axis == 0) {
      bool complete = dispenser_head_.x().onStep();
      if (complete) axis ++;
    } else if (axis == 1) {
      bool complete = dispenser_head_.y().onStep();
      if (complete) axis ++;
    } else if (axis == 2) {
      bool complete = dispenser_head_.z().onStep();
      if (complete) return true;
    }
    return false;
  }

protected:
  // Reference to the state controller
  DispenserHead &dispenser_head_;
  int axis = 0;
};

class HomeAction : public Action {
public:
  HomeAction(DispenserHead &dispenser_head_)
    : Action(dispenser_head_) {}

  int type() const override { 
    return HOME_ACTION;
  }

  void on_start() override {
    dispenser_head_.x().moveToMin();
    dispenser_head_.y().moveToMin();
    dispenser_head_.z().moveToMin();
  }
};

class ClearAction : public Action {
public:
  ClearAction(DispenserHead &dispenser_head_)
    : Action(dispenser_head_) {}

  int type() const override { 
    return CLEAR_ACTION; 
  }

  void on_start() override {
    // Check if any axis is at min or max and move it slightly away
    if (dispenser_head_.x().isAtMin()) {
      dispenser_head_.x().moveBy(100);  // Move away from min by 100 steps
    } else if (dispenser_head_.x().isAtMax()) {
      dispenser_head_.x().moveBy(-100);  // Move away from max by 100 steps
    }

    if (dispenser_head_.y().isAtMin()) {
      dispenser_head_.y().moveBy(100);
    } else if (dispenser_head_.y().isAtMax()) {
      dispenser_head_.y().moveBy(-100);
    }

    if (dispenser_head_.z().isAtMin()) {
      dispenser_head_.z().moveBy(100);
    } else if (dispenser_head_.z().isAtMax()) {
      dispenser_head_.z().moveBy(-100);
    }
  }
};

class ZeroAction : public Action {
public:
  ZeroAction(DispenserHead &dispenser_head_)
    : Action(dispenser_head_) {}

  int type() const { 
    return ZERO_ACTION; 
  }

  void on_start() override {
    dispenser_head_.x().moveToMax();
    dispenser_head_.y().moveToMin();
    dispenser_head_.z().moveToMin();
  }
};

class MoveAction : public Action {
public:

  MoveAction(DispenserHead &dispenser_head_, long x, long y, int purpose)
    : Action(dispenser_head_), x_(x), y_(y), purpose_(purpose) {}

  int type() const override { 
    return MOVE_ACTION; 
  }

  void on_start() override {
    dispenser_head_.x().moveBy(x_);
    dispenser_head_.y().moveBy(y_);
  }

  int movement_purpose() const { 
    return purpose_; 
  }

private:
  long x_;
  long y_;
  int purpose_;
};

class DispenseAction : public Action {
public:
  /**
   * Constructor for DispenseAction
   * 
   * @param dispenser_head_ Reference to the dispenser head
   * @param z_dip Amount in mm that the Z-axis should lower and raise (default: 20)
   */
  DispenseAction(DispenserHead &dispenser_head_,
                 float z_dip = 20.0)
    : Action(dispenser_head_), z_dip_(static_cast<long>(z_dip * STEPS_PER_UNIT_Z)) {}

  int type() const { 
    return DISPENSE_ACTION; 
  }

  enum Stage {
    LOWER_HEAD,
    START_DISPENSE,
    WAIT_DISPENSE,
    RAISE_HEAD,
  };

  void on_start() override {
    stage_ = LOWER_HEAD;
    dispenser_head_.z().moveBy(-z_dip_);
  }

  bool on_step() override {
    switch (stage_) {
      case LOWER_HEAD:
        dispenser_head_.z().onStep();
        if (dispenser_head_.z().isComplete()) {
          stage_ = START_DISPENSE;
        }
        return false;
      case START_DISPENSE:
        // Send dispense command
        dispenser_head_.send_dispense(1);
        stage_ = WAIT_DISPENSE;
        return false;

      case WAIT_DISPENSE:
        // Wait for dispense complete response
        if (dispenser_head_.get_state() == DispenserHead::COMPLETED) {
          stage_ = RAISE_HEAD;
        }
        return false;
      case RAISE_HEAD:
        dispenser_head_.z().moveBy(z_dip_);
        if (dispenser_head_.z().isComplete()) {
          return true;
        }
        return false;
    }
  }

private:
  int stage_;
  long z_dip_;  // Z-axis dip amount in steps
};