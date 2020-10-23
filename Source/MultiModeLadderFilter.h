#pragma once

#include "JuceHeader.h"
#include "BaseProcessor.h"
#include "ParameterUtils.h"
#include "SingleSampleLadderFilter.h"

using namespace juce;

namespace synthvr
{
    class MultiModeLadderFilter : public BaseProcessor {
    public:
        MultiModeLadderFilter();
        ~MultiModeLadderFilter();

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void releaseResources() override;
        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "MultiModeLadderFilter"; }
    private:
        float calculateMixModulation(
            float originalValue, 
            float modulationValue, 
            float modulationAmount,
            float clampLow = 0.0f,
            float clampHigh = 1.0f);

        float calculateFrequencyModulation(
            float originalValue,
            float modulationValue,
            float modulationAmount,
            float clampLow = 0.0f,
            float clampHigh = 1.0f);

        int calculateMode(int baseMode, bool rollOffMode)
        {
            // There are 3 12dB/oct modes (0,1,2) and 3 24db/oct modes (3,4,5)
            return baseMode + (rollOffMode * 3);
        }

        // Parameters
        AudioParameterFloat* mixParam;
        AudioParameterFloat* mixModulationAmountParam;
        AudioParameterFloat* frequencyParam;
        AudioParameterFloat* frequencyModulationAAmountParam;
        AudioParameterFloat* frequencyModulationBAmountParam;
        AudioParameterFloat* resonanceParam;
        AudioParameterFloat* driveParam;
        AudioParameterInt* modeParam;
        AudioParameterBool* rollOffParam;
        AudioParameterFloat* frequencyDisplay;

        float paramSmoothingTime = 0.04f;
        SmoothedValue<float> smoothedMixParam;
        SmoothedValue<float> smoothedFrequencyParam;
        SmoothedValue<float> smoothedResonanceParam;

        SingleSampleLadderFilter<float> filterProcessor;

        // Defaults
        float frequencySkew = 0.2f;
        float minFrequency = 20.0f;
        float maxFrequency = 20000.0f;
        float resonanceSkew = 1.5f;
        float minResonance = 0.0f;
        float maxResonance = 0.99f;
        float maxDrive = 5.0f;

        int currentMode = 0;
        float currentFrequency = 0;
        float currentResonance = 0;
        float currentMix = 0;
        float currentFilterOutput = 0;

        enum
        {
            inputChannel = 0,
            outputChannel = 0,
            frequencyAInputChannel = 1,
            frequencyBInputChannel = 2,
            mixInputChannel = 3,
        };
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiModeLadderFilter)
    };
}
