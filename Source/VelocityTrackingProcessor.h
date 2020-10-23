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
    class VelocityTrackingProcessor : public BaseProcessor {
    public:
        VelocityTrackingProcessor();
        ~VelocityTrackingProcessor();

        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
        //==============================================================================
        const String getName() const override { return "VelocityTrackingProcessor"; }
    private:
        void resetSmoothers();

        AudioParameterFloat* smoothness;
        AudioParameterFloat* strength;
        AudioParameterFloat* xActual;
        AudioParameterFloat* yActual;
        AudioParameterFloat* zActual;

        float currentSmoothness;
        double currentSampleRate;
        SmoothedValue<float, ValueSmoothingTypes::Linear> xSmooth;
        SmoothedValue<float, ValueSmoothingTypes::Linear> ySmooth;
        SmoothedValue<float, ValueSmoothingTypes::Linear> zSmooth;

        enum
        {
            outX = 0,
            outXInverted = 1,
            outY = 2,
            outYInverted = 3,
            outZ = 4,
            outZInverted = 5,
        };

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VelocityTrackingProcessor)
    };
}
