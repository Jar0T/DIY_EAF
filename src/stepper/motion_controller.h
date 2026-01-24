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
    unsigned long step_interval_us_ = 0;
    uint8_t speed_ = 0x04;

    float min_speed_ = 8.0f;      // steps per second
    float max_speed_ = 125.0f;    // steps per second
    float current_speed_ = 0.0f;  // steps per second
    float acceleration_ = 100.0f; // steps per second squared

    void updateDirection()
    {
        direction_ = (current_position_ < target_position_) ? FocuserDirection::OUTWARD : FocuserDirection::INWARD;
        stepper_driver_.setDirection(direction_ == FocuserDirection::OUTWARD);
    }

    void updateSpeed(float delta_time)
    {
        // calculate number of steps to deccelerate from current speed to min speed
        float speed_diff_squared = current_speed_ * current_speed_ - min_speed_ * min_speed_;
        unsigned long steps_to_stop = (speed_diff_squared > 0) ? (speed_diff_squared / (2 * acceleration_)) : 0;
        float speed_change = acceleration_ * delta_time;

        if (distance_ <= steps_to_stop)
        {
            // decelerate
            current_speed_ -= speed_change;
            if (current_speed_ < min_speed_)
                current_speed_ = min_speed_;
        }
        else if (current_speed_ < max_speed_)
        {
            // accelerate
            current_speed_ += speed_change;
            if (current_speed_ > max_speed_)
                current_speed_ = max_speed_;
        }

        step_interval_us_ = static_cast<unsigned long>(1e6f / current_speed_);
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
            max_speed_ = 250.0f;
            speed_ = speed;
            break;
        case 0x04: // 125 steps/s
            max_speed_ = 125.0f;
            speed_ = speed;
            break;
        case 0x08: // 63 steps/s
            max_speed_ = 63.0f;
            speed_ = speed;
            break;
        case 0x10: // 32 steps/s
            max_speed_ = 32.0f;
            speed_ = speed;
            break;
        case 0x20: // 16 steps/s
            max_speed_ = 16.0f;
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
            current_speed_ = 0.0f;
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

        updateSpeed(actual_interval_us / 1e6f);

        last_step_time_ = now;
    }

    void startMovement()
    {
        if (current_position_ != target_position_)
        {
            is_moving_ = true;
            current_speed_ = min_speed_;
            step_interval_us_ = static_cast<unsigned long>(1e6f / current_speed_);
            last_step_time_ = micros();
        }
    }

    void stopMovement()
    {
        is_moving_ = false;
        current_speed_ = 0.0f;
    }
};