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
    currentTriggerNumber = 0;
    outputGateOpen = false;
    currentlyTriggered = false;
    previouslyTriggered = false;
}

float ClockDivider::processSample(float sample) noexcept
{
    if (!enabled)
        return sample;

    samplesSinceLastTick++;
    samplesSinceGateOpened++;

    if (outputGateOpen && samplesSinceGateOpened >= gateLengthSamples)
        outputGateOpen = false;

    currentlyTriggered = sample >= 0.5f;
    if (currentlyTriggered && !previouslyTriggered)
    {
        // Only trigger this clock if we hit the rigth division
        if (currentTriggerNumber++ % divisions == 0)
        {
            samplesPerTick = samplesSinceLastTick;
            gateLengthSamples = samplesPerTick * divisions / 2;
            outputGateOpen = true;
            samplesSinceGateOpened = 0;
        }

        samplesSinceLastTick = 0;
    }

    previouslyTriggered = currentlyTriggered;

    return outputGateOpen;
}