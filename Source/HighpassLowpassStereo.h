/*
  ==============================================================================
    Choose highpass or lowpass filter
    Stereo input
    Stereo output
    CV input: modulate cutoff frequency
    CV input: modulate resonance
  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "BaseProcessor.h"
#include "ParameterUtils.h"

using namespace juce;

namespace synthvr
{
    class HighpassLowpassStereo : public BaseProcessor {
    public:
        HighpassLowpassStereo();
        ~HighpassLowpassStereo();

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "HighpassLowpassStereo"; }

    private:
      

        enum
        {
            inputLeft = 0,
            inputRight = 1,
            frequencyCVModInput = 2,
            resonanceCVModInput = 3,
            outputLeft = 0,
            outputRight = 1,
        };

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HighpassLowpassStereo)
    };
}
