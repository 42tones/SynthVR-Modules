/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "SignalSplitterProcessor.h"

using namespace synthvr;

SignalSplitterProcessor::SignalSplitterProcessor() : BaseProcessor(BusesProperties()
        .withInput("Inputs", AudioChannelSet::discreteChannels(1))
        .withOutput ("Output", AudioChannelSet::discreteChannels(4)))
{
}

SignalSplitterProcessor::~SignalSplitterProcessor() {}

void SignalSplitterProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &)
{
    // Copy first input (channel 0) to outputs
    buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());
    buffer.copyFrom(2, 0, buffer, 0, 0, buffer.getNumSamples());
    buffer.copyFrom(3, 0, buffer, 0, 0, buffer.getNumSamples());
}

