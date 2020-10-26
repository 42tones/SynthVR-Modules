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
    addParameter(timeParam = new AudioParameterFloat("time", "Time (ms)", 0.0f, 4000.0f, 500.0f));
    addParameter(timeModulationAmountParam = new AudioParameterFloat("timeModulationAmount", "Time Modulation Amount", -1.0f, 1.0f, 0.0f));
    addParameter(feedbackParam = new AudioParameterFloat("feedback", "Feedback", 0.0f, 1.3f, 0.5f));
    addParameter(feedbackModulationAmountParam = new AudioParameterFloat("feedbackModulationAmount", "Feedback Modulation Amount", -1.0f, 1.0f, 0.0f));
    addParameter(mixParam = new AudioParameterFloat("mix", "Mix", 0.0f, 1.0f, 0.35f));
    addParameter(mixModulationAmountParam = new AudioParameterFloat("mixModulationAmount", "Mix Modulation Amount", -1.0f, 1.0f, 0.0f));
    addParameter(colorParam = new AudioParameterFloat("color", "Color", -1.0f, 1.0f, -0.2f));

    timeParam->range.setSkewForCentre(defaultSpeedCenterMs);
}

DelayProcessor::~DelayProcessor() {}

void DelayProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    this->sampleRate = sampleRate;

    dsp::ProcessSpec processSpec{ sampleRate, static_cast<uint32> (maximumExpectedSamplesPerBlock) };

    this->delay.prepare(processSpec);
    this->delay.reset();
    this->delay.setDelay(*timeParam);

    this->filter.prepare(processSpec);
    this->filter.reset();
    this->filterCoefficients = calculateFilterCoefficientsFromColor(*colorParam);
    this->filter.coefficients = filterCoefficients;
}

void DelayProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    // TODO: This can be optimized by checking for changes in color param
    this->filterCoefficients = calculateFilterCoefficientsFromColor(*colorParam);
    this->filter.coefficients = filterCoefficients;

    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
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

