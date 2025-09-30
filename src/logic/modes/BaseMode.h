#pragma once

#include "../DispenserHead.h"
#include "../../../SaveProfile.h"
#include "ModesCommon.h"
#include "../../ui/App_Common.h"

class BaseMode {
public:
  DispenserHead& dispenserHead;
  Gpu_Hal_Context_t *phost;

public:
  // Constructor that accepts a dispenser head reference and GPU HAL context
  BaseMode(DispenserHead& head, Gpu_Hal_Context_t *host)
    : dispenserHead(head), phost(host) {}

  // Virtual destructor for proper cleanup in derived classes
  virtual ~BaseMode() = default;

  // Pure virtual methods that must be implemented by derived classes
  virtual void on_start(Profile& profile) = 0;
  virtual void on_button_pressed(int button) = 0;
  virtual int on_step() = 0;  // Returns error code or MODE_COMPLETE
  
  // Virtual method to get mode type
  virtual int get_mode_type() const = 0;
};

