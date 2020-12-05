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
    addParameter(startParam = new AudioParameterFloat("start", "Start", 0.0f, 0.9f, 0.0f));
    addParameter(lengthParam = new AudioParameterFloat("length", "Length", 0.1f, 1.0f, 1.0f));
    addParameter(pitchParam = new AudioParameterFloat("pitch", "Pitch", 0.25f, 4.0f, 1.0f));
    addParameter(transientParam = new AudioParameterFloat("transient", "Transient", -1.0f, 1.0f, 0.0f));
    addParameter(volumeParam = new AudioParameterFloat("volume", "Volume", 0.0f, 1.0f, 0.75f));
    addParameter(triggerParam = new AudioParameterBool("trigger", "Trigger", false));

    pitchParam->range.setSkewForCentre(1.0f);

    // Initialize float data with room for 2 channels
    floatArrayData = std::vector<std::vector<float>>(2);
    floatArrayData[0] = std::vector<float>(4096);
    floatArrayData[1] = std::vector<float>(4096);
}

OneshotSamplerProcessor::~OneshotSamplerProcessor() {}

void OneshotSamplerProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentStartPosition = *startParam;
    currentLength = *lengthParam;

    smoothedPitch.reset(currentSampleRate, 0.05f);
    smoothedPitch.setCurrentAndTargetValue(*pitchParam);

    smoothedVolume.reset(currentSampleRate, 0.1f);
    smoothedVolume.setCurrentAndTargetValue(*volumeParam);

    transientShaper.reset(currentSampleRate, defaultTransientSpeed);
    transientShaper.setCurrentAndTargetValue(1.0f);
}

void OneshotSamplerProcessor::releaseResources() {}

void OneshotSamplerProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    smoothedVolume.setTargetValue(*volumeParam);

    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        handleStartPosition(buffer, sample);
        handleLength(buffer, sample);
        handlePitch(buffer, sample);

        handleTriggers(buffer, sample);
        handlePlayback(buffer, sample);

        previouslyTriggered = currentlyTriggered;
    }
}

void synthvr::OneshotSamplerProcessor::handleTriggers(juce::AudioSampleBuffer& buffer, int sample)
{
    currentlyTriggered = buffer.getSample(triggerInputChannel, sample) >= 0.5f || *triggerParam;
    if (currentlyTriggered && !previouslyTriggered)
    {
        currentPosition = currentStartPosition;
        currentlyPlaying = true;
        currentIncrement = 1.0f / std::max(floatArrayData[0].size(), floatArrayData[1].size());

        transientShaper.setCurrentAndTargetValue(*transientParam + 1.0f);
        transientShaper.setTargetValue(1.0f);
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
            0.9f);
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
            0.1f,
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
                0.4f,
                0.1f,
                4.0f));
    }
    else
        smoothedPitch.setTargetValue(*pitchParam);
    currentPitch = smoothedPitch.getNextValue();
}

void synthvr::OneshotSamplerProcessor::handlePlayback(juce::AudioSampleBuffer& buffer, int sample)
{
    currentVolume = ParameterUtils::clamp(
        smoothedVolume.getNextValue() * transientShaper.getNextValue(), 
        0.0f, 
        2.0f);

    if (currentlyPlaying)
    {
        currentPosition = getNextPosition(currentPosition, currentPitch, currentStartPosition, currentLength);
        
        // TODO: Lower volume when close to length to prevent clicking
        // TODO: Lower volume when close to start to prevent clicking

        buffer.setSample(
            outputChannelLeft, 
            sample, 
            getSampleAtPosition(floatArrayData[0], currentPosition) * currentVolume);
        
        buffer.setSample(
            outputChannelRight, 
            sample, 
            getSampleAtPosition(floatArrayData[1], currentPosition) * currentVolume);

        if (currentPosition >= 1.0f || currentPosition >= (currentStartPosition + currentLength))
            currentlyPlaying = false;
    }
    else
    {
        buffer.setSample(outputChannelLeft, sample, 0.0f);
        buffer.setSample(outputChannelRight, sample, 0.0f);
    }
}

float OneshotSamplerProcessor::getNextPosition(float currentPos, float pitch, float startPos, float length)
{
    return std::min(ParameterUtils::clamp(currentPos + currentIncrement * pitch, 0.0f, startPos+length), 1.0f);
}

float OneshotSamplerProcessor::getSampleAtPosition(const std::vector<float> &samples, float position)
{
    float scaledPosition = position * (samples.size() - 1);

    int positionA = (int)std::floor(scaledPosition);
    int positionB = (int)std::ceil(scaledPosition);

    float interpolationPosition = scaledPosition - (float)positionA;
    return ParameterUtils::lerp(samples[positionA], samples[positionB], interpolationPosition);
}