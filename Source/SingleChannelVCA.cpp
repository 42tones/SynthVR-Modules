/*
  ==============================================================================
    This is a module which takes audio input on channel 0, and CV input on channel 1.
    Intented use e.g. tremolo effect
  ==============================================================================
*/

#include "SingleChannelVCA.h"

using namespace synthvr;

SingleChannelVCA::SingleChannelVCA() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(2))
    .withOutput("Output", AudioChannelSet::discreteChannels(1)))
{
    addParameter(gainParam = new AudioParameterFloat("InputGain", "Input Gain", 0.0f, 1.0f, 1.0f));
    addParameter(gainCVAmountParam = new AudioParameterFloat("GainCVAmount", "GainCVAmount", 0.0f, 1.0f, 0.0f));
    
    gain.setRampDurationSeconds(0.1f);
    gain.setGainLinear(*gainParam);

    paramSmoothTime = 0.01f;

    normalizedGainCVModInput = 0.f;
}



SingleChannelVCA::~SingleChannelVCA() {}

void SingleChannelVCA::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    dsp::ProcessSpec processSpec{ sampleRate, static_cast<uint32> (maximumExpectedSamplesPerBlock) };
    this->gain.prepare(processSpec);

    smoothedGainCVAmountParam.reset(sampleRate, paramSmoothTime);
    smoothedGainCVAmountParam.setCurrentAndTargetValue(*gainCVAmountParam);
}

void SingleChannelVCA::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    if (isInputConnected[inputChannel]) 
    {

        gain.setGainLinear(*gainParam);
        

        if (!isInputConnected[gainCVModInput])
        {
            for (int sample = 0; sample < buffer.getNumSamples(); sample++) {

                buffer.setSample(
                    outputChannel, 
                    sample, 
                    gain.processSample(buffer.getSample(inputChannel, sample))
                );
            }
        }

        else 
        {
            smoothedGainCVAmountParam.setTargetValue(*gainCVAmountParam);

            for (int sample = 0; sample < buffer.getNumSamples(); sample++) {

                normalizedGainCVModInput = normalizeGainCVInput(buffer.getSample(gainCVModInput, sample));

                buffer.setSample(
                    outputChannel, 
                    sample, 
                    gain.processSample(buffer.getSample(inputChannel, sample)) + (buffer.getSample(inputChannel, sample) * normalizedGainCVModInput * smoothedGainCVAmountParam.getNextValue()));
            }
        }
    }
}

float SingleChannelVCA::normalizeGainCVInput(float sampleAmplitude)
{
    return  ParameterUtils::clamp((sampleAmplitude + 1.0f) * 0.5, 0.0f, 1.0f);
}
