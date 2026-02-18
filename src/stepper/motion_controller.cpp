#include "motion_controller.h"

MotionController::MotionController() : stepper_driver_(6, 5, 21, 7, 8)
{
}

void MotionController::begin()
{
  stepper_driver_.begin();
  stepper_driver_.enable();
}
