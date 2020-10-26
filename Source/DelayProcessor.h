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
    class DelayProcessor : public BaseProcessor {
    public:
        DelayProcessor();
        ~DelayProcessor();

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "DelayProcessor"; }
    private:
        dsp::IIR::Coefficients<float>::Ptr calculateFilterCoefficientsFromColor(float colorValue);

        AudioParameterFloat* timeParam;
        AudioParameterFloat* timeModulationAmountParam;
        AudioParameterFloat* feedbackParam;
        AudioParameterFloat* feedbackModulationAmountParam;
        AudioParameterFloat* mixParam;
        AudioParameterFloat* mixModulationAmountParam;
        AudioParameterFloat* colorParam;

        dsp::DelayLine<float, dsp::DelayLineInterpolationTypes::Linear> delay;
        dsp::IIR::Filter<float> filter;
        dsp::IIR::Coefficients<float>::Ptr filterCoefficients;

        float defaultSpeedCenterMs = 500.0f;
        float defaultHighShelfFrequency = 4000.0f;
        float defaultHighShelfQFactor = 0.3f;
        float defaultHighShelfColorFactor = 0.5f;

        double sampleRate = 1000.0f;

        enum
        {
            inputChannel = 0,
            returnChannel = 1,
            speedChannel = 2,
            feedbackChannel = 3,
            mixChannel = 4,
            outputChannel = 0,
            sendChannel = 1
        };


        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayProcessor)
    };
}
