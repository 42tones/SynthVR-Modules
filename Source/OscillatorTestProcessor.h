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
    class OscillatorTestProcessor : public BaseProcessor {
    public:
        OscillatorTestProcessor();
        ~OscillatorTestProcessor();

        void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void releaseResources() override;
        void processBlock (AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "OscillatorTestProcessor"; }
    private:
        float calculateFrequency(float param, float fmInput);
        
        // Parameters
        AudioParameterFloat* freqParam;
        AudioParameterFloat* gainParam;
        AudioParameterFloat* fmParam;
        
        dsp::Oscillator<float> osc;
        dsp::Gain<float> gain;
        float freqToSet = 440.0f;

        float minFrequency = 1.0f;
        float maxFrequency = 2217.46f;
        float defaultCenterFrequency = 220.0f;
        float maxCVInfluence = 500.0f;

        float freqParamSmoothTime = 0.1f;
        SmoothedValue<float> smoothedFreqParam;
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscillatorTestProcessor)
    };
}
