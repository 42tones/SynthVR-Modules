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

        @param notes the new scale notes to use.
    */
    void setScale(std::vector<int> notes) noexcept;

    float processSample(float sample) noexcept;

private:
    bool enabled = true;
    std::vector<int> currentScale;

    int remappedSample = 0.0f;
    bool isPositive = true;

    int octaveInSample = 0;
    int noteInSample = 0;
    int selectedNote = 0;

    int octavesInRange = 5;
    int notesInOctave = 12;
};
