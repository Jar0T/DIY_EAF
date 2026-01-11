#pragma once

#include <cstdint>
#include "step_mode.h"

class DRV8825Driver
{
private:
    bool enabled_;
    bool direction_;
    StepMode step_mode_;
    uint8_t step_pin_;
    uint8_t dir_pin_;
    uint8_t enable_pin_;

    uint8_t m0_pin_;
    uint8_t m1_pin_;
    uint8_t m2_pin_;

public:
    explicit DRV8825Driver(uint8_t step_pin, uint8_t dir_pin, uint8_t enable_pin,
                           uint8_t m0_pin, uint8_t m1_pin, uint8_t m2_pin);

    void step();

    void enable();

    void disable();

    bool isEnabled() const;

    void setDirection(bool clockwise);

    bool getDirection() const;

    void setStepMode(StepMode mode);

    StepMode getStepMode() const;
};
