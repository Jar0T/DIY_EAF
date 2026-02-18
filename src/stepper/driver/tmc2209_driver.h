#pragma once

#include "step_mode.h"
#include <cstdint>
#include <TMCStepper.h>

class TMC2209Driver
{
private:
    static constexpr float R_SENSE = 0.11f;          // Sense resistor value in ohms
    static constexpr uint8_t DEFAULT_ADDRESS = 0b00; // Default UART address for TMC2209
    static constexpr uint8_t FS_MICROSTEPS = 16;
    static constexpr uint8_t HS_MICROSTEPS = 32;

    bool enabled_;
    bool direction_;
    uint8_t step_pin_;
    uint8_t dir_pin_;
    uint8_t enable_pin_;
    uint8_t tx_pin_;
    uint8_t rx_pin_;

    TMC2209Stepper tmc2209_;

public:
    explicit TMC2209Driver(uint8_t step_pin, uint8_t dir_pin, uint8_t enable_pin, uint8_t tx_pin, uint8_t rx_pin);

    void begin();

    void step();

    void enable();

    void disable();

    bool isEnabled();

    void setDirection(bool clockwise);

    bool getDirection() const;

    void setStepMode(StepMode mode);

    StepMode getStepMode();
};