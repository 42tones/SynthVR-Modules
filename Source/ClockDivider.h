#include "JuceHeader.h"
#include <algorithm>

using namespace juce;

class ClockDivider
{
public:
    ClockDivider(int divisions);

    void setEnabled(bool isEnabled) noexcept { enabled = isEnabled; }
    void setDivisions(int divisions) noexcept;
    void reset() noexcept;
    float processSample(float sample) noexcept;

private:
    int divisions = 1;
    bool enabled = true;

    bool currentlyTriggered = false;
    bool previouslyTriggered = false;
    bool outputGateOpen = false;
    int currentTriggerNumber = 0;

    int samplesPerTick = 0;
    int samplesSinceLastTick = 0;
    int gateLengthSamples = 0;
    int samplesSinceGateOpened = 0;
};
