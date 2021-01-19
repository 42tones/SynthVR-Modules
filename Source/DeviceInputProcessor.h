/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "BaseProcessor.h"
#include "ParameterUtils.h"
#include "StoreChannelAudioCallback.h"

using namespace juce;

namespace synthvr
{
    class DeviceInputProcessor : public BaseProcessor {
    public:
        DeviceInputProcessor();
        ~DeviceInputProcessor();

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void releaseResources() override;
        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "DeviceInputProcessor"; }
    private:
        // Parameters
        AudioParameterFloat* gainParam;
        AudioParameterInt* selectedChannelParam;

        // DSP
        AudioDeviceManager deviceManager = AudioDeviceManager();
        StoreChannelAudioCallback callback = StoreChannelAudioCallback();

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceInputProcessor)
    };
}
