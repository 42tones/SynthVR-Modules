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
    class MasterClockProcessor : public BaseProcessor {
    public:
        MasterClockProcessor();
        ~MasterClockProcessor();

        void processBlock (AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "MasterClockProcessor"; }
    private:
        float calculateFrequency(float param, float fmInput);
    
        AudioParameterFloat* clockFrequency;
        AudioParameterFloat* frequencyModulation;
        dsp::Oscillator<float> clock;
        
        // ~1BPM at 32nd subdivisions
        float minClockFrequency = 0.1f;

        // ~200BPM at 32nd subdivisions
        float maxClockFrequency = 27.0f;

        // This is used when initializing skew such that setting parameter to 0.5 will result in this frequency
        float frequencyParameterCenterValue = 10.0f;

        // Preallocated float for frequency calculations in process block
        float freqToSet = frequencyParameterCenterValue;
        
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MasterClockProcessor)
    };
}
