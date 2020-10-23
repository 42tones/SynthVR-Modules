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
    class SingleChannelTestProcessor : public BaseProcessor {
    public:
        SingleChannelTestProcessor();
        ~SingleChannelTestProcessor();

        void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void releaseResources() override;
        void processBlock (AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "SingleChannelTestProcessor"; }
    private:
        AudioParameterFloat* valueParameter;
        AudioParameterInt* intParameter;
        AudioParameterBool* boolParameter;
        AudioParameterChoice* choiceParameter;

        double expectedSampleRate;
        int expectedBlockSize;
        const StringArray choices = {"ChoiceOne", "ChoiceTwo", "ChoiceThree"};
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SingleChannelTestProcessor)
    };
}
