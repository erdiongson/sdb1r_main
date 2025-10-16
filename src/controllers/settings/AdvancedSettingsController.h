#pragma once

#include "../BaseController.h"
#include "../../views/settings/AdvancedSettingsScreen.h"

// Result of skip parameter verification
struct SkipVerificationResult {
  bool is_valid;
  SkipErrors errors;
};

class AdvancedSettingsController : public BaseController {
 private:
  Profile* current_profile;
  char skipCol[SKIP_STRING_LEN];
  char skipRow[SKIP_STRING_LEN];
  char skipSinglePos[SKIP_STRING_LEN];
  void editSkipColumn(Gpu_Hal_Context_t* phost);
  void editSkipRow(Gpu_Hal_Context_t* phost);
  void editSkipIndividual(Gpu_Hal_Context_t* phost);

  // Verifies skip positions and returns validation result with error flags.
  // @return SkipVerificationResult containing validity status and specific error flags.
  SkipVerificationResult verifyParameters();

  // Helper method to draw the advanced settings screen with current profile and errors.
  void drawScreen(int dialog_code = 0);

 public:
  AdvancedSettingsController(ControllerParams params);

  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;
};
