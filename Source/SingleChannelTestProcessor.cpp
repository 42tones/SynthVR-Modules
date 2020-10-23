/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "SingleChannelTestProcessor.h"

using namespace synthvr;

SingleChannelTestProcessor::SingleChannelTestProcessor() : BaseProcessor(BusesProperties()
        .withInput("Inputs", AudioChannelSet::discreteChannels(1))
        .withOutput ("Output", AudioChannelSet::discreteChannels(1)))
{
    addParameter(valueParameter = new AudioParameterFloat ("SampleValue", "Sample Value", -1.0f, 1.0f, 0.0f));
    addParameter(intParameter = new AudioParameterInt ("IntParameter", "Int Parameter", -10, 10, 0));
    addParameter(boolParameter = new AudioParameterBool ("BoolParameter", "Bool Parameter", false));
    addParameter(choiceParameter = new AudioParameterChoice ("ChoiceParameter", "Choice Parameter", choices, 0));
}

SingleChannelTestProcessor::~SingleChannelTestProcessor() {}

void SingleChannelTestProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    this->expectedSampleRate = sampleRate;
    this->expectedBlockSize = maximumExpectedSamplesPerBlock;
    this->enableAllBuses();
}

void SingleChannelTestProcessor::releaseResources() {}

void SingleChannelTestProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &)
{
    // Do whatever work is needed for the processor.
    // The input AudioBuffer will contain channels matching the inputs and outputs of processor
    // If processor has 1 input and 5 outputs: AudioBuffer will have 6 channels where channel 0 is input.

    for(int channel = 0; channel < buffer.getNumChannels(); channel++) {
        for(int sample = 0; sample < buffer.getNumSamples(); sample++) {
            buffer.setSample(channel, sample, *valueParameter);
        }
    }
}


