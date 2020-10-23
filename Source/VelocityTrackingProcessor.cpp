/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "VelocityTrackingProcessor.h"

using namespace synthvr;

VelocityTrackingProcessor::VelocityTrackingProcessor() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(0))
    .withOutput("Output", AudioChannelSet::discreteChannels(6)))
{
    addParameter(smoothness = new AudioParameterFloat("smoothness", "The smoothness of the velocity tracking.", 0.05f, 1.0f, 0.1f));
    addParameter(strength = new AudioParameterFloat("strength", "The amplitue of the velocity.", 0.0f, 3.0f, 1.0f));
    addParameter(xActual = new AudioParameterFloat("xActual", "The actual x velocity (set by Unity).", -1.0f, 1.0f, 0.0f));
    addParameter(yActual = new AudioParameterFloat("yActual", "The actual y velocity (set by Unity).", -1.0f, 1.0f, 0.0f));
    addParameter(zActual = new AudioParameterFloat("zActual", "The actual z velocity (set by Unity).", -1.0f, 1.0f, 0.0f));

    currentSmoothness = *smoothness;
    currentSampleRate = 44100.0;
}

VelocityTrackingProcessor::~VelocityTrackingProcessor() {}

void VelocityTrackingProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    currentSmoothness = *smoothness;
    currentSampleRate = sampleRate;
    resetSmoothers();
}

void VelocityTrackingProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    if (fabs(currentSmoothness - *smoothness) > 0.01f)
    {
        currentSmoothness = *smoothness;
        resetSmoothers();
    }

    xSmooth.setTargetValue(*xActual);
    ySmooth.setTargetValue(*yActual);
    zSmooth.setTargetValue(*zActual);

    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        buffer.setSample(outX,          sample, *strength * ParameterUtils::clamp(xSmooth.getNextValue(), 0.0f, 1.0f));
        buffer.setSample(outXInverted,  sample, *strength * ParameterUtils::clamp(-xSmooth.getCurrentValue(), 0.0f, 1.0f));
        buffer.setSample(outY,          sample, *strength * ParameterUtils::clamp(ySmooth.getNextValue(), 0.0f, 1.0f));
        buffer.setSample(outYInverted,  sample, *strength * ParameterUtils::clamp(-ySmooth.getCurrentValue(), 0.0f, 1.0f));
        buffer.setSample(outZ,          sample, *strength * ParameterUtils::clamp(zSmooth.getNextValue(), 0.0f, 1.0f));
        buffer.setSample(outZInverted,  sample, *strength * ParameterUtils::clamp(-zSmooth.getCurrentValue(), 0.0f, 1.0f));
    }
}

void VelocityTrackingProcessor::resetSmoothers()
{
    xSmooth.reset(currentSampleRate, currentSmoothness);
    ySmooth.reset(currentSampleRate, currentSmoothness);
    zSmooth.reset(currentSampleRate, currentSmoothness);
}