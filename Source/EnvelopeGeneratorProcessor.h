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
    class EnvelopeGeneratorProcessor : public BaseProcessor {
    public:
        EnvelopeGeneratorProcessor();
        ~EnvelopeGeneratorProcessor();

        void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void processBlock (AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "EnvelopeGeneratorProcessor"; }
    private:
        AudioParameterFloat* attack;
        AudioParameterFloat* decay;
        AudioParameterFloat* sustain;
        AudioParameterFloat* release;
        AudioParameterFloat* speed;
        AudioParameterFloat* outputDisplay;

        ADSR envelope;
        ADSR::Parameters envelopeParams;
        bool envelopeIsOn = false;
        bool sampleIsTrigger = false;

        float maxValue = 4.0f;
        float minValue = 0.001f;
        float defaultCenterValue = 0.5f;
        float maxCVInfluence = 2.0f;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeGeneratorProcessor)
    };
}
