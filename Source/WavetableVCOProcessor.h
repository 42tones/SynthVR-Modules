/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include "JuceHeader.h"
#include "BaseProcessor.h"
#include "ParameterUtils.h"
#include "WaveUtils.h"

using namespace juce;

namespace synthvr
{
    class WavetableVCOProcessor : public BaseProcessor {
    public:
        WavetableVCOProcessor();
        ~WavetableVCOProcessor();

        void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void releaseResources() override;
        void processBlock (AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "WavetableVCOProcessor"; }
    protected:
        float calculateFrequency(float coarseTune, float fineTune, float offsetTune, float detune, float fmInput, float pitchInput);
        void calculateCrossfade(float mix);
        void computeWaveTables();
        void updateFrequencies(float fmInput, float pitchInput);
        void onStateUpdated() override { waveTablesShouldBeRecomputed = true; }
        void onFloatArrayDataUpdated() override { waveTablesShouldBeRecomputed = true; }

        // Tuning params
        AudioParameterInt* coarseTuneParam;
        AudioParameterFloat* fineTuneParam;
        AudioParameterInt* coarseOffsetAParam;
        AudioParameterInt* coarseOffsetBParam;
        AudioParameterFloat* detuneParam;
        
        // Control params
        AudioParameterFloat* gainParam;
        AudioParameterFloat* fmAmountParam;
        AudioParameterFloat* oscMixParam;

        // Unity utility params
        AudioParameterFloat* oscAWave;
        AudioParameterFloat* oscBWave;
        AudioParameterFloat* totalWaveMixDisplay;
        
        // DSP units
        std::unique_ptr<WaveTableOsc> oscA = std::make_unique<WaveTableOsc>();
        std::unique_ptr<WaveTableOsc> oscB = std::make_unique<WaveTableOsc>();
        dsp::Gain<float> masterGain;
        
        // Frequency related vars
        float freqToSet = 110.0f;
        float baseFrequency = 110.0f;
        int coarseTuneRange = 12;
        float detuneRange = 0.5f;
        double sampleRate = 44100;

        // Oscillator related vars
        int defaultWaveTableLength = 8;
        bool waveTablesShouldBeRecomputed = false;
        float currentOscASample = 0.0f;
        float currentOscBSample = 0.0f;
        float oscATargetGain = 0.0f;
        float oscBTargetGain = 0.0f;

        // Enums to more safely refer to channels (channel num typo could cause crash!)
        enum
        {
            fmInputChannel = 0,
            pitchInputChannel = 1,
            waveInputChannel = 2
        };

        enum
        {
            oscAOutputChannel = 0,
            oscBOutputChannel = 1,
            mixOutputChannel = 2
        };

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableVCOProcessor)
    };
}
