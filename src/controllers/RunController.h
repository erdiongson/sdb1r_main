#include "BaseController.h"
#include "../logic/TrayPositionHandler.h"
#include "../../Config.h"

class RunController : public BaseController {
private:
  bool paused = false;

  enum Stage {
    STAGE_IDLE,
    STAGE_SET_VIB_LEVEL,
    STAGE_SET_VIB_DURATION,
    STAGE_ZERO,
    STAGE_START_PRIME,
    STAGE_WAIT_PRIME,
    STAGE_LOWER_HEAD,
    STAGE_START_DISPENSE,
    STAGE_WAIT_DISPENSE,
    STAGE_RAISE_HEAD,
    STAGE_MOVE,
  };

  int stage = STAGE_IDLE;
  Profile profile;
  TrayHandler::TrayPositionHandler trayHandler;
  int32_t target_x = 0;
  int32_t target_y = 0;
  int cycle = 0;

  void process_stage_logic(DispenserProcessResult& dispenserProcessResult);
  void start_stage(Stage newStage);
  void pause();
  void stop();
  void start();

public:
  RunController(ControllerParams params);

  void on_start(Profile& profile) override;
  void on_interaction(const Interaction& interaction) override;
  ControllerStepResult on_step() override;

  int get_mode_type() const override;
};
