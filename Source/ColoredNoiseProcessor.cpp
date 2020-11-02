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
    addParameter(colorParam = new AudioParameterFloat("color", "Color", -1.0f, 1.0f, -0.6f));
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

    smoothedLevel.reset(sampleRate, defaultParameterSmoothing);
    smoothedLevel.setCurrentAndTargetValue(*levelParam);
}

void ColoredNoiseProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    smoothedColor.setTargetValue(*colorParam);
    smoothedLevel.setTargetValue(*levelParam);

    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        // Calculate color
        currentColor = smoothedColor.getNextValue();
        if (isInputConnected[colorInputChannel])
            currentColor = ParameterUtils::calculateModulationLinear(
                currentColor,
                1.0f,
                buffer.getSample(colorInputChannel, sample),
                -1.0f);

        recalculateFilterCoefficients(currentColor);

        // Calculate level
        currentLevel = smoothedLevel.getNextValue();
        if (isInputConnected[levelInputChannel])
            currentLevel = ParameterUtils::calculateModulationLinear(
                currentLevel,
                1.0f,
                buffer.getSample(levelInputChannel, sample));

        // Make white noise
        buffer.setSample(
            whiteNoiseOutputChannel, 
            sample,
            randomGenerator.nextFloat() * currentLevel
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
    colorValue *= maxColorAmount;

    this->lowFilter.coefficients = dsp::IIR::Coefficients<float>::makeLowShelf(
        sampleRate,
        defaultCenterFrequency,
        defaultColorQFactor,
        1.0f - colorValue);

    this->highFilter.coefficients = dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate,
        defaultCenterFrequency,
        defaultColorQFactor,
        1.0f + colorValue);
}
