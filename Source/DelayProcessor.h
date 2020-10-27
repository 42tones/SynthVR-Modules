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

        // Processor parameters
        AudioParameterFloat* timeParam;
        AudioParameterFloat* timeModulationAmountParam;
        AudioParameterFloat* feedbackParam;
        AudioParameterFloat* feedbackModulationAmountParam;
        AudioParameterFloat* mixParam;
        AudioParameterFloat* mixModulationAmountParam;
        AudioParameterFloat* colorParam;

        // DSP elements
        dsp::DelayLine<float, dsp::DelayLineInterpolationTypes::Linear> delay {441000};
        dsp::IIR::Filter<float> filter;

        // Settings / defaults
        float defaultSpeedCenterSeconds = 0.5f;
        float defaultHighShelfFrequency = 4000.0f;
        float defaultLowShelfFrequency = 1000.0f;
        float defaultColorQFactor = 0.5f;
        float defaultColorFactor = 0.5f;

        float minDelaySpeedSamples = 5.0f;
        float maxDelaySpeedSamples = 441000.0f;
        float maxDelaySpeedSeconds = 10.0f;
        float maxFeedback = 0.9999f;

        // Current processor state
        double sampleRate = 1000.0f;
        float currentDelayInput = 0.0f;
        float currentDelayOutput = 0.0f;
        float currentDelayInSamples = 0.0f;
        float currentMix = 0.0f;
        float currentFeedback = 0.0f;
        SmoothedValue<float> smoothedDelaySpeedSamples;

        // Channel name helpers
        enum
        {
            inputChannel = 0,
            timeChannel = 1,
            feedbackChannel = 2,
            mixChannel = 3,
            returnChannel = 4,
            outputChannel = 0,
            sendChannel = 1
        };

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayProcessor)
    };
}
