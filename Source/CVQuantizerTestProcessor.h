/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "BaseProcessor.h"
#include "ParameterUtils.h"
#include <vector>

using namespace juce;
using namespace std;

namespace synthvr
{
    class CVQuantizerTestProcessor : public BaseProcessor {
    public:
        CVQuantizerTestProcessor();
        ~CVQuantizerTestProcessor();

        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "CVQuantizerTestProcessor"; }
    private:
        float getSignMultiplier(bool positive);

        AudioParameterChoice* scale;

        StringArray scaleNames = { 
            "Minor", 
            "Major", 
            "Ionian", 
            "Dorian", 
            "Phrygian", 
            "Lydian", 
            "Mixolydian", 
            "Aeolian", 
            "Locrian", 
            "Whole-Tone", 
            "Octaves", 
            "Chromatic" 
        };

        vector<vector<int>> scaleNotes {
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

        int remappedSample = 0.0f;
        bool isPositive = true;

        int octaveInSample = 0;
        int noteInSample = 0;
        int selectedNote = 0;

        int octavesInRange = 5;
        int notesInOctave = 12;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CVQuantizerTestProcessor)
    };
}
