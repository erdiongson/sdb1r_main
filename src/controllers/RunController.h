#include "BaseController.h"
#include "../logic/TrayPositionHandler.h"
#include "../../Config.h"

class RunController : public BaseController {
private:
  bool paused = false;

  enum Stage {
    IDLE_STAGE,
    SET_VIB_LEVEL_STAGE,
    SET_VIB_DURATION_STAGE,
    ZERO_STAGE,
    PRIME_STAGE,
    LOWER_HEAD_STAGE,
    START_DISPENSE_STAGE,
    DISPENSE_STAGE,
    RAISE_HEAD_STAGE,
    MOVE_STAGE,
    HOME_STAGE
  };

  int stage = IDLE_STAGE;
  Profile profile;
  TrayHandler::TrayPositionHandler trayHandler;
  int32_t target_x = 0;
  int32_t target_y = 0;
  int cycle = 0;

  void process_stage_logic(DispenserProcessResult& dispenserProcessResult);
  void start_stage(Stage newStage);

public:
  RunController(ControllerParams params);

  void on_start(Profile& profile) override;
  void on_interaction(const Interaction& interaction) override;
  ControllerStepResult on_step() override;

  int get_mode_type() const override;
};
