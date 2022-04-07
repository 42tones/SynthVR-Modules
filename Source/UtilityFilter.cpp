#include "UtilityFilter.h"

using namespace synthvr;

UtilityFilter::UtilityFilter() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(2))
    .withOutput("Output", AudioChannelSet::discreteChannels(2)))
{
    addParameter(colorParam = new AudioParameterFloat("frequency", "The frequency of the filter.", -1.0f, 1.0f, 0.0f));
    addParameter(resonanceParam = new AudioParameterFloat("resonance", "The resonance of the filter.", 0.2f, 2.0f, 0.0f));

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

    this->lowpassLeft.prepare(processSpec);
    this->lowpassLeft.reset();
    this->lowpassLeft.coefficients = calculateLowpassCoefficients(*colorParam, *resonanceParam);
    this->lowpassRight.prepare(processSpec);
    this->lowpassRight.reset();
    this->lowpassRight.coefficients = calculateLowpassCoefficients(*colorParam, *resonanceParam);

    this->highpassLeft.prepare(processSpec);
    this->highpassLeft.reset();
    this->highpassLeft.coefficients = calculateHighpassCoefficients(*colorParam, *resonanceParam);
    this->highpassRight.prepare(processSpec);
    this->highpassRight.reset();
    this->highpassRight.coefficients = calculateHighpassCoefficients(*colorParam, *resonanceParam);

}

void UtilityFilter::releaseResources() {}

void UtilityFilter::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    smoothedFrequencyParam.setTargetValue(*colorParam);
    smoothedResonanceParam.setTargetValue(*resonanceParam);

    this->lowpassLeft.snapToZero();
    this->highpassLeft.snapToZero();
    this->lowpassRight.snapToZero();
    this->highpassRight.snapToZero();

    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        currentColor = smoothedFrequencyParam.getNextValue();
        currentResonance = smoothedResonanceParam.getNextValue();
        lowpassCoeffecients = calculateLowpassCoefficients(currentColor, currentResonance);
        highpassCoeffecients = calculateHighpassCoefficients(currentColor, currentResonance);

        if (isInputConnected[inputLeft])
        {
            this->lowpassLeft.coefficients = lowpassCoeffecients;
            this->highpassLeft.coefficients = highpassCoeffecients;

            buffer.setSample(
                outputLeft, 
                sample, 
                this->highpassLeft.processSample(
                    this->lowpassLeft.processSample(
                        buffer.getSample(inputLeft, sample))));
        }

        if (isInputConnected[inputRight])
        {
            this->lowpassRight.coefficients = lowpassCoeffecients;
            this->highpassRight.coefficients = highpassCoeffecients;

            buffer.setSample(
                outputRight,
                sample,
                this->highpassRight.processSample(
                    this->lowpassRight.processSample(
                        buffer.getSample(inputRight, sample))));
        }
    }
}

dsp::IIR::Coefficients<float>::Ptr synthvr::UtilityFilter::calculateLowpassCoefficients(float color, float resonance)
{
    return dsp::IIR::Coefficients<float>::makeLowPass(
        sampleRate,
        std::pow(std::clamp(1.0f + color, 0.0f, 1.0f), frequencySkew) * maxFrequency + minFrequency,
        resonance);
}

dsp::IIR::Coefficients<float>::Ptr synthvr::UtilityFilter::calculateHighpassCoefficients(float color, float resonance)
{
    return dsp::IIR::Coefficients<float>::makeHighPass(
        sampleRate,
        std::pow(std::clamp(color, 0.0f, 1.0f), frequencySkew) * maxFrequency + minFrequency,
        resonance);
}
