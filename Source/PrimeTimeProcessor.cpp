/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "PrimeTimeProcessor.h"

using namespace synthvr;

PrimeTimeProcessor::PrimeTimeProcessor() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(3))
    .withOutput("Output", AudioChannelSet::discreteChannels(8)))
{
    // Parameters
    addParameter(bpmParam = new AudioParameterFloat("bpm", "BPM", 10.0f, 200.0f, defaultBPM));
    addParameter(bpmModAmountParam = new AudioParameterFloat("bpmModAmount", "BPM Modulation Amount", -1.0f, 1.0f, 0.0f));

    bpmParam->range.setSkewForCentre(defaultBPM);
    *bpmParam = defaultBPM;

    clock.initialise([](float x)
    {
        return x < 0.0f ? 0.0f : 1.0f;
    });

    clock.setFrequency(calculateFrequency(0.0f));
}

PrimeTimeProcessor::~PrimeTimeProcessor() {}

void PrimeTimeProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
}

void PrimeTimeProcessor::releaseResources() {}

void PrimeTimeProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    for (int sample = 0; sample < buffer.getNumSamples(); sample++) 
    {
        // Allow external clock to override if its connected
        if (isInputConnected[extClockInputChannel])
            currentSample = buffer.getSample(extClockInputChannel, sample);
        else
        {
            clock.setFrequency(buffer.getSample(speedInputChannel, sample));
            currentSample = clock.processSample(0.0f);
        }

        currentlyResetting = buffer.getSample(resetInputChannel, sample) >= 0.5f;
        shouldReset = currentlyResetting && !previouslyResetting;
        previouslyResetting = currentlyResetting;

        for (int i = 0; i < dividers.size(); i++)
        {
            if (shouldReset)
                dividers[i].reset();

            // TODO: If the dividers are eating too much CPU, we can disable them based on input connection
            buffer.setSample(i, sample, dividers[i].processSample(currentSample));
        }
    }
}

float synthvr::PrimeTimeProcessor::calculateFrequency(float fmInput)
{
    return ParameterUtils::calculateModulationFrequency(*bpmParam / 60.0f * 8.0f, fmInput, *bpmModAmountParam);
}
