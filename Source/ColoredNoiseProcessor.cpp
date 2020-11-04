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
    this->highShelfFilter.prepare(processSpec);
    this->highShelfFilter.reset();
    this->lowShelfFilter.prepare(processSpec);
    this->lowShelfFilter.reset();
    recalculateFilterCoefficients(*colorParam);

    this->lowCutFilter.prepare(processSpec);
    this->lowCutFilter.reset();
    this->lowCutFilter.coefficients = dsp::IIR::Coefficients<float>::makeHighPass(
        sampleRate,
        lowCutFrequency);

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
            lowCutFilter.processSample(randomGenerator.nextFloat() * currentLevel)
        );

        // Make colored noise
        currentColoredNoise = buffer.getSample(whiteNoiseOutputChannel, sample);
        currentColoredNoise = highShelfFilter.processSample(currentColoredNoise);
        currentColoredNoise = lowShelfFilter.processSample(currentColoredNoise);
        buffer.setSample(coloredNoiseOutputChannel, 
            sample, 
            currentColoredNoise);
    }
}

void synthvr::ColoredNoiseProcessor::recalculateFilterCoefficients(float colorValue)
{
    colorValue *= maxColorAmount;

    this->lowShelfFilter.coefficients = dsp::IIR::Coefficients<float>::makeLowShelf(
        sampleRate,
        defaultCenterFrequency,
        defaultColorQFactor,
        1.0f - colorValue);

    this->highShelfFilter.coefficients = dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate,
        defaultCenterFrequency,
        defaultColorQFactor,
        1.0f + colorValue);
}
