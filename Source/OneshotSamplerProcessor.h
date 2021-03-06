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
    class OneshotSamplerProcessor : public BaseProcessor {
    public:
        OneshotSamplerProcessor();
        ~OneshotSamplerProcessor();

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void releaseResources() override;
        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "OneshotSamplerProcessor"; }
    protected:
        void handleTriggers(juce::AudioSampleBuffer& buffer, int sample);
        void handleStartPosition(juce::AudioSampleBuffer& buffer, int sample);
        void handleLength(juce::AudioSampleBuffer& buffer, int sample);
        void handlePitch(juce::AudioSampleBuffer& buffer, int sample);
        void handlePlayback(juce::AudioSampleBuffer& buffer, int sample);
        float getNextPosition(float currentPos, float pitch, float startPos, float length);
        float getSampleAtPosition(const std::vector<float> &samples, float position);
        void onFloatArrayDataUpdated() override { currentlyPlaying = false; }

        // Params
        AudioParameterFloat* bankParam;
        AudioParameterFloat* programParam;
        AudioParameterFloat* startParam;
        AudioParameterFloat* lengthParam;
        AudioParameterFloat* pitchParam;
        AudioParameterFloat* transientParam;
        AudioParameterFloat* volumeParam;
        AudioParameterBool* triggerParam;

        // Defaults
        int fadeOutSamples = 10;

        // State
        double currentSampleRate = 1000.0f;
        float currentPosition = 0.0f;
        float currentIncrement = 0.0f;
        bool currentlyPlaying = false;
        bool currentlyTriggered = false;
        bool previouslyTriggered = false;
        float currentStartPosition = 0.0f;
        float currentLength = 0.0f;

        SmoothedValue<float> smoothedPitch;
        float currentPitch = 0.0f;
        SmoothedValue<float> smoothedVolume;
        float currentVolume = 0.75f;
        SmoothedValue<float> transientShaper;
        SmoothedValue<float> antiClickFade;

        // Channel setup enum
        enum
        {
            triggerInputChannel = 0,
            startInputChannel = 1,
            lengthInputChannel = 2,
            pitchInputChannel = 3,
            volumeInputChannel = 4,
            outputChannelLeft = 0,
            outputChannelRight = 1
        };

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OneshotSamplerProcessor)
    };
}
