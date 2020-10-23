/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "MultiChannelTestProcessor.h"

using namespace synthvr;

MultiChannelTestProcessor::MultiChannelTestProcessor() : BaseProcessor(BusesProperties()
        .withInput("Inputs", AudioChannelSet::discreteChannels(2))
        .withOutput ("Output", AudioChannelSet::discreteChannels(3)))
{
    addParameter(startOfChannelParam = new AudioParameterFloat ("StartOfChannel", "Start Of Channel", -1.0f, 1.0f, 0.0f));
    addParameter(restOfChannelParam = new AudioParameterFloat ("RestOfChannel", "Rest Of Channel", -1.0f, 1.0f, 0.0f));
}

MultiChannelTestProcessor::~MultiChannelTestProcessor() {}

void MultiChannelTestProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    this->expectedSampleRate = sampleRate;
    this->expectedBlockSize = maximumExpectedSamplesPerBlock;
    this->enableAllBuses();
}

void MultiChannelTestProcessor::releaseResources() {}

void MultiChannelTestProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &)
{
    int numInputs = getTotalNumInputChannels();
    int numOutputs = getTotalNumOutputChannels();

    AudioBuffer<float>* tempBuffer = new AudioBuffer<float>(buffer);

    for(int channel = 0; channel < numOutputs; channel++) {
        for(int sample = 0; sample < buffer.getNumSamples(); sample++) {
            float amtToAdd = 0.0f;

            if (channel < numInputs)
                amtToAdd = tempBuffer->getSample(channel, sample);

            if (sample == 0)
                buffer.setSample(channel, sample, *startOfChannelParam + amtToAdd);
            else
                buffer.setSample(channel, sample, *restOfChannelParam + amtToAdd);
        }
    }
}


