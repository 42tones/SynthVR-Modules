/*
  ==============================================================================
    Choose highpass or lowpass filter
    12 dB/oct or 24 dB/Oct
    Stereo input
    Stereo output
    CV input: modulate cutoff frequency
    CV input: modulate resonance
  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "BaseProcessor.h"
#include "ParameterUtils.h"
#include "SingleSampleLadderFilter.h"

using namespace juce;

namespace synthvr
{
    class HighpassLowpassStereo : public BaseProcessor {
    public:
        HighpassLowpassStereo();
        ~HighpassLowpassStereo();

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void releaseResources() override;
        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "HighpassLowpassStereo"; }

    private:
        SingleSampleLadderFilter<float> filterProcessorLeft;
        SingleSampleLadderFilter<float> filterProcessorRight;

        AudioParameterFloat* frequencyParam;
        AudioParameterFloat* resonanceParam;
        AudioParameterFloat* frequencyCVModParam;
        AudioParameterFloat* resonanceCVModParam;
        AudioParameterInt* highpassLowpassToggleParam;
        AudioParameterInt* filterRollOff12or24Param;

        float paramSmoothingTime = 0.04f;
        SmoothedValue<float> smoothedFrequencyParam;
        SmoothedValue<float> smoothedResonanceParam;
        SmoothedValue<float> smoothedFrequencyCVModParam;
        SmoothedValue<float> smoothedResonanceCVModParam;

        // Defaults
        float frequencySkew = 0.2f;
        float minFrequency = 20.0f;
        float maxFrequency = 20000.0f;
        float resonanceSkew = 1.5f;
        float minResonance = 0.0f;
        float maxResonance = 0.99f;
        float driveAmount = 3.0f;

        int currentFilterMode = 0;
        int calculatedFilterMode = 0;
        float currentFrequency = 0;
        float currentResonance = 0;
        float currentFilterOutput = 0;

        enum class InOut
        {
            inputLeft = 0,
            inputRight = 1,
            frequencyCVModInput = 2,
            resonanceCVModInput = 3,
            outputLeft = 0,
            outputRight = 1,
        };

        int calculateFilterMode(int highpassLowpassToggle, int rollOff12or24);
        void ProcessSingleSampleLadderFilter(AudioBuffer<float>& buffer, SingleSampleLadderFilter<float>* thisFilterProcessor, InOut whichInput, InOut whichOutput);
        
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HighpassLowpassStereo)
    };
}
