#include "../BaseController.h"

class SettingsController : public BaseController {
private:
  Profile* currentProfile;
  
  void incrementVibrationLevel();
  void incrementVibrationTime();
  void editSkipColumn(Gpu_Hal_Context_t* phost);
  void editSkipRow(Gpu_Hal_Context_t* phost);
  void editSkipIndividual(Gpu_Hal_Context_t* phost);

public:
  SettingsController(ControllerParams params);

  void onStart(Profile& profile) override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;
};
