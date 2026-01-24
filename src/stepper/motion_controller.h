#pragma once
#include <Arduino.h>
#include "driver/drv8825_driver.h"
#include "driver/step_mode.h"
#include "focuser_direction.h"

class MotionController
{
private:
    DRV8825Driver stepper_driver_;

    long current_position_ = 0;
    long target_position_ = 0;
    unsigned long distance_ = 0;

    bool is_moving_ = false;
    bool change_position_ = true;

    FocuserDirection direction_ = FocuserDirection::OUTWARD;

    unsigned long last_step_time_ = 0;
    unsigned long step_interval_us_ = 4000; // 250 steps per second
    uint8_t speed_ = 2;

    void updateDirection()
    {
        direction_ = (current_position_ < target_position_) ? FocuserDirection::OUTWARD : FocuserDirection::INWARD;
        stepper_driver_.setDirection(direction_ == FocuserDirection::OUTWARD);
    }

public:
    MotionController();

    void setCurrentPosition(long position)
    {
        if (is_moving_)
            return;

        current_position_ = position;
        target_position_ = position;
        distance_ = 0;
        updateDirection();
    }

    long getCurrentPosition() const
    {
        return current_position_;
    }

    void setTargetPosition(long position)
    {
        if (is_moving_)
            return;

        target_position_ = position;
        distance_ = abs(target_position_ - current_position_);
        updateDirection();
    }

    long getTargetPosition() const
    {
        return target_position_;
    }

    bool getIsMoving() const
    {
        return is_moving_;
    }

    void setStepMode(StepMode mode)
    {
        if (is_moving_)
            return;

        stepper_driver_.setStepMode(mode);
    }

    StepMode getStepMode() const
    {
        return stepper_driver_.getStepMode();
    }

    void setSpeed(uint8_t speed)
    {
        if (is_moving_)
            return;

        switch (speed)
        {
        case 0x02: // 250 steps/s
            step_interval_us_ = 4000;
            speed_ = speed;
            break;
        case 0x04: // 125 steps/s
            step_interval_us_ = 8000;
            speed_ = speed;
            break;
        case 0x08: // 63 steps/s
            step_interval_us_ = 16000;
            speed_ = speed;
            break;
        case 0x10: // 32 steps/s
            step_interval_us_ = 32000;
            speed_ = speed;
            break;
        case 0x20: // 16 steps/s
            step_interval_us_ = 64000;
            speed_ = speed;
            break;
        default:
            break;
        }
    }

    uint8_t getSpeed() const
    {
        return speed_;
    }

    void update()
    {
        if (!is_moving_)
            return;

        if (distance_ == 0)
        {
            is_moving_ = false;
            return;
        }

        auto now = micros();
        auto delta_time = now - last_step_time_;

        auto actual_interval_us = stepper_driver_.getStepMode() == StepMode::FULL_STEP ? step_interval_us_ : step_interval_us_ >> 1;

        if (delta_time < actual_interval_us)
            return;

        stepper_driver_.step();

        if (stepper_driver_.getStepMode() == StepMode::HALF_STEP)
            change_position_ = !change_position_;

        if (change_position_)
        {
            current_position_ += (direction_ == FocuserDirection::OUTWARD) ? 1 : -1;
            distance_--;
        }

        last_step_time_ = now;
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