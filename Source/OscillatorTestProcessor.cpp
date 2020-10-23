/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "OscillatorTestProcessor.h"

using namespace synthvr;

OscillatorTestProcessor::OscillatorTestProcessor() : BaseProcessor(BusesProperties()
        .withInput("Inputs", AudioChannelSet::discreteChannels(1))
        .withOutput ("Output", AudioChannelSet::discreteChannels(1)))
{
    // Parameters
    addParameter(freqParam = new AudioParameterFloat ("Frequency", "Frequency of oscillator", minFrequency, maxFrequency, defaultCenterFrequency));
    addParameter(gainParam = new AudioParameterFloat ("Gain", "Gain of oscillator", 0.0f, 1.0f, 0.5f));
    addParameter(fmParam = new AudioParameterFloat ("fmAmount", "Amount of FM to add from input", 0.0f, 1.0f, 0.5f));

    freqParam->range.setSkewForCentre(defaultCenterFrequency);
    *freqParam = defaultCenterFrequency;

    // DSP
    osc.initialise ([] (float x) { return std::sin (x); }, 64);
    osc.setFrequency(calculateFrequency(*freqParam, 0.0f));
    gain.setRampDurationSeconds(0.1f);
    gain.setGainLinear(*gainParam);
}

OscillatorTestProcessor::~OscillatorTestProcessor() {}

void OscillatorTestProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    // Prepare DSP modules
    dsp::ProcessSpec processSpec { sampleRate, static_cast<uint32> (maximumExpectedSamplesPerBlock) };
    this->osc.prepare(processSpec);
    this->gain.prepare(processSpec);

    smoothedFreqParam.reset (sampleRate, freqParamSmoothTime);
    smoothedFreqParam.setCurrentAndTargetValue(*freqParam);
}

void OscillatorTestProcessor::releaseResources() {}

void OscillatorTestProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &)
{
    // Update gain per sample block since it is not CV controlled anyway
    gain.setGainLinear(*gainParam);

    smoothedFreqParam.setTargetValue(*freqParam);
    
    // Just work on channel 0
    for(int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        // Update oscillator frequency per sample using frequency param +
        freqToSet = calculateFrequency(smoothedFreqParam.getNextValue(),
                                       buffer.getSample(0, sample) * *fmParam);
        osc.setFrequency(freqToSet, true);
        
        // Process a single sample with osc overwriting buffer input
        buffer.setSample(0, sample,
                         osc.processSample(0.0f));
        
        // Process single sample with gain
        buffer.setSample(0, sample,
                         gain.processSample(buffer.getSample(0, sample)));
    }
}

float OscillatorTestProcessor::calculateFrequency(float param, float fmInput)
{
    return ParameterUtils::clamp((param * std::pow(2, 5.0f * fmInput)), minFrequency, 20000.0f);
}
