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
    class MatrixMixerProcessor : public BaseProcessor {
    public:
        MatrixMixerProcessor();
        ~MatrixMixerProcessor();

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
        void releaseResources() override;
        void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
        //==============================================================================
        const String getName() const override { return "MatrixMixerProcessor"; }
    private:
        // Parameters
        AudioParameterBool* isBipolarParam;
        std::vector<std::vector<int>> gainParamIndices;

        // DSP
        std::vector<std::vector<dsp::Gain<float>>> gainProcessors;
        float defaultGainRampTime = 0.05f;
        float maxGainAmount = 1.5f;
        int numChannels = 4;

        // State
        float currentGain = 0.0f;
        float currentSample = 0.0f;
        std::vector <std::vector<float>> currentSamples
        {
            {0.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f}
        };

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MatrixMixerProcessor)
    };
}
