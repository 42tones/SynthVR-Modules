#include "UtilityFilter.h"

using namespace synthvr;

UtilityFilter::UtilityFilter() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(2))
    .withOutput("Output", AudioChannelSet::discreteChannels(2)))
{
    addParameter(colorParam = new AudioParameterFloat("frequency", "The frequency of the filter.", -1.0f, 1.0f, 0.0f));
    addParameter(resonanceParam = new AudioParameterFloat("resonance", "The resonance of the filter.", 0.0f, 0.99f, 0.0f));

    resonanceParam->range.skew = 1.5f;
}

UtilityFilter::~UtilityFilter() {}

void UtilityFilter::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    this->sampleRate = sampleRate;

    // Prepare parameter smoothers
    smoothedFrequencyParam.reset(sampleRate, paramSmoothingTime);
    smoothedResonanceParam.reset(sampleRate, paramSmoothingTime);

    smoothedFrequencyParam.setCurrentAndTargetValue(*colorParam);
    smoothedResonanceParam.setCurrentAndTargetValue(*resonanceParam);

    // Prepare DSP modules
    dsp::ProcessSpec processSpec{ sampleRate, static_cast<uint32> (maximumExpectedSamplesPerBlock), 1 };

    this->filterLeft.prepare(processSpec);
    this->filterLeft.reset();
    this->filterLeft.coefficients = calculateFilterCoefficientsFromColor(*colorParam);
    this->filterRight.prepare(processSpec);
    this->filterRight.reset();
    this->filterRight.coefficients = calculateFilterCoefficientsFromColor(*colorParam);

}

void UtilityFilter::releaseResources() {}

void UtilityFilter::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    smoothedFrequencyParam.setTargetValue(*colorParam);
    smoothedResonanceParam.setTargetValue(*resonanceParam);

    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        currentFrequency = smoothedFrequencyParam.getNextValue();
        currentResonance = smoothedFrequencyParam.getNextValue();
        currentCoefficients = calculateFilterCoefficientsFromColor(currentFrequency);

        if (isInputConnected[inputLeft])
        {
            this->filterLeft.coefficients = currentCoefficients;
            buffer.setSample(
                outputLeft, 
                sample, 
                this->filterLeft.processSample(buffer.getSample(inputLeft, sample)));
        }

        if (isInputConnected[inputRight])
        {
            this->filterRight.coefficients = currentCoefficients;
            buffer.setSample(
                outputRight,
                sample,
                this->filterRight.processSample(buffer.getSample(inputRight, sample)));
        }
    }
}

dsp::IIR::Coefficients<float>::Ptr synthvr::UtilityFilter::calculateFilterCoefficientsFromColor(float color)
{
    if (color >= 0.0f)
        return dsp::IIR::Coefficients<float>::makeHighPass(
            sampleRate, 
            std::pow(color, frequencySkew) * maxFrequency + minFrequency);
    else
        return dsp::IIR::Coefficients<float>::makeLowPass(
            sampleRate,
            std::pow(1.0f + color, frequencySkew) * maxFrequency + minFrequency);
}
