/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "BaseProcessor.h"

using namespace juce;

namespace synthvr
{
    class AmplifierTestProcessor : public BaseProcessor {
    public:
        AmplifierTestProcessor();
        ~AmplifierTestProcessor();

        void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void releaseResources() override;
        void processBlock (AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "AmplifierTestProcessor"; }
    private:
        float calculateGain(float cvInput);

        // Parameters
        AudioParameterFloat* gainAmount;
        AudioParameterFloat* cvAmount;

        dsp::Gain<float> gainProcessor;

        enum
        {
            inOutChannel = 0,
            cvChannel = 1,
            displayChannel = 2
        };
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmplifierTestProcessor)
    };
}
