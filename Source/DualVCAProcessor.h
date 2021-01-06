/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "BaseProcessor.h"
#include "ParameterUtils.h"

using namespace juce;

namespace synthvr
{
    class DualVCAProcessor : public BaseProcessor {
    public:
        DualVCAProcessor();
        ~DualVCAProcessor();

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void releaseResources() override;
        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "AmplifierTestProcessor"; }
    private:
        // Parameters
        AudioParameterFloat* gainAmountA;
        AudioParameterFloat* cvAmountA;
        AudioParameterFloat* gainAmountB;
        AudioParameterFloat* cvAmountB;

        AudioParameterFloat* cvADisplay;
        AudioParameterFloat* outADisplay;
        AudioParameterFloat* cvBDisplay;
        AudioParameterFloat* outBDisplay;

        // DSP
        dsp::Gain<float> gainProcessorA;
        dsp::Gain<float> gainProcessorB;
        float defaultGainRampTime = 0.001f;

        enum
        {
            inputAAChannel = 0,
            inputABChannel = 1,
            cvAChannel = 2,
            inputBAChannel = 3,
            inputBBChannel = 4,
            cvBChannel = 5,
            outputAChannel = 0,
            outputBChannel = 1
        };
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DualVCAProcessor)
    };
}
