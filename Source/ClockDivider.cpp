#include "ClockDivider.h"

ClockDivider::ClockDivider(int divisions)
{
    setDivisions(divisions);
}

void ClockDivider::setDivisions(int divisions) noexcept
{
    this->divisions = divisions;
    reset();
}

void ClockDivider::reset() noexcept
{
    currentTriggerNumber = divisions - 1;
}

float ClockDivider::processSample(float sample) noexcept
{
    if (!enabled)
        return sample;

    samplesSinceLastTick++;

    currentlyTriggered = sample >= 0.5f;
    if (currentlyTriggered && !previouslyTriggered)
    {
        // Only trigger this clock if we hit the rigth division
        if (currentTriggerNumber++ % divisions == 0)
        {
            samplesPerTick = samplesSinceLastTick;
            gateLengthSamples = samplesPerTick / 2 * divisions;
            outputGateOpen = true;
        }

        samplesSinceLastTick = 0;
    }

    if (outputGateOpen && samplesSinceLastTick >= gateLengthSamples)
        outputGateOpen = false;

    previouslyTriggered = currentlyTriggered;

    return outputGateOpen;
}