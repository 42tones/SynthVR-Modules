#pragma once

#include "JuceHeader.h"
#include "BaseProcessor.h"
#include "ParameterUtils.h"
#include "SingleSampleLadderFilter.h"

using namespace juce;

namespace synthvr
{
    class UtilityFilter : public BaseProcessor {
    public:
        UtilityFilter();
        ~UtilityFilter();

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void releaseResources() override;
        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "UtilityFilter"; }

    private:
        dsp::IIR::Filter<float> lowpassLeft;
        dsp::IIR::Filter<float> lowpassRight;
        dsp::IIR::Coefficients<float>::Ptr lowpassCoeffecients;

        dsp::IIR::Filter<float> highpassLeft;
        dsp::IIR::Filter<float> highpassRight;
        dsp::IIR::Coefficients<float>::Ptr highpassCoeffecients;

        AudioParameterFloat* colorParam;
        AudioParameterFloat* resonanceParam;

        float paramSmoothingTime = 0.04f;
        SmoothedValue<float> smoothedFrequencyParam;
        SmoothedValue<float> smoothedResonanceParam;

        // Defaults
        float minFrequency = 20.0f;
        float maxFrequency = 20000.0f;
        float frequencySkew = 3.0f;
        float qFactor = 2.0f;

        double sampleRate = 1000.0f;
        float currentColor = 0;
        float currentResonance = 0;
        float currentFilterOutput = 0;

        enum
        {
            inputLeft = 0,
            inputRight = 1,
            outputLeft = 0,
            outputRight = 1,
        };

        dsp::IIR::Coefficients<float>::Ptr calculateLowpassCoefficients(float color, float resonance);
        dsp::IIR::Coefficients<float>::Ptr calculateHighpassCoefficients(float color, float resonance);

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UtilityFilter)
    };
}
