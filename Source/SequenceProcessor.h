/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "BaseProcessor.h"
#include "ParameterUtils.h"
#include "CVQuantizer.h"
#include <algorithm>

using namespace juce;

namespace synthvr
{
    class SequenceProcessor : public BaseProcessor {
    public:
        SequenceProcessor();
        ~SequenceProcessor();

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void handleScaleUpdate();
        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        void writeOutputs(juce::AudioSampleBuffer& buffer, int sample);
        void handleStart();
        void handleReset();
        //==============================================================================
        const String getName() const override { return "SequenceProcessor"; }
    protected:
        void onStateUpdated() override { currentlyRunning = *currentlyRunningDisplay; }
    private:
        dsp::IIR::Coefficients<float>::Ptr calculateGlideFilterCoefficients();
        void handleNewClockTrigger();
        void HandleIncrementedStep();
        void updatePitch();
        void updateGate();
        bool getOnOffStatusForStep(int step);
        bool areAllStepsSkipped();
        int firstNonSkippedStep();
        int getNumPulsesForStep(int step);
        int getGateModeForStep(int step);
        float getGateLengthForMode(int mode);
        float getPitchForStep(int mode);
        bool incrementCurrentStepUntilEnd();
        MusicalScale getMusicalScale(int sequencerScale);

        // Parameters
        AudioParameterFloat* gateLengthParam;
        AudioParameterFloat* glideParam;
        AudioParameterBool* loopingParam;
        AudioParameterFloat* pitchExtentParam;
        AudioParameterInt* rootPitchParam;
        AudioParameterInt* pitchScaleParam;
        AudioParameterBool* toggleRunningParam;

        std::vector<int> stepsPitchIndices;
        std::vector<int> stepsOnIndices;
        std::vector<int> stepsGateModeIndices;
        std::vector<int> stepsPulseCountIndices;

        AudioParameterInt* currentStepDisplay;
        AudioParameterBool* currentlyTriggeredDisplay;
        AudioParameterBool* currentlyEOSTriggeredDisplay;
        AudioParameterBool* currentlyRunningDisplay;

        // DSP
        CVQuantizer quantizer;
        dsp::IIR::Filter<float> glideFilter;
        SmoothedValue<float> smoothedGlideFilterFrequency;
        float currentGlideFilterFrequency = 0.0f;

        // Defaults
        int numSteps = 8;
        float fullGlideFrequency = 50.0f;
        float noGlideFrequency = 1000.0f;
        float minTriggerDisplayLengthSamples = 5000.0f;

        // General state
        double currentSampleRate = 100.0f;
        int currentStep = 0;
        int currentPulse = 0;

        // Trigger state
        int samplesPerPulse = 0;
        int samplesSinceLastPulse = 0;
        int samplesSinceSecondLastPulse = 0;
        bool currentlyRunning = false;
        bool previouslyRunning = false;
        bool currentlyTriggered = false;
        bool previouslyTriggered = false;
        bool allStepsAreSkipped = false;
        bool currentlyReset = false;
        bool previouslyReset = false;
        bool previouslyToggledRunning = false;

        // Gate state
        float currentGateLengthSamples = 0.0f;
        float currentEndOfSequenceGateLengthSamples = 0.0f;
        float currentTriggerDisplayLengthSamples = 0.0f;
        int samplesSinceLastGate = 0;
        int samplesSinceLastEndOfSequenceGate = 0;
        int samplesSinceLastTriggerDisplay = 0;
        bool currentGateOpen = false;
        bool currentEndOfSequenceGateOpen = false;
        bool currentTriggerDisplayOpen = false;

        // Pitch state
        float currentPitch = 0.0f;
        float targetPitch = 0.0f;
        float previousGlide = 0.0f;
        float currentStepPitch = 0.0f;
        int currentSequencerScale = -1;

        // Scale enum
        enum SequencerScale
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
