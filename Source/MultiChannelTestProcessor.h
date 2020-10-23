/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "BaseProcessor.h"

using namespace juce;

namespace synthvr
{
    class MultiChannelTestProcessor : public BaseProcessor {
    public:
        MultiChannelTestProcessor();
        ~MultiChannelTestProcessor();

        void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void releaseResources() override;
        void processBlock (AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "MultiChannelTestProcessor"; }
    private:
        AudioParameterFloat* startOfChannelParam;
        AudioParameterFloat* restOfChannelParam;

        double expectedSampleRate;
        int expectedBlockSize;
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiChannelTestProcessor)
    };
}
