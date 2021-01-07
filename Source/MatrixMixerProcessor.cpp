/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "MatrixMixerProcessor.h"

using namespace synthvr;

MatrixMixerProcessor::MatrixMixerProcessor() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(numChannels))
    .withOutput("Output", AudioChannelSet::discreteChannels(numChannels)))
{
    // Parameters
    addParameter(isBipolarParam = new AudioParameterBool("bipolar", "Bipolar", false));

    // Add gain params and processors
    auto paramIndexOffset = getNumParameters();
    for (int outputChannel = 0; outputChannel < numChannels; outputChannel++)
    {
        auto outputChannelIndices = std::vector<int>();
        auto outputChannelGains = std::vector<dsp::Gain<float>>();

        for (int inputChannel = 0; inputChannel < numChannels; inputChannel++)
        {
            // Add gain parameter
            addParameter(
                new AudioParameterFloat(
                    "in_" + std::to_string(inputChannel) + "_out_" + std::to_string(outputChannel),
                    "Gain", 
                    -0.1f, 
                    maxGainAmount, 
                    1.0f
                )
            );
            outputChannelIndices.push_back(outputChannel + inputChannel + paramIndexOffset);

            // Add gain processor
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
    for (int outputChannel = 0; outputChannel < numChannels; outputChannel++)
        for (int inputChannel = 0; inputChannel < numChannels; inputChannel++)
            this->gainProcessors[outputChannel][inputChannel].prepare(processSpec);
}

void MatrixMixerProcessor::releaseResources() {}

void MatrixMixerProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    for (int outputChannel = 0; outputChannel < numChannels; outputChannel++)
    {
        // Adjust the input gain processors for this output (at block time)
        for (int inputChannel = 0; inputChannel < numChannels; inputChannel++)
        {
            currentGainParam = static_cast<AudioParameterFloat*>(
                getParameters()[gainParamIndices[outputChannel][inputChannel]]);
            currentGain = ParameterUtils::clamp(*currentGainParam, 0.0f, maxGainAmount);
            if (*isBipolarParam)
                currentGain = (currentGain * 2) - (maxGainAmount / 2);

            gainProcessors[outputChannel][inputChannel].setGainLinear(currentGain);
        }

        // Process the buffer for this output
        for (int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            // Mix all input channels for this output channel
            currentSample = 0.0f;
            for (int inputChannel = 0; inputChannel < numChannels; inputChannel++)
                currentSample += gainProcessors[outputChannel][inputChannel].processSample(
                    buffer.getSample(inputChannel, sample));

            buffer.setSample(outputChannel, sample, currentSample);
        }
    }
}