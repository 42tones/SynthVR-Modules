/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "DualVCAProcessor.h"

using namespace synthvr;

DualVCAProcessor::DualVCAProcessor() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(6))
    .withOutput("Output", AudioChannelSet::discreteChannels(2)))
{
    // Parameters
    addParameter(gainAmountA = new AudioParameterFloat("gainA", "Gain A", 0.0f, 2.0f, 1.0f));
    addParameter(cvAmountA = new AudioParameterFloat("cvA", "CV A", 0.0f, 2.0f, 0.5f));
    addParameter(gainAmountB = new AudioParameterFloat("gainB", "Gain A", 0.0f, 2.0f, 1.0f));
    addParameter(cvAmountB = new AudioParameterFloat("cvB", "CV A", 0.0f, 2.0f, 0.5f));
    
    addParameter(cvADisplay = new AudioParameterFloat("cvADisplay", "CV A Display", 0.0f, 1.0f, 0.0f));
    addParameter(outADisplay = new AudioParameterFloat("outADisplay", "Out A Display", 0.0f, 1.0f, 0.0f));
    addParameter(cvBDisplay = new AudioParameterFloat("cvBDisplay", "CV B Display", 0.0f, 1.0f, 0.0f));
    addParameter(outBDisplay = new AudioParameterFloat("outBDisplay", "Out B Display", 0.0f, 1.0f, 0.0f));

    // DSP
    gainProcessorA.setGainLinear(0.0f);
    gainProcessorA.setRampDurationSeconds(0.001f);
    gainProcessorB.setGainLinear(0.0f);
    gainProcessorB.setRampDurationSeconds(0.001f);
}

DualVCAProcessor::~DualVCAProcessor() {}

void DualVCAProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    // Prepare DSP modules
    dsp::ProcessSpec processSpec{ sampleRate, static_cast<uint32> (maximumExpectedSamplesPerBlock) };
    this->gainProcessorA.prepare(processSpec);
    this->gainProcessorB.prepare(processSpec);
}

void DualVCAProcessor::releaseResources() {}

void DualVCAProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    // Write CV inputs to displays
    *cvADisplay = (buffer.getRMSLevel(cvAChannel, 0, buffer.getNumSamples()));
    *cvBDisplay = (buffer.getRMSLevel(cvBChannel, 0, buffer.getNumSamples()));

    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        // Process A block
        if (isInputConnected[inputAAChannel] || isInputConnected[inputABChannel])
        {
            gainProcessorA.setGainLinear(
                ParameterUtils::calculateModulationLinear(
                    *gainAmountA, 
                    buffer.getSample(cvAChannel, sample),
                    *cvAmountA,
                    0.0f,
                    4.0f
                )
            );

            buffer.setSample(outputAChannel, sample,
                gainProcessorA.processSample(
                    buffer.getSample(inputAAChannel, sample) 
                    + buffer.getSample(inputABChannel, sample)
                )
            );
        }

        // Process B block
        if (isInputConnected[inputBAChannel] || isInputConnected[inputBBChannel])
        {
            gainProcessorB.setGainLinear(
                ParameterUtils::calculateModulationLinear(
                    *gainAmountB,
                    buffer.getSample(cvBChannel, sample),
                    *cvAmountB,
                    0.0f,
                    4.0f
                )
            );

            buffer.setSample(outputBChannel, sample,
                gainProcessorA.processSample(
                    buffer.getSample(inputBAChannel, sample)
                    + buffer.getSample(inputBBChannel, sample)
                )
            );
        }
    }

    // Write outputs to display
    *outADisplay = (buffer.getRMSLevel(outputAChannel, 0, buffer.getNumSamples()));
    *outBDisplay = (buffer.getRMSLevel(outputBChannel, 0, buffer.getNumSamples()));
}