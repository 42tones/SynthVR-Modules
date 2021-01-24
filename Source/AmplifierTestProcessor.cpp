/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "AmplifierTestProcessor.h"

using namespace synthvr;

AmplifierTestProcessor::AmplifierTestProcessor() : BaseProcessor(BusesProperties()
        .withInput("Inputs", AudioChannelSet::discreteChannels(2))
        .withOutput ("Output", AudioChannelSet::discreteChannels(3)))
{
    // Parameters
    addParameter(gainAmount = new AudioParameterFloat ("gainAmount", "The amount of gain to use.", 0.0f, 2.0f, 1.0f));
    addParameter(cvAmount = new AudioParameterFloat ("cvAmount", "The amount of CV to apply to gain.", 0.0f, 1.0f, 0.5f));

    // DSP
    gainProcessor.setGainLinear(calculateGain(0.0f));
    gainProcessor.setRampDurationSeconds(0.001f);
}

AmplifierTestProcessor::~AmplifierTestProcessor() {}

void AmplifierTestProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    // Prepare DSP modules
    dsp::ProcessSpec processSpec { sampleRate, static_cast<uint32> (maximumExpectedSamplesPerBlock) };
    this->gainProcessor.prepare(processSpec);
}

void AmplifierTestProcessor::releaseResources() {}

void AmplifierTestProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &)
{
    // Iterate through once, since we are just working with one true input channel
    for(int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        // Calculate and set gain amt
        gainProcessor.setGainLinear(calculateGain(buffer.getSample(cvChannel, sample)));

        // Process a single sample with osc overwriting buffer input
        buffer.setSample(inOutChannel, sample,
                gainProcessor.processSample(buffer.getSample(inOutChannel, sample)));

        // Fill the second channel with amp output
        buffer.setSample(cvChannel, sample, buffer.getSample(inOutChannel, sample));

        // Fill the third channel with amp output for display (a channel whose input is not written to by other processors)
        buffer.setSample(displayChannel, sample, buffer.getSample(inOutChannel, sample));
    }
}

float AmplifierTestProcessor::calculateGain(float cvInput)
{
    return *gainAmount + (cvInput * *cvAmount);
}
