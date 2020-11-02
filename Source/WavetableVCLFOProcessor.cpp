/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "WavetableVCLFOProcessor.h"

using namespace synthvr;

WavetableVCLFOProcessor::WavetableVCLFOProcessor() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(2))
    .withOutput("Output", AudioChannelSet::discreteChannels(1)))
{
    // Parameters
    addParameter(frequencyParam = new AudioParameterFloat("frequency", "Frequency", 0.015f, 8.0f, 1.0f));
    addParameter(fmAmountParam = new AudioParameterFloat("fmAmount", "FMAmount", -1.0f, 1.0f, 0.2f));
    addParameter(oscOutputDisplay = new AudioParameterFloat("oscOutputDisplay", "Output", 0.0f, 1.0f, 0.0f));

    frequencyParam->range.setSkewForCentre(1.0f);

    // Initialize float data with room for 4 wave arrays
    floatArrayData = std::vector<std::vector<float>>(1);
    floatArrayData[0] = std::vector<float>(defaultWaveTableLength);
}

WavetableVCLFOProcessor::~WavetableVCLFOProcessor() {}

void WavetableVCLFOProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    computeWaveTable();

    currentSampleRate = sampleRate;
    smoothedFrequency.reset(currentSampleRate, 0.1f);
    smoothedFrequency.setCurrentAndTargetValue(*frequencyParam);
}

void WavetableVCLFOProcessor::releaseResources() {}

void WavetableVCLFOProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    // Update oscillator waves
    if (waveTableShouldBeRecomputed)
        computeWaveTable();

    smoothedFrequency.setTargetValue(*frequencyParam);

    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        // Compute frequency
        currentFrequency = smoothedFrequency.getNextValue();
        if (isInputConnected[fmInputChannel])
            currentFrequency = ParameterUtils::calculateModulationFrequency(
                currentFrequency, 
                buffer.getSample(fmInputChannel, sample), 
                *fmAmountParam, 
                0.0001f, 
                100.0f);

        // Do phase reset
        if (isInputConnected[resetInputChannel])
        {
            currentResetValue = buffer.getSample(resetInputChannel, sample);
            if (currentResetValue >= 0.5f && previousResetValue <= 0.5f)
                osc->resetPhase();
            previousResetValue = currentResetValue;
        }

        osc->setFrequency(currentFrequency, currentSampleRate);
        buffer.setSample(outputChannel, sample, osc->process());
    }

    *oscOutputDisplay = buffer.getRMSLevel(outputChannel, 0, buffer.getNumSamples());
}


void WavetableVCLFOProcessor::computeWaveTable()
{
    // TODO: Can oscillator be optimized so it no loger needs to be re-instantiated?
    osc = std::make_unique<WaveTableOsc>();
    setOscWaveTable(osc.get(), floatArrayData[0].data(), floatArrayData[0].size());
    waveTableShouldBeRecomputed = false;
}
