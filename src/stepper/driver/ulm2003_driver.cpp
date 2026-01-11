#include "ulm2003_driver.h"

ULM2003Driver::ULM2003Driver(uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4, StepMode mode)
    : pins_{in1, in2, in3, in4}, current_step_(0), step_mode_(mode)
{
    setStepSequence();
    setStepsPerCycle();

    for (uint8_t i = 0; i < NO_PINS; i++)
    {
        pinMode(pins_[i], OUTPUT);
        digitalWrite(pins_[i], LOW);
    }
}

void ULM2003Driver::step(bool clockwise)
{
    if (clockwise)
    {
        current_step_ = (current_step_ + 1) % steps_per_cycle_;
    }
    else
    {
        current_step_ = (current_step_ + steps_per_cycle_ - 1) % steps_per_cycle_;
    }

    applyCurrentStep();
}

void ULM2003Driver::disable()
{
    for (uint8_t i = 0; i < NO_PINS; i++)
    {
        digitalWrite(pins_[i], LOW);
    }
}

void ULM2003Driver::enable()
{
    applyCurrentStep();
}

void ULM2003Driver::setStepMode(StepMode mode)
{
    if (step_mode_ != mode)
    {
        step_mode_ = mode;
        setStepSequence();
        setStepsPerCycle();
        current_step_ = 0;
        applyCurrentStep();
    }
}

StepMode ULM2003Driver::getStepMode() const
{
    return step_mode_;
}

void ULM2003Driver::setStepSequence()
{
    step_sequence_ = (step_mode_ == StepMode::FULL_STEP)
                         ? FULL_STEP_SEQUENCE
                         : HALF_STEP_SEQUENCE;
}

void ULM2003Driver::setStepsPerCycle()
{
    steps_per_cycle_ = (step_mode_ == StepMode::FULL_STEP) ? 4 : 8;
}

void ULM2003Driver::applyCurrentStep()
{
    const uint8_t *pattern = step_sequence_[current_step_];

    for (uint8_t i = 0; i < NO_PINS; i++)
    {
        digitalWrite(pins_[i], pattern[i]);
    }
}
