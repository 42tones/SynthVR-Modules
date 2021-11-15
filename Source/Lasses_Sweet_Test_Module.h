/*
  ==============================================================================
    This is a module which takes audio input on channel 0, and CV input on channel 1.
    Intented use e.g. tremolo effect
  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "BaseProcessor.h"
#include "ParameterUtils.h"

using namespace juce;

namespace synthvr
{
    class Lasses_Sweet_Test_Module : public BaseProcessor {
    public:
        Lasses_Sweet_Test_Module();
        ~Lasses_Sweet_Test_Module();

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "Lasses_Sweet_Test_Module"; }

    private:
        float normalizeGainCVInput(float sampleAmplitude);

        dsp::Gain<float> gain;
        AudioParameterFloat* gainParam;

        AudioParameterFloat* gainCVAmountParam;

        SmoothedValue<float> smoothedGainCVAmountParam;
        float paramSmoothTime;

        float normalizedGainCVModInput;

        enum
        {
            inputChannel = 0,
            outputChannel = 0,
            gainCVModInput = 1
        };

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Lasses_Sweet_Test_Module)
    };
}
