/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "MasterClockProcessor.h"

using namespace synthvr;

MasterClockProcessor::MasterClockProcessor() : BaseProcessor(BusesProperties()
        .withInput("Input", AudioChannelSet::discreteChannels(1))
        .withOutput ("Output", AudioChannelSet::discreteChannels(1)))
{
    addParameter(clockFrequency = new AudioParameterFloat ("clockFrequency", "The frequency of the clock.", minClockFrequency, maxClockFrequency, frequencyParameterCenterValue));
    addParameter(frequencyModulation = new AudioParameterFloat ("frequencyModulation", "The amount of frequency modulation to apply from input signal.", 0.0f, 1.0f, 0.5f));

    clockFrequency->range.setSkewForCentre(frequencyParameterCenterValue);
    *clockFrequency = frequencyParameterCenterValue;

    clock.initialise ([] (float x)
    { 
        return x < 0.0f ? 0.0f : 1.0f;
    });

    clock.setFrequency(calculateFrequency(*clockFrequency, 0.0f));
}

MasterClockProcessor::~MasterClockProcessor() {}

void MasterClockProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &)
{
    // Single channel process
    for(int sample = 0; sample < buffer.getNumSamples(); sample++) {
        // Update oscillator frequency per sample using frequency param +
        freqToSet = calculateFrequency(*clockFrequency,
                buffer.getSample(0, sample) * *frequencyModulation);
        clock.setFrequency(freqToSet);

        // Process a single sample with osc overwriting buffer input
        buffer.setSample(0, sample,
                clock.processSample(0.0f));
    }
}

float MasterClockProcessor::calculateFrequency(float param, float fmInput) {
    return std::max(param + (fmInput * maxClockFrequency / 2.0f), minClockFrequency);
}
