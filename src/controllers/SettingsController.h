#include "BaseController.h"

class SettingsController : public BaseController {
private:
  Profile* currentProfile;
  
  void increment_vibration_level();
  void increment_vibration_time();
  void editSkipColumn(Gpu_Hal_Context_t* phost);
  void editSkipRow(Gpu_Hal_Context_t* phost);
  void editSkipIndividual(Gpu_Hal_Context_t* phost);

public:
  SettingsController(ControllerParams params);

  void on_start(Profile& profile) override;
  void on_interaction(const Interaction& interaction) override;
  ControllerStepResult on_step() override;

  int get_mode_type() const override;
};
