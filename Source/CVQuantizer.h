#include "JuceHeader.h"
#include <algorithm>

using namespace juce;

enum class MusicalScale
{
    Minor,
    Major,
    Ionian,
    Dorian,
    Phrygian,
    Lydian,
    Mixolydian,
    Aeolian,
    Locrian,
    WholeTone,
    Octaves,
    Chromatic
};

static std::vector<std::vector<int>> musicalScaleNotes{
    {0,2,3,5,7,8,10},  // pentatonic minor
    {0,2,4,5,7,9,10},  // pentatonic major
    {0,2,4,5,7,9,11},  // ionian (natural major)
    {0,2,3,5,7,9,10},  // dorian
    {0,1,3,5,7,8,10},  // phrygian
    {0,2,4,6,7,9,11},  // lydian
    {0,2,4,5,7,9,10},  // mixolydian
    {0,2,3,5,7,8,10},  // aeolian
    {0,1,3,5,6,8,10},  // locrian
    {0,2,4,6,8,10},    // whole-tone
    {0},               // octaves 
    {0,1,2,3,4,5,6,7,8,9,10,11}  // chromatic 
};

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

    /** Sets the scale of the quantizer.

    @param scale: the new scale to use.
    */
    void setScale(MusicalScale scale) noexcept;

    float processSample(float sample) noexcept;

    static std::vector<int> makeScale(MusicalScale scale) noexcept;

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
