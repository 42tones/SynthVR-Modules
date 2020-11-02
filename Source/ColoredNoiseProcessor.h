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
    class ColoredNoiseProcessor : public BaseProcessor {
    public:
        ColoredNoiseProcessor();
        ~ColoredNoiseProcessor();

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "DelayProcessor"; }
    private:
        void recalculateFilterCoefficients(float colorValue);

        // Processor parameters
        AudioParameterFloat* colorParam;
        AudioParameterFloat* levelParam;

        // DSP elements
        Random randomGenerator;
        dsp::IIR::Filter<float> highFilter;
        dsp::IIR::Filter<float> lowFilter;

        // Settings / defaults
        float defaultCenterFrequency = 1000.0f;
        float defaultColorQFactor = 0.1f;
        float defaultColorFactor = 0.8f;
        float defaultLevelSmoothing = 0.1f;
        double sampleRate = 1000.0f;

        float currentColoredNoise = 0.0f;
        float currentTargetLevel = 0.0f;
        SmoothedValue<float> smoothedLevel;

        // Channel name helpers
        enum
        {
            colorInputChannel = 0,
            levelInputChannel = 1,
            whiteNoiseOutputChannel = 0,
            coloredNoiseOutputChannel = 1,
        };

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColoredNoiseProcessor)
    };
}
