/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "DelayProcessor.h"

using namespace synthvr;

DelayProcessor::DelayProcessor() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(6))
    .withOutput("Output", AudioChannelSet::discreteChannels(2)))
{
    addParameter(timeParam = new AudioParameterFloat("time", "Time (s)", 0.0f, 4.0f, defaultSpeedCenterSeconds));
    addParameter(timeModulationAmountParam = new AudioParameterFloat("timeModulationAmount", "Time Modulation Amount", -1.0f, 1.0f, 0.0f));
    addParameter(feedbackParam = new AudioParameterFloat("feedback", "Feedback", 0.0f, 1.1f, 0.5f));
    addParameter(feedbackModulationAmountParam = new AudioParameterFloat("feedbackModulationAmount", "Feedback Modulation Amount", -1.0f, 1.0f, 0.0f));
    addParameter(mixParam = new AudioParameterFloat("mix", "Mix", 0.0f, 1.0f, 0.35f));
    addParameter(mixModulationAmountParam = new AudioParameterFloat("mixModulationAmount", "Mix Modulation Amount", -1.0f, 1.0f, 0.0f));
    addParameter(colorParam = new AudioParameterFloat("color", "Color", -1.0f, 1.0f, -0.2f));

    timeParam->range.setSkewForCentre(defaultSpeedCenterSeconds);
}

DelayProcessor::~DelayProcessor() {}

void DelayProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    this->sampleRate = sampleRate;

    dsp::ProcessSpec processSpec{ sampleRate, static_cast<uint32> (maximumExpectedSamplesPerBlock), 1 };
    this->delay.prepare(processSpec);
    this->delay.reset();
    this->delay.setDelay(*timeParam);

    this->filter.prepare(processSpec);
    this->filter.reset();
    this->filter.coefficients = calculateFilterCoefficientsFromColor(*colorParam);
}

void DelayProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    // TODO: This can be optimized by checking for changes in color param
    this->filter.coefficients = calculateFilterCoefficientsFromColor(*colorParam);

    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        // Update modulation
        if (isInputConnected[timeChannel])
            currentDelayInSamples = ParameterUtils::calculateModulationMultiply(
                *timeParam,
                buffer.getSample(timeChannel, sample),
                *timeModulationAmountParam,
                0.001f,
                maxDelaySpeedSeconds) * sampleRate;
        else
            currentDelayInSamples = *timeParam;

        if (isInputConnected[feedbackChannel])
            currentFeedback = ParameterUtils::calculateModulationMultiply(
                *feedbackParam,
                buffer.getSample(feedbackChannel, sample),
                *feedbackModulationAmountParam,
                0.001f,
                maxFeedback);
        else
            currentFeedback = *feedbackParam;

        if (isInputConnected[mixChannel])
            currentMix = ParameterUtils::calculateModulationMultiply(
                *mixParam,
                buffer.getSample(mixChannel, sample),
                *mixModulationAmountParam);
        else
            currentMix = *mixParam;

        // Pop a sample off the delay buffer
        currentDelayOutput = delay.popSample(0, currentDelayInSamples);

        // Process delay input
        currentDelayInput = buffer.getSample(inputChannel, sample);
        if (isOutputConnected[sendChannel])
        {
            buffer.setSample(sendChannel, sample, currentDelayInput);
            currentDelayInput = buffer.getSample(returnChannel, sample);
        }

        currentDelayInput += currentDelayOutput * currentFeedback;
        currentDelayInput = filter.processSample(currentDelayInput);
        delay.pushSample(0, currentDelayInput);
    }
}

dsp::IIR::Coefficients<float>::Ptr synthvr::DelayProcessor::calculateFilterCoefficientsFromColor(float colorValue)
{
    return dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate,
        defaultHighShelfFrequency,
        defaultHighShelfQFactor,
        1.0f + colorValue * defaultHighShelfColorFactor);
}
