#pragma once
#include <Arduino.h>
#include "driver/drv8825_driver.h"
#include "driver/step_mode.h"
#include "focuser_direction.h"

class MotionController
{
private:
    DRV8825Driver stepper_driver_;

    int current_position_ = 0;
    int target_position_ = 0;

    bool is_moving_ = false;

    FocuserDirection direction_ = FocuserDirection::OUTWARD;

    unsigned long last_step_time_ = 0;
    unsigned long step_interval_us_ = 8000;

    void updateDirection()
    {
        direction_ = (current_position_ < target_position_) ? FocuserDirection::OUTWARD : FocuserDirection::INWARD;
        stepper_driver_.setDirection(direction_ == FocuserDirection::OUTWARD);
    }

public:
    MotionController();

    void setCurrentPosition(int position)
    {
        current_position_ = position;
        updateDirection();
    }

    int getCurrentPosition() const
    {
        return current_position_;
    }

    void setTargetPosition(int position)
    {
        target_position_ = position;
        updateDirection();
    }

    int getTargetPosition() const
    {
        return target_position_;
    }

    bool getIsMoving() const
    {
        return is_moving_;
    }

    void setStepMode(StepMode mode)
    {
        stepper_driver_.setStepMode(mode);
    }

    StepMode getStepMode() const
    {
        return stepper_driver_.getStepMode();
    }

    void update()
    {
        if (is_moving_ && (micros() - last_step_time_ >= step_interval_us_))
        {
            int increment = stepper_driver_.getStepMode() == StepMode::FULL_STEP ? 2 : 1;

            if (direction_ == FocuserDirection::OUTWARD)
            {
                // Check if next step would overshoot
                if (current_position_ + increment <= target_position_)
                {
                    stepper_driver_.step();
                    current_position_ += increment;
                    last_step_time_ = micros();
                }
                else
                {
                    is_moving_ = false;
                }
            }
            else
            {
                // Check if next step would overshoot
                if (current_position_ - increment >= target_position_)
                {
                    stepper_driver_.step();
                    current_position_ -= increment;
                    last_step_time_ = micros();
                }
                else
                {
                    is_moving_ = false;
                }
            }
        }
    }

    void startMovement()
    {
        if (current_position_ != target_position_)
        {
            is_moving_ = true;
        }
    }

    void stopMovement()
    {
        is_moving_ = false;
    }
};