/*
  ==============================================================================
    Choose highpass or lowpass filter
    Stereo input
    Stereo output
    CV input: modulate cutoff frequency
    CV input: modulate resonance
  ==============================================================================
*/

#include "HighpassLowpassStereo.h"

using namespace synthvr;

HighpassLowpassStereo::HighpassLowpassStereo() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(4))
    .withOutput("Output", AudioChannelSet::discreteChannels(2)))
{
    addParameter(frequencyParam = new AudioParameterFloat("frequency", "The frequency of the filter.", minFrequency, maxFrequency, 3000.0f));
    addParameter(frequencyCVModParam = new AudioParameterFloat("frequencyCVModParam", "The amount to modulate the filter frequency.", -1.0f, 1.0f, 0.0f));
    addParameter(resonanceParam = new AudioParameterFloat("resonance", "The resonance of the filter.", minResonance, maxResonance, 0.0f));
    addParameter(resonanceCVModParam = new AudioParameterFloat("resonanceCVModParam", "The amount to modulate the resonance frequency.", 0.0f, 1.0f, 0.0f));
    addParameter(highpassLowpassToggleParam = new AudioParameterInt("mode", "Highpass or Lowpass Filter", 0, 1, 0));
    addParameter(filterRollOff12or24Param = new AudioParameterInt("rollOff", "Whether to use 12dB/oct filter rolloff or 24db/oct.", 0, 1, 0));

    frequencyParam->range.skew = frequencySkew;
    resonanceParam->range.skew = 1.5f;
}



HighpassLowpassStereo::~HighpassLowpassStereo() {}

void HighpassLowpassStereo::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    currentFilterMode = calculateFilterMode(*highpassLowpassToggleParam, *filterRollOff12or24Param);

    // Prepare parameter smoothers
    smoothedFrequencyParam.reset(sampleRate, paramSmoothingTime);
    smoothedResonanceParam.reset(sampleRate, paramSmoothingTime);

    smoothedFrequencyParam.setCurrentAndTargetValue(*frequencyParam);
    smoothedResonanceParam.setCurrentAndTargetValue(*resonanceParam);

    // Prepare DSP modules
    dsp::ProcessSpec processSpec{ sampleRate, static_cast<uint32> (maximumExpectedSamplesPerBlock), 1 };

    this->filterProcessorLeft.prepare(processSpec);
    this->filterProcessorLeft.setEnabled(true);
    this->filterProcessorLeft.setMode(static_cast<SingleSampleLadderFilterMode>(currentFilterMode));
    this->filterProcessorLeft.setCutoffFrequencyHz(smoothedFrequencyParam.getCurrentValue());
    this->filterProcessorLeft.setResonance(*resonanceParam);
    this->filterProcessorLeft.setDrive(driveAmount);
    
    this->filterProcessorRight.prepare(processSpec);
    this->filterProcessorRight.setEnabled(true);
    this->filterProcessorRight.setMode(static_cast<SingleSampleLadderFilterMode>(currentFilterMode));
    this->filterProcessorRight.setCutoffFrequencyHz(smoothedFrequencyParam.getCurrentValue());
    this->filterProcessorRight.setResonance(*resonanceParam);
    this->filterProcessorRight.setDrive(driveAmount);
}

void HighpassLowpassStereo::releaseResources() {}

void HighpassLowpassStereo::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    calculatedFilterMode = calculateFilterMode(*highpassLowpassToggleParam, *filterRollOff12or24Param);

    // Check if the filter mode has changed
    if (calculatedFilterMode != currentFilterMode)
    {
        currentFilterMode = calculatedFilterMode;
        this->filterProcessorLeft.setMode(static_cast<SingleSampleLadderFilterMode>(currentFilterMode));
        this->filterProcessorRight.setMode(static_cast<SingleSampleLadderFilterMode>(currentFilterMode));
    }

    // Update smoothers
    smoothedFrequencyParam.setTargetValue(*frequencyParam);
    smoothedResonanceParam.setTargetValue(*resonanceParam);
    smoothedFrequencyCVModParam.setTargetValue(*frequencyCVModParam);
    smoothedResonanceCVModParam.setTargetValue(*resonanceCVModParam);

    // Process Samples

    if (isInputConnected[static_cast<int>(InOut::inputLeft)])
    {
        this->filterProcessorLeft.setEnabled(true);
        ProcessSingleSampleLadderFilter(buffer, &this->filterProcessorLeft, InOut::inputLeft, InOut::outputLeft);
    }

    if (!isInputConnected[static_cast<int>(InOut::inputLeft)]) 
        this->filterProcessorLeft.setEnabled(false);
    
    if (isInputConnected[static_cast<int>(InOut::inputRight)])
    {
        this->filterProcessorRight.setEnabled(true);
        ProcessSingleSampleLadderFilter(buffer, &this->filterProcessorRight, InOut::inputLeft, InOut::outputRight);
    }

    if (!isInputConnected[static_cast<int>(InOut::inputRight)]) 
        this->filterProcessorRight.setEnabled(false);
}

void HighpassLowpassStereo::ProcessSingleSampleLadderFilter(AudioBuffer<float>& buffer, SingleSampleLadderFilter<float>* thisFilterProcessor, InOut whichInput, InOut whichOutput)
{
    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        // Calculate frequency modulator
        if (isInputConnected[static_cast<int>(InOut::frequencyCVModInput)])
            currentFrequency = ParameterUtils::calculateModulationFrequency(
                smoothedFrequencyParam.getNextValue(),
                buffer.getSample(static_cast<int>(InOut::frequencyCVModInput), sample),
                *frequencyCVModParam,
                minFrequency,
                maxFrequency);
        else
            currentFrequency = smoothedFrequencyParam.getNextValue();

        
        // Calculate Resonance modulator
        if (isInputConnected[static_cast<int>(InOut::resonanceCVModInput)])
            currentResonance = ParameterUtils::calculateModulationMultiply(
                smoothedResonanceParam.getNextValue(),
                buffer.getSample(static_cast<int>(InOut::resonanceCVModInput), sample),
                *resonanceCVModParam,
                0.0f,
                0.99f);
        else
                currentResonance = smoothedResonanceParam.getNextValue();

        thisFilterProcessor->setCutoffFrequencyHz(currentFrequency);
        thisFilterProcessor->setResonance(currentResonance);
        thisFilterProcessor->setDrive(driveAmount);

        currentFilterOutput = thisFilterProcessor->processSample(buffer.getSample(static_cast<int>(whichInput), sample), 0);
       
        buffer.setSample(
            static_cast<int>(whichOutput),
            sample,
            currentFilterOutput
        );
    }
}

int HighpassLowpassStereo::calculateFilterMode(int highpassLowpassToggle, int rollOff12or24)
{
    // There are 3 12dB/oct modes (0,1,2) and 3 24db/oct modes (3,4,5)
    // 2 and 5, which are bandpass modes are never used in this implementation
    return highpassLowpassToggle + (rollOff12or24 * 3);
}