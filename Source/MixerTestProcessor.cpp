/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "MixerTestProcessor.h"

using namespace synthvr;

MixerTestProcessor::MixerTestProcessor() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(8))
    .withOutput("Output", AudioChannelSet::discreteChannels(2)))
{
    // Add master parameters
    addParameter(masterGainAmount = new AudioParameterFloat("masterGainAmount", "The volume of the main output.", 0.0f, 1.0f, 1.0f));
    gainParameterIndices = std::vector<int>();
    panParameterIndices = std::vector<int>();

    // Add gain & pan parameters for all channels
    for (int i = 0; i < numChannels; i++)
    {
        addParameter(new AudioParameterFloat("channelGain_" + std::to_string(i), "Sets the gain of a channel.", 0.0f, 1.0f, 1.0f));
        addParameter(new AudioParameterFloat("channelPan_" + std::to_string(i), "Sets the panning of a channel", 0.0f, 1.0f, 0.5f));
        gainParameterIndices.push_back((i * 2) + 1);
        panParameterIndices.push_back((i * 2) + 2);
    }

    // Set up master gain
    masterGainProcessor.setGainLinear(*masterGainAmount);
    masterGainProcessor.setRampDurationSeconds(rampDuration);

    // Set up channel gain
    for (int i = 0; i < numChannels; i++)
    {
        channelGainProcessors.push_back(dsp::Gain<float>());
        channelGainProcessors[i].setGainLinear(getChannelGain(i));
        channelGainProcessors[i].setRampDurationSeconds(rampDuration);
    }
}

MixerTestProcessor::~MixerTestProcessor() {}

void MixerTestProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    // Prepare gain processors
    dsp::ProcessSpec processSpec{ sampleRate, static_cast<uint32> (maximumExpectedSamplesPerBlock) };
    masterGainProcessor.prepare(processSpec);

    for (int i = 0; i < numChannels; i++)
        channelGainProcessors[i].prepare(processSpec);

    // Make temporary buffer for writing output to
    outputBuffer = AudioBuffer<float>(2, maximumExpectedSamplesPerBlock);
}

void MixerTestProcessor::releaseResources() {}

void MixerTestProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    outputBuffer.clear();

    for (int channel = 0; channel < numChannels; channel++)
    {
        channelGainProcessors[channel].setGainLinear(getChannelGain(channel));
        tempPan = getChannelPan(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            tempSample = channelGainProcessors[channel].processSample(buffer.getSample(channel, sample));
            writeGainsForStereoPanning(tempPan + buffer.getSample(channel + numChannels, sample));
            outputBuffer.addSample(0, sample, tempSample * tempGainLeft);
            outputBuffer.addSample(1, sample, tempSample * tempGainRight);
        }
    }

    // Multiply outbut buffer by master gain
    masterGainProcessor.setGainLinear(*masterGainAmount);
    for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++)
    {
        for (int sample = 0; sample < outputBuffer.getNumSamples(); sample++)
        {
            // Do gain and add it directly to output buffer
            tempSample = masterGainProcessor.processSample(outputBuffer.getSample(channel, sample));
            buffer.setSample(channel, sample, tempSample);
        }
    }
}

float MixerTestProcessor::getChannelGain(int channel)
{
    return getParameters()[gainParameterIndices[channel]]->getValue();
}

float MixerTestProcessor::getChannelPan(int channel)
{
    return getParameters()[panParameterIndices[channel]]->getValue();
}

void MixerTestProcessor::writeGainsForStereoPanning(float pan)
{
    tempGainLeft = dsp::FastMathApproximations::sin(pan * M_PI_2);
    tempGainRight = dsp::FastMathApproximations::cos(pan * M_PI_2);
}