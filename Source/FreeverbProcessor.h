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
    class FreeverbProcessor : public BaseProcessor {
    public:
        FreeverbProcessor();
        ~FreeverbProcessor();

        void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void processBlock (AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "FreeverbProcessor"; }
    private:
        AudioParameterFloat* roomSizeParam;
        AudioParameterFloat* roomSizeModulationAmountParam;
        AudioParameterFloat* dampingParam;
        AudioParameterFloat* widthParam;
        AudioParameterFloat* mixParam;
        AudioParameterFloat* mixModulationAmountParam;
        AudioParameterBool* freezeParam;
        AudioParameterFloat* freezeDisplay;

        Reverb reverb;
        Reverb::Parameters reverbParams;

        float mixValue;

        enum
        {
            inputChannelLeft = 0,
            inputChannelRight = 1,
            outputChannelLeft = 0,
            outputChannelRight = 1,
            roomSizeInputChannel = 2,
            mixInputChannel = 3,
            freezeInputChannel = 4
        };


        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FreeverbProcessor)
    };
}
