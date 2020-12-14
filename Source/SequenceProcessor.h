/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "BaseProcessor.h"
#include "ParameterUtils.h"
#include <algorithm>

using namespace juce;

namespace synthvr
{
    class SequenceProcessor : public BaseProcessor {
    public:
        SequenceProcessor();
        ~SequenceProcessor();

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "SequenceProcessor"; }
    private:
        dsp::IIR::Coefficients<float>::Ptr calculateGlideFilterCoefficients();
        void handleNewClockTrigger();
        void updatePitch();
        void updateGate();
        bool getOnOffStatusForStep(int step);
        bool areAllStepsSkipped();
        int getNumPulsesForStep(int step);
        int getGateModeForStep(int step);
        float getGateLengthForMode(int mode);
        float getPitchForStep(int mode);

        // Parameters
        AudioParameterFloat* gateLengthParam;
        AudioParameterFloat* glideParam;
        AudioParameterBool* loopingParam;
        AudioParameterInt* rootPitchParam;
        AudioParameterInt* pitchScaleParam;

        std::vector<int> stepsPitchIndices;
        std::vector<int> stepsOnIndices;
        std::vector<int> stepsGateModeIndices;
        std::vector<int> stepsPulseCountIndices;

        AudioParameterInt* currentStepDisplay;
        AudioParameterBool* currentlyTriggeredDisplay;

        // DSP
        dsp::IIR::Filter<float> glideFilter;

        // Defaults
        int numSteps = 8;
        float fullGlideFrequency = 0.5f;
        float noGlideFrequency = 10.0f;

        // General state
        double currentSampleRate = 100.0f;
        int currentStep = 0;
        int currentPulse = 0;

        // Gate state
        int samplesPerPulse = 0;
        int samplesSinceLastPulse = 0;
        float currentGateLengthSamples = 0.0f;
        int samplesSinceLastGate = 0;
        bool currentGateOpen = false;
        bool currentEndOfSequenceGateOpen = false;

        // Trigger state
        bool currentlyStarted = true;
        bool endOfSequence = false;
        bool currentlyTriggered = false;
        bool previouslyTriggered = false;

        // Pitch state
        float currentPitch = 0.0f;
        float targetPitch = 0.0f;
        float previousGlide = 0.0f;

        // Scale enum
        enum
        {
            unscaled = 0,
            chromatic = 1,
            minor = 2,
            major = 3
        };

        // Pulse enum
        enum
        {
            silence = 0,
            singlePulse = 1,
            multiPulse = 2,
            holdForPulse = 3
        };

        // Channel enum
        enum
        {
            clockInputChannel = 0,
            startInputChannel = 1,
            stopInputChannel = 2,
            resetInputChannel = 3,
            triggerOutputChannel = 0,
            pitchOutputChannel = 1,
            endOfSequenceOutputChannel = 2
        };

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SequenceProcessor)
    };
}
