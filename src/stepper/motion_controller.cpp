#include "motion_controller.h"

MotionController::MotionController() : stepper_driver_(6, 5, 21, 20, 10, 9)
{
  stepper_driver_.enable();
}
