#pragma once

#include <Arduino.h>
#include <cstdint>
#include "step_mode.h"

/**
 * @brief Low-level driver for ULM2003 stepper motor controller
 *
 * Controls the 28BYJ-48 stepper motor via ULM2003 driver board.
 * Supports both full-step (4 steps/cycle) and half-step (8 steps/cycle) modes.
 */
class ULM2003Driver
{
private:
    // Number of control pins
    static constexpr uint8_t NO_PINS = 4;

    // Pin assignments for IN1-IN4
    const uint8_t pins_[NO_PINS];

    // Current step position in sequence
    uint8_t current_step_;

    // Current stepping mode
    StepMode step_mode_;

    // Full step sequence: 2 coils energized for maximum torque
    inline static constexpr uint8_t FULL_STEP_SEQUENCE[4][NO_PINS] = {
        {1, 1, 0, 0}, // IN1 + IN2
        {0, 1, 1, 0}, // IN2 + IN3
        {0, 0, 1, 1}, // IN3 + IN4
        {1, 0, 0, 1}  // IN4 + IN1
    };

    // Half step sequence: alternates between 1 and 2 coils for smoother motion
    inline static constexpr uint8_t HALF_STEP_SEQUENCE[8][NO_PINS] = {
        {1, 0, 0, 0}, // IN1
        {1, 1, 0, 0}, // IN1 + IN2
        {0, 1, 0, 0}, // IN2
        {0, 1, 1, 0}, // IN2 + IN3
        {0, 0, 1, 0}, // IN3
        {0, 0, 1, 1}, // IN3 + IN4
        {0, 0, 0, 1}, // IN4
        {1, 0, 0, 1}  // IN4 + IN1
    };

    // Pointer to current step sequence
    const uint8_t (*step_sequence_)[NO_PINS];

    // Current number of steps per cycle
    uint8_t steps_per_cycle_;

    /**
     * @brief Set current step pattern to motor coils
     */
    void setStepSequence();

    /**
     * @brief Set current steps per cycle based on mode
     */
    void setStepsPerCycle();

    /**
     * @brief Apply the current step pattern to motor coils
     */
    void applyCurrentStep();

public:
    /**
     * @brief Construct driver and initialize pins
     * @param in1 GPIO pin connected to IN1
     * @param in2 GPIO pin connected to IN2
     * @param in3 GPIO pin connected to IN3
     * @param in4 GPIO pin connected to IN4
     * @param mode Initial stepping mode (default: FULL_STEP)
     */
    explicit ULM2003Driver(uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4, StepMode mode = StepMode::FULL_STEP);

    /**
     * @brief Advance motor by one step in specified direction
     * @param clockwise True for clockwise, false for counter-clockwise
     */
    void step(bool clockwise);

    /**
     * @brief Disable all motor coils (power off)
     *
     * Turns off all coils to save power and reduce heat.
     * Motor loses holding torque and may drift if under load.
     */
    void disable();

    /**
     * @brief Re-enable motor at current step position
     *
     * Restores coil energization pattern for current step.
     * Use after disable() to restore holding torque.
     */
    void enable();

    /**
     * @brief Change stepping mode
     * @param mode New stepping mode
     *
     * Resets step position to 0 when mode changes.
     * Call this when motor is stopped to avoid jerky transitions.
     */
    void setStepMode(StepMode mode);

    /**
     * @brief Get current stepping mode
     */
    StepMode getStepMode() const;
};
