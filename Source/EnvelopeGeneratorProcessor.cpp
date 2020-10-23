/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "EnvelopeGeneratorProcessor.h"

using namespace synthvr;

EnvelopeGeneratorProcessor::EnvelopeGeneratorProcessor() : BaseProcessor(BusesProperties()
        .withInput("Inputs", AudioChannelSet::discreteChannels(5))
        .withOutput ("Output", AudioChannelSet::discreteChannels(1)))
{
    // Parameters
    addParameter(attack = new AudioParameterFloat ("attack", "Attack time in seconds.", minValue, 1.0f, 0.3f));
    addParameter(decay = new AudioParameterFloat ("decay", "Decay time in seconds.", minValue, 1.0f, 0.3f));
    addParameter(sustain = new AudioParameterFloat ("sustain", "Sustain level.", 0.0f, 1.0f, 0.5f));
    addParameter(release = new AudioParameterFloat ("release", "Release time in seconds.", minValue, 1.0f, 0.3f));
    addParameter(speed = new AudioParameterFloat ("speed", "Max speed of envelope.", 1.0f, 8.0f, 4.0f));
    addParameter(outputDisplay = new AudioParameterFloat ("outputDisplay", "Displays envelope output.", 0.0f, 1.0f, 0.0f));

    attack->range.setSkewForCentre(defaultCenterValue);
    *attack = defaultCenterValue;
    decay->range.setSkewForCentre(defaultCenterValue);
    *decay = defaultCenterValue;
    release->range.setSkewForCentre(defaultCenterValue);
    *release = defaultCenterValue;
}

EnvelopeGeneratorProcessor::~EnvelopeGeneratorProcessor() {}

void EnvelopeGeneratorProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    envelope.setSampleRate(sampleRate);
}

void EnvelopeGeneratorProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &)
{
    for(int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        // Set envelope params
        envelopeParams.attack = ParameterUtils::clamp(*attack * *speed + buffer.getSample(1, sample) * maxCVInfluence, minValue, maxValue);
        envelopeParams.decay = ParameterUtils::clamp(*decay * *speed + buffer.getSample(2, sample) * maxCVInfluence, minValue, maxValue);
        envelopeParams.sustain = ParameterUtils::clamp(*sustain + buffer.getSample(3, sample), 0.0f, 1.0f);
        envelopeParams.release = ParameterUtils::clamp(*release * *speed + buffer.getSample(4, sample) * maxCVInfluence, minValue, maxValue);

        envelope.setParameters(envelopeParams);

        // Set envelope on/off state
        sampleIsTrigger = buffer.getSample(0, sample) >= 0.5f;
        if (sampleIsTrigger && !envelopeIsOn) // Trigger envelope Note On if its off and sample is trigger
        {
            envelopeIsOn = true;
            envelope.noteOn();
        }
        else if (!sampleIsTrigger && envelopeIsOn) // Trigger envelope Note Off if it is on and sample is not trigger
        {
            envelopeIsOn = false;
            envelope.noteOff();
        }

        buffer.setSample(0, sample, envelope.getNextSample());
        *outputDisplay = buffer.getSample(0, sample);
    }
}