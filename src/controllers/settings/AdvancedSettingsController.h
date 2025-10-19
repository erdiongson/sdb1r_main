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
  char skip_col[SKIP_STRING_LEN];
  char skip_row[SKIP_STRING_LEN];
  char skip_single_pos[SKIP_STRING_LEN];
  char skipInputBuffer[SKIP_STRING_INDIVIDUAL_LEN];  // Shared input buffer for all skip editing
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
