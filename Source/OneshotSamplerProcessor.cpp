/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "OneshotSamplerProcessor.h"

using namespace synthvr;

OneshotSamplerProcessor::OneshotSamplerProcessor() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(4))
    .withOutput("Output", AudioChannelSet::discreteChannels(2)))
{
    // Parameters
    addParameter(bankParam = new AudioParameterFloat("bank", "Bank", 0.0f, 1.0f, 0.0f));
    addParameter(programParam = new AudioParameterFloat("program", "Program", 0.0f, 1.0f, 0.0f));
    addParameter(startParam = new AudioParameterFloat("start", "Start", 0.0f, 0.99f, 0.0f));
    addParameter(lengthParam = new AudioParameterFloat("length", "Length", 0.01f, 1.0f, 1.0f));
    addParameter(pitchParam = new AudioParameterFloat("pitch", "Pitch", 0.5f, 2.0f, 1.0f));
    addParameter(transientParam = new AudioParameterFloat("transient", "Transient", -1.0f, 1.0f, 0.0f));
    addParameter(triggerParam = new AudioParameterBool("trigger", "Trigger", false));

    // Initialize float data with room for 2 channels
    floatArrayData = std::vector<std::vector<float>>(2);
    floatArrayData[0] = std::vector<float>(4096);
    floatArrayData[1] = std::vector<float>(4096);
}

OneshotSamplerProcessor::~OneshotSamplerProcessor() {}

void OneshotSamplerProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    currentSampleRate = sampleRate;

    smoothedPitch.reset(currentSampleRate, 0.1f);
    smoothedPitch.setCurrentAndTargetValue(*pitchParam);
}

void OneshotSamplerProcessor::releaseResources() {}

void OneshotSamplerProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        handleTriggers(buffer, sample);
        handleStartPosition(buffer, sample);
        handleLength(buffer, sample);
        handlePitch(buffer, sample);
        handlePlayback(buffer, sample);

        previouslyTriggered = currentlyTriggered;
    }
}

void synthvr::OneshotSamplerProcessor::handleTriggers(juce::AudioSampleBuffer& buffer, int sample)
{
    currentlyTriggered = buffer.getSample(triggerInputChannel, sample) >= 0.5f || *triggerParam;
    if (currentlyTriggered && !previouslyTriggered)
    {
        currentPosition = *startParam;
        currentlyPlaying = true;
        currentIncrement = 1.0f / std::max(floatArrayData[0].size(), floatArrayData[1].size());
    }
}

void synthvr::OneshotSamplerProcessor::handleStartPosition(juce::AudioSampleBuffer& buffer, int sample)
{
    if (isInputConnected[startInputChannel])
    {
        currentStartPosition = ParameterUtils::calculateModulationLinear(
            *startParam,
            buffer.getSample(startInputChannel, sample),
            1.0f,
            0.0f,
            0.99f);
    }
    else
        currentStartPosition = *startParam;
}

void synthvr::OneshotSamplerProcessor::handleLength(juce::AudioSampleBuffer& buffer, int sample)
{
    if (isInputConnected[lengthInputChannel])
    {
        currentLength = ParameterUtils::calculateModulationLinear(
            *lengthParam,
            buffer.getSample(lengthInputChannel, sample),
            1.0f,
            0.01f,
            1.0f);
    }
    else
        currentLength = *lengthParam;
}

void synthvr::OneshotSamplerProcessor::handlePitch(juce::AudioSampleBuffer& buffer, int sample)
{
    if (isInputConnected[pitchInputChannel])
    {
        smoothedPitch.setTargetValue(
            ParameterUtils::calculateModulationFrequency(
                *pitchParam,
                buffer.getSample(pitchInputChannel, sample),
                1.0f,
                0.1f,
                4.0f));
    }
    else
        smoothedPitch.setTargetValue(*pitchParam);
    currentPitch = smoothedPitch.getNextValue();
}

void synthvr::OneshotSamplerProcessor::handlePlayback(juce::AudioSampleBuffer& buffer, int sample)
{
    if (currentlyPlaying)
    {
        currentPosition = getNextPosition(currentPosition, currentPitch, currentStartPosition, currentLength);
        buffer.setSample(outputChannelLeft, sample, getSampleAtPosition(floatArrayData[0], currentPosition));
        buffer.setSample(outputChannelRight, sample, getSampleAtPosition(floatArrayData[1], currentPosition));

        if (currentPosition >= 1.0f || currentStartPosition + currentLength)
            currentlyPlaying = false;
    }
}

float OneshotSamplerProcessor::getNextPosition(float currentPos, float pitch, float startPos, float length)
{
    return std::min(ParameterUtils::clamp(currentPos + currentIncrement * pitch, 0.0f, startPos+length), 1.0f);
}

float OneshotSamplerProcessor::getSampleAtPosition(const std::vector<float> &samples, float position)
{
    int highestIndex = samples.size() - 1;
    float scaledPosition = position * highestIndex;

    int positionA = (int)std::floor(scaledPosition);
    int positionB = (int)std::ceil(scaledPosition);

    float interpolationPosition = scaledPosition - (float)positionA;
    return ParameterUtils::lerp(samples[positionA], samples[positionB], interpolationPosition);
}