/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "MatrixMixerProcessor.h"

using namespace synthvr;

MatrixMixerProcessor::MatrixMixerProcessor() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(4))
    .withOutput("Output", AudioChannelSet::discreteChannels(4)))
{
    // Parameters
    addParameter(isBipolarParam = new AudioParameterBool("bipolar", "Bipolar", false));

    // Add gain params and processors
    for (int o = 0; o < numChannels; o++)
    {
        auto outputChannelIndices = std::vector<int>();
        auto outputChannelGains = std::vector<dsp::Gain<float>>();

        for (int i = 0; i < numChannels; i++)
        {
            // Add gain parameter
            addParameter(
                new AudioParameterFloat(
                    "in_" + std::to_string(i) + "_out_" + std::to_string(o),
                    "Gain", 
                    -0.1f, 
                    maxGainAmount, 
                    1.0f
                )
            );
            outputChannelIndices.push_back(getNumParameters() - 1);

            // Add gain processor initialized to 0
            auto gainProcessor = dsp::Gain<float>();
            gainProcessor.setGainLinear(0.0f);
            gainProcessor.setRampDurationSeconds(defaultGainRampTime);
            outputChannelGains.push_back(gainProcessor);
        }

        gainParamIndices.push_back(outputChannelIndices);
        gainProcessors.push_back(outputChannelGains);
    }
}

MatrixMixerProcessor::~MatrixMixerProcessor() {}

void MatrixMixerProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    // Prepare DSP modules
    dsp::ProcessSpec processSpec{ sampleRate, static_cast<uint32> (maximumExpectedSamplesPerBlock) };
    for (int o = 0; o < numChannels; o++)
        for (int i = 0; i < numChannels; i++)
            this->gainProcessors[o][i].prepare(processSpec);
}

void MatrixMixerProcessor::releaseResources() {}

void MatrixMixerProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    // Process each sample of the input
    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        //currentSamples.clear();
        for (int o = 0; o < numChannels; o++)
            for (int i = 0; i < numChannels; i++)
                currentSamples[o][i] = 0.0f;

        // Mix all input channels for each output channel
        for (int o = 0; o < numChannels; o++)
        {
            if (!isOutputConnected[o])
                continue;

            for (int i = 0; i < numChannels; i++)
            {
                if (!isInputConnected[i])
                    continue;

                // Update gain
                auto currentGainParam = static_cast<AudioParameterFloat*>(getParameters()[gainParamIndices[o][i]]);
                if (*isBipolarParam)
                    currentGain = (*currentGainParam * 2) - maxGainAmount;
                else
                    currentGain = ParameterUtils::clamp(*currentGainParam, 0.0f, maxGainAmount);

                gainProcessors[o][i].setGainLinear(currentGain);

                // Mix in sample
                currentSamples[o][i] = gainProcessors[o][i].processSample(buffer.getSample(i, sample));
               // currentSamples(o,i) = gainProcessors[o][i].processSample(buffer.getSample(i, sample));
            }
        }

        for (int o = 0; o < numChannels; o++)
        {
            currentSample = 0.0f;
            for (int i = 0; i < numChannels; i++)
                currentSample += currentSamples[o][i];
                
            //currentSample += currentSamples(o, i);

            buffer.setSample(o, sample, currentSample);
        }
    }
}