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
    class SignalSplitterProcessor : public BaseProcessor {
    public:
        SignalSplitterProcessor();
        ~SignalSplitterProcessor();

        void processBlock (AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "SignalSplitterProcessor"; }
    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SignalSplitterProcessor)
    };
}
