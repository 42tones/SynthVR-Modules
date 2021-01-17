/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "BaseProcessor.h"
#include "ParameterUtils.h"
#include "ClockDivider.h"

using namespace juce;

namespace synthvr
{
    class PrimeTimeProcessor : public BaseProcessor {
    public:
        PrimeTimeProcessor();
        ~PrimeTimeProcessor();

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void releaseResources() override;
        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "PrimeTimeProcessor"; }
    private:
        float calculateFrequency(float fmInput);

        // Parameters
        AudioParameterFloat* bpmParam;
        AudioParameterFloat* bpmModAmountParam;

        float defaultBPM = 120.0f;

        // DSP
        dsp::Oscillator<float> clock;
        std::vector<ClockDivider> dividers{
            ClockDivider(1),
            ClockDivider(2),
            ClockDivider(3),
            ClockDivider(4),
            ClockDivider(5),
            ClockDivider(6),
            ClockDivider(7),
            ClockDivider(8)
        };

        // State
        float currentSample = 0.0f;
        bool currentlyResetting = false;
        bool previouslyResetting = false;
        bool shouldReset = false;

        enum
        {
            extClockInputChannel = 0,
            speedInputChannel = 1,
            resetInputChannel = 2
        };
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PrimeTimeProcessor)
    };
}
