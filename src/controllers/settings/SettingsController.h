#pragma once

#include "../BaseController.h"
#include "../../views/settings/SettingsScreen.h"

// Result of parameter verification
struct VerificationResult {
  bool is_valid;
  InputErrors errors;
  bool has_parameter_error;  // True if error is due to parameter range violations
  bool has_dimension_error;  // True if error is due to dimension boundary violations
};

class SettingsController : public BaseController {
 private:
  Profile* current_profile;

  void incrementVibrationLevel();
  void incrementVibrationTime();
  
  // Verifies all profile parameters and returns validation result with error flags.
  // @return VerificationResult containing validity status and specific error flags.
  VerificationResult verifyParameters();
  
  // Helper method to draw the settings screen with current profile and errors.
  // @param dialog_code Optional dialog code to display (default 0).
  void drawScreen(int dialog_code = 0);

 public:
  SettingsController(ControllerParams params);

  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;
};
