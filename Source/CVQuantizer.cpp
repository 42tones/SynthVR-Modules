#include "CVQuantizer.h"

CVQuantizer::CVQuantizer()
{
    // Use chromatic scale
    auto chromaticScale = std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    setScale(chromaticScale);
}

void CVQuantizer::setScale(const std::vector<int> scale) noexcept
{
    // Reject scales with no notes or too many notes
    if (scale.size() < 1 || scale.size() > 12)
        return;

    // Copy scale and ensure all notes to be in same octave range (0-11)
    currentScale = scale;
    for (int note = 0; note < currentScale.size(); note++)
        currentScale[note] = currentScale[note] % 12;
}

void CVQuantizer::setScale(MusicalScale scale) noexcept
{
    auto scaleToUse = makeScale(scale);
    setScale(scaleToUse);
}

float CVQuantizer::processSample(float sample) noexcept
{
    if (!enabled)
        return sample;

    // Map sample into a positive 5 octave range
    remappedSample = sample * notesPerOctave * octavesPerVolt;
    remappedSample = std::round(remappedSample);
    remappedSample = std::abs(remappedSample);

    // Get the desired octave and note from remapped signal
    currentOctave = remappedSample / notesPerOctave;
    currentNote = (int)remappedSample % notesPerOctave;

    // Find the best matching note in desired scale
    for (int note = 0; note < currentScale.size(); note++)
    {
        if (currentNote == currentScale[note])
        {
            selectedNote = currentScale[note];
            break;
        }
        else if (currentNote >= currentScale[note])
            selectedNote = currentScale[note];
    }

    // Map selection back to normalized float range
    // TODO: This is hard to read. Refactor later.
    return  (
                (
                    (float) (currentOctave * notesPerOctave + selectedNote)
                    / (float) notesPerOctave
                )
                / (float) octavesPerVolt
            )
            * getSignMultiplier(sample >= 0.0f);
}

float CVQuantizer::getSignMultiplier(bool positive) noexcept
{
    if (positive)
        return 1.0f;
    else
        return -1.0f;
}

std::vector<int> CVQuantizer::makeScale(MusicalScale scale) noexcept
{
    return musicalScaleNotes[(int)scale];
}
