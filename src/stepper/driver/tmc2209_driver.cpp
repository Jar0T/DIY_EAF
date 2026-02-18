#include "tmc2209_driver.h"
#include <Arduino.h>

TMC2209Driver::TMC2209Driver(uint8_t step_pin, uint8_t dir_pin, uint8_t enable_pin, uint8_t tx_pin, uint8_t rx_pin)
    : tmc2209_(&Serial1, R_SENSE, DEFAULT_ADDRESS),
      step_pin_(step_pin), dir_pin_(dir_pin), enable_pin_(enable_pin),
      tx_pin_(tx_pin), rx_pin_(rx_pin),
      enabled_(false), direction_(true)
{
    pinMode(step_pin_, OUTPUT);
    pinMode(dir_pin_, OUTPUT);
    pinMode(enable_pin_, OUTPUT);
}

void TMC2209Driver::begin()
{
    Serial1.begin(115200, SERIAL_8N1, rx_pin_, tx_pin_);
    while (!Serial1)
        ;

    tmc2209_.begin();
    tmc2209_.toff(5);

    tmc2209_.rms_current(600); // Set current to 600mA

    tmc2209_.microsteps(FS_MICROSTEPS); // Start in full-step mode
    tmc2209_.intpol(true);              // Enable interpolation to 256 microsteps for smoother motion

    tmc2209_.en_spreadCycle(false); // Disable spreadCycle for quieter operation
    tmc2209_.pwm_autoscale(true);   // Enable automatic scaling of PWM amplitude
    tmc2209_.I_scale_analog(false); // Use internal current scaling

    tmc2209_.TPOWERDOWN(10);
    tmc2209_.ihold(16);
    tmc2209_.irun(31);
    tmc2209_.iholddelay(10);
}

void TMC2209Driver::step()
{
    digitalWrite(step_pin_, HIGH);
    delayMicroseconds(2); // Minimum pulse width
    digitalWrite(step_pin_, LOW);
    delayMicroseconds(2); // Minimum pulse width
}

void TMC2209Driver::enable()
{
    digitalWrite(enable_pin_, LOW); // Active LOW
    enabled_ = true;
}

void TMC2209Driver::disable()
{
    digitalWrite(enable_pin_, HIGH); // Active LOW
    enabled_ = false;
}

bool TMC2209Driver::isEnabled()
{
    return enabled_;
}

void TMC2209Driver::setDirection(bool clockwise)
{
    digitalWrite(dir_pin_, clockwise ? HIGH : LOW);
    delayMicroseconds(2);
    direction_ = clockwise;
}

bool TMC2209Driver::getDirection() const
{
    return direction_;
}

void TMC2209Driver::setStepMode(StepMode mode)
{
    switch (mode)
    {
    case StepMode::FULL_STEP:
        tmc2209_.microsteps(FS_MICROSTEPS);
        break;
    case StepMode::HALF_STEP:
        tmc2209_.microsteps(HS_MICROSTEPS);
        break;
    default:
        break;
    }
}

StepMode TMC2209Driver::getStepMode()
{
    uint16_t microsteps = tmc2209_.microsteps();
    switch (microsteps)
    {
    case FS_MICROSTEPS:
        return StepMode::FULL_STEP;
    case HS_MICROSTEPS:
        return StepMode::HALF_STEP;
    default:
        return StepMode::UNKNOWN;
    }
}
