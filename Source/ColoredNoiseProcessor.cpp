/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "ColoredNoiseProcessor.h"

using namespace synthvr;

ColoredNoiseProcessor::ColoredNoiseProcessor() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(2))
    .withOutput("Output", AudioChannelSet::discreteChannels(2)))
{
    addParameter(colorParam = new AudioParameterFloat("color", "Color", -1.0f, 1.0f, -0.2f));
    addParameter(levelParam = new AudioParameterFloat("level", "Level", 0.0f, 1.0f, 0.6f));
}

ColoredNoiseProcessor::~ColoredNoiseProcessor() {}

void ColoredNoiseProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    this->sampleRate = sampleRate;

    dsp::ProcessSpec processSpec{ sampleRate, static_cast<uint32> (maximumExpectedSamplesPerBlock), 1 };
    this->highFilter.prepare(processSpec);
    this->highFilter.reset();
    this->lowFilter.prepare(processSpec);
    this->lowFilter.reset();
    recalculateFilterCoefficients(*colorParam);

    smoothedLevel.reset(sampleRate, defaultLevelSmoothing);
    smoothedLevel.setCurrentAndTargetValue(*levelParam);
}

void ColoredNoiseProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    // Calculate color once per block if input not connected
    if (!isInputConnected[colorInputChannel])
        recalculateFilterCoefficients(*colorParam);

    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        // Calculate color coefficients
        if (isInputConnected[colorInputChannel])
            recalculateFilterCoefficients(*colorParam + buffer.getSample(colorInputChannel, sample));

        // Calculate target level
        if (isInputConnected[levelInputChannel])
            currentTargetLevel = ParameterUtils::calculateModulationLinear(
                *levelParam,
                1.0f,
                buffer.getSample(levelInputChannel, sample));
        else
            currentTargetLevel = *levelParam;

        smoothedLevel.setTargetValue(currentTargetLevel);

        // Make white noise
        buffer.setSample(
            whiteNoiseOutputChannel, 
            sample,
            randomGenerator.nextFloat() * smoothedLevel.getNextValue()
        );

        // Make colored noise
        currentColoredNoise = buffer.getSample(whiteNoiseOutputChannel, sample);
        currentColoredNoise = highFilter.processSample(currentColoredNoise);
        currentColoredNoise = lowFilter.processSample(currentColoredNoise);
        buffer.setSample(coloredNoiseOutputChannel, 
            sample, 
            currentColoredNoise);
    }
}

void synthvr::ColoredNoiseProcessor::recalculateFilterCoefficients(float colorValue)
{
    colorValue = ParameterUtils::clamp(colorValue, 0.0f, 1.0f);

    this->lowFilter.coefficients = dsp::IIR::Coefficients<float>::makeLowShelf(
        sampleRate,
        defaultCenterFrequency,
        defaultColorQFactor,
        1.0f - colorValue * defaultColorFactor);

    this->highFilter.coefficients = dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate,
        defaultCenterFrequency,
        defaultColorQFactor,
        1.0f + colorValue * defaultColorFactor);
}
