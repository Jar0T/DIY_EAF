#include "drv8825_driver.h"
#include <Arduino.h>

DRV8825Driver::DRV8825Driver(uint8_t step_pin, uint8_t dir_pin, uint8_t enable_pin,
                             uint8_t m0_pin, uint8_t m1_pin, uint8_t m2_pin)
    : enabled_(false), direction_(true), step_mode_(StepMode::FULL_STEP),
      step_pin_(step_pin), dir_pin_(dir_pin), enable_pin_(enable_pin),
      m0_pin_(m0_pin), m1_pin_(m1_pin), m2_pin_(m2_pin)
{
    pinMode(step_pin_, OUTPUT);
    pinMode(dir_pin_, OUTPUT);
    pinMode(enable_pin_, OUTPUT);
    pinMode(m0_pin_, OUTPUT);
    pinMode(m1_pin_, OUTPUT);
    pinMode(m2_pin_, OUTPUT);

    digitalWrite(dir_pin_, direction_ ? HIGH : LOW);
    digitalWrite(step_pin_, LOW);

    disable();
    setStepMode(step_mode_);
}

void DRV8825Driver::step()
{
    digitalWrite(step_pin_, HIGH);
    delayMicroseconds(2); // Minimum pulse width
    digitalWrite(step_pin_, LOW);
    delayMicroseconds(2); // Minimum pulse width
}

void DRV8825Driver::enable()
{
    digitalWrite(enable_pin_, LOW); // Active low
    enabled_ = true;
}

void DRV8825Driver::disable()
{
    digitalWrite(enable_pin_, HIGH); // Active low
    enabled_ = false;
}

bool DRV8825Driver::isEnabled() const
{
    return enabled_;
}

void DRV8825Driver::setDirection(bool clockwise)
{
    direction_ = clockwise;
    digitalWrite(dir_pin_, direction_ ? HIGH : LOW);
    delayMicroseconds(2);
}

bool DRV8825Driver::getDirection() const
{
    return direction_;
}

void DRV8825Driver::setStepMode(StepMode mode)
{
    step_mode_ = mode;

    switch (step_mode_)
    {
    case StepMode::FULL_STEP:
        digitalWrite(m0_pin_, HIGH);
        digitalWrite(m1_pin_, LOW);
        digitalWrite(m2_pin_, LOW);
        break;
    case StepMode::HALF_STEP:
        digitalWrite(m0_pin_, LOW);
        digitalWrite(m1_pin_, HIGH);
        digitalWrite(m2_pin_, LOW);
        break;
    }
}

StepMode DRV8825Driver::getStepMode() const
{
    return step_mode_;
}
