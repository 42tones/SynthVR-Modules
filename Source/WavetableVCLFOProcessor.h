/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "BaseProcessor.h"
#include "ParameterUtils.h"
#include "WaveUtils.h"

using namespace juce;

namespace synthvr
{
    class WavetableVCLFOProcessor : public BaseProcessor {
    public:
        WavetableVCLFOProcessor();
        ~WavetableVCLFOProcessor();

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void releaseResources() override;
        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "WavetableVCOProcessor"; }
    protected:
        void computeWaveTable();
        void onStateUpdated() override { waveTableShouldBeRecomputed = true; }
        void onFloatArrayDataUpdated() override { waveTableShouldBeRecomputed = true; }

        // Tuning params
        AudioParameterFloat* frequencyParam;
        AudioParameterFloat* fmAmountParam;
        AudioParameterFloat* oscWaveParam;
        AudioParameterBool* bipolarParam;
        AudioParameterFloat* oscOutputDisplayParam;

        // DSP units
        std::unique_ptr<WaveTableOsc> osc = std::make_unique<WaveTableOsc>();

        // Defaults
        int defaultWaveTableLength = 8;

        // State
        double currentSampleRate = 1000.0f;
        float currentFrequency = 0.0f;
        SmoothedValue<float> smoothedFrequency;
        float currentResetValue = 0.0f;
        float previousResetValue = 0.0f;
        float oscillatorOutput = 0.0f;
        bool waveTableShouldBeRecomputed = false;

        // Channel setup enum
        enum
        {
            fmInputChannel = 0,
            resetInputChannel = 1,
            outputChannel = 0
        };

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WavetableVCLFOProcessor)
    };
}
