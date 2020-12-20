#include "JuceHeader.h"
#include <algorithm>

using namespace juce;

class CVQuantizer
{
public:
    /** Creates an uninitialised quantizer. You will probably want to run setScale() before use. */
    CVQuantizer();

    /** Enables or disables the quantizer. If disabled it will simply pass through the input signal. */
    void setEnabled(bool isEnabled) noexcept { enabled = isEnabled; }

    /** Sets the scale of the quantizer.

        @param scale: the new scale notes to use.
    */
    void setScale(const std::vector<int> scale) noexcept;

    float processSample(float sample) noexcept;

private:
    float getSignMultiplier(bool positive) noexcept;

    bool enabled = true;
    std::vector<int> currentScale;

    float remappedSample = 0.0f;
    bool isPositive = true;

    int currentOctave = 0;
    int currentNote = 0;
    int selectedNote = 0;

    int octavesPerVolt = 5;
    int notesPerOctave = 12;
};
