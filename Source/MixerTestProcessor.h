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

using namespace juce;

namespace synthvr
{
    class MixerTestProcessor : public BaseProcessor {
    public:
        MixerTestProcessor();
        ~MixerTestProcessor();

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void releaseResources() override;
        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "MixerTestProcessor"; }
    private:
        float getChannelGain(int channel);
        float getChannelPan(int channel);
        void writeGainsForStereoPanning(float pan);

        // Parameters
        AudioParameterFloat* masterGainAmount;

        int numChannels = 4;
        std::vector<int> gainParameterIndices;
        std::vector<int> panParameterIndices;

        // DSP
        AudioBuffer<float> outputBuffer;
        float rampDuration = 0.1f;
        dsp::Gain<float> masterGainProcessor;
        std::vector<dsp::Gain<float>> channelGainProcessors;

        // Preallocated temporary variables
        float tempSample;
        float tempPan;
        float tempGainLeft;
        float tempGainRight;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerTestProcessor)
    };
}
