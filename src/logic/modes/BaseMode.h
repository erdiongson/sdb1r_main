#pragma once

#include "../DispenserHead.h"
#include "../../../SaveProfile.h"
#include "ModesCommon.h"

class BaseMode {
public:
  DispenserHead& dispenserHead;

public:
  // Constructor that accepts a dispenser head reference
  BaseMode(DispenserHead& head)
    : dispenserHead(head) {}

  // Virtual destructor for proper cleanup in derived classes
  virtual ~BaseMode() = default;

  // Pure virtual methods that must be implemented by derived classes
  virtual void on_start(Profile& profile) = 0;
  virtual void on_button_pressed(int button) = 0;
  virtual int on_step() = 0;  // Returns error code or MODE_COMPLETE
  
  // Virtual method to get mode type
  virtual int get_mode_type() const = 0;
};

