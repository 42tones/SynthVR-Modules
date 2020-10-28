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
        void UpdateModulation(juce::AudioSampleBuffer& buffer, int sample);
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
        dsp::LookupTableTransform<float> saturationLUT{ [](float x) { return std::tanh(x); },
                                                    -5.0f, 5.0f, 128 };


        // Settings / defaults
        float defaultSpeedCenterSeconds = 0.25f;
        float defaultSpeedSmoothing = 0.2f;
        float defaultHighShelfFrequency = 4000.0f;
        float defaultLowShelfFrequency = 800.0f;
        float defaultColorQFactor = 0.6f;
        float defaultColorFactor = 0.7f;

        float minDelaySpeedSamples = 50.0f;
        float maxDelaySpeedSamples = 220500.0f;
        float maxDelaySpeedSeconds = 5.0f;
        float maxFeedback = 1.1f;

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
