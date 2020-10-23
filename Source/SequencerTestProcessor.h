/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "BaseProcessor.h"
#include <algorithm>

using namespace juce;

namespace synthvr
{
    class SequencerTestProcessor : public BaseProcessor {
    public:
        SequencerTestProcessor();
        ~SequencerTestProcessor();

        void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void processBlock (AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "SequencerTestProcessor"; }
    private:
        void ResetState();
        void HandleClockTrigger();

        AudioParameterFloat* gateLength;
        AudioParameterInt* clockDivision;
        std::vector<int> stepsPitchIndices;
        std::vector<int> stepsOnIndices;
        AudioParameterInt* currentStepDisplay;
        AudioParameterBool* currentlyTriggeredDisplay;

        int numSteps = 8;
        int currentStep = -1;

        float currentPitchValue = 0.0f;
        float previousClockValue = 0.0f;
        float previousResetValue = 0.0f;

        bool currentlyTriggered = false;
        int ticksSinceLastSequencerTrigger = 0;
        int samplesSinceLastOutputTrigger = 0;

        int currentClockTriggerRate = 512;
        int samplesSinceLastClockTrigger = 0;
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerTestProcessor)
    };
}
