#include "MultiModeLadderFilter.h"

using namespace synthvr;

MultiModeLadderFilter::MultiModeLadderFilter() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(4))
    .withOutput("Output", AudioChannelSet::discreteChannels(1)))
{
    // Parameters
    addParameter(mixParam = new AudioParameterFloat("mix", "The mix between the original and filtered signal.", 0.0f, 1.0f, 1.0f));
    addParameter(mixModulationAmountParam = new AudioParameterFloat("mixModulationAmount", "The amount to modulate the mix value.", -1.0f, 1.0f, 0.2f));
    addParameter(frequencyParam = new AudioParameterFloat("frequency", "The frequency of the filter.", minFrequency, maxFrequency, 3000.0f));
    addParameter(frequencyModulationAAmountParam = new AudioParameterFloat("frequencyModulationAAmount", "The amount to modulate the filter frequency.", -1.0f, 1.0f, 0.0f));
    addParameter(frequencyModulationBAmountParam = new AudioParameterFloat("frequencyModulationBAmount", "The amount to modulate the filter frequency.", -1.0f, 1.0f, 0.0f));
    addParameter(resonanceParam = new AudioParameterFloat("resonance", "The resonance of the filter.", minResonance, maxResonance, 0.0f));
    addParameter(driveParam = new AudioParameterFloat("drive", "The saturation of the filter.", 1.0f, maxDrive, 1.0f));
    addParameter(modeParam = new AudioParameterInt("mode", "The filtering mode to use.", 0, 2, 0));
    addParameter(rollOffParam = new AudioParameterBool("rollOff", "Whether to use 12dB/oct filter rolloff or 24db/oct.", true));
    addParameter(frequencyDisplay = new AudioParameterFloat("frequencyDisplay", "Displays the current normalized frequency value.", minFrequency, maxFrequency, 3000.0f));

    frequencyParam->range.skew = frequencySkew;
    frequencyDisplay->range.skew = frequencySkew;
    resonanceParam->range.skew = 1.5f;
}

MultiModeLadderFilter::~MultiModeLadderFilter() {}

void MultiModeLadderFilter::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    // Set filter mode
    currentMode = calculateMode(*modeParam, *rollOffParam);

    // Prepare parameter smoothers
    smoothedMixParam.reset(sampleRate, paramSmoothingTime);
    smoothedFrequencyParam.reset(sampleRate, paramSmoothingTime);
    smoothedResonanceParam.reset(sampleRate, paramSmoothingTime);

    smoothedMixParam.setCurrentAndTargetValue(*mixParam);
    smoothedFrequencyParam.setCurrentAndTargetValue(*frequencyParam);
    smoothedResonanceParam.setCurrentAndTargetValue(*resonanceParam);

    // Prepare DSP modules
    dsp::ProcessSpec processSpec{ sampleRate, static_cast<uint32> (maximumExpectedSamplesPerBlock), 1 };
    this->filterProcessor.prepare(processSpec);
    this->filterProcessor.setEnabled(true);
    this->filterProcessor.setMode(static_cast<SingleSampleLadderFilterMode>(currentMode));
    this->filterProcessor.setCutoffFrequencyHz(smoothedFrequencyParam.getCurrentValue());
    this->filterProcessor.setResonance(*resonanceParam);
    this->filterProcessor.setDrive(*driveParam);
}

void MultiModeLadderFilter::releaseResources() {}

void MultiModeLadderFilter::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    // Check if the filter mode has changed
    if (calculateMode(*modeParam, *rollOffParam) != currentMode)
    {
        currentMode = calculateMode(*modeParam, *rollOffParam);
        this->filterProcessor.setMode(static_cast<SingleSampleLadderFilterMode>(currentMode));
    }

    // Update smoothers
    smoothedMixParam.setTargetValue(*mixParam);
    smoothedFrequencyParam.setTargetValue(*frequencyParam);
    smoothedResonanceParam.setTargetValue(*resonanceParam);

    // Process samples
    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        // Run first frequency modulator
        if (isInputConnected[frequencyAInputChannel])
            currentFrequency = ParameterUtils::calculateModulationFrequency(
                smoothedFrequencyParam.getNextValue(),
                buffer.getSample(frequencyAInputChannel, sample),
                *frequencyModulationAAmountParam,
                minFrequency,
                maxFrequency);
        else
            currentFrequency = smoothedFrequencyParam.getNextValue();

        // Add second frequency modulator
        if (isInputConnected[frequencyBInputChannel])
            currentFrequency = ParameterUtils::calculateModulationFrequency(
                currentFrequency,
                buffer.getSample(frequencyBInputChannel, sample),
                *frequencyModulationBAmountParam,
                minFrequency,
                maxFrequency);

        this->filterProcessor.setCutoffFrequencyHz(currentFrequency);
        this->filterProcessor.setResonance(smoothedResonanceParam.getNextValue());
        this->filterProcessor.setDrive(*driveParam);

        if (isInputConnected[mixInputChannel])
            currentMix = ParameterUtils::calculateModulationMultiply(
                smoothedMixParam.getNextValue(),
                buffer.getSample(mixInputChannel, sample),
                *mixModulationAmountParam,
                0.0f,
                1.0f);
        else
            currentMix = smoothedMixParam.getNextValue();

        currentFilterOutput = this->filterProcessor.processSample(buffer.getSample(inputChannel, sample), 0);
        buffer.setSample(outputChannel, 
            sample, 
            (currentFilterOutput * currentMix) + (buffer.getSample(inputChannel, sample) * (1.0f - currentMix)));
    }

    // Set display
    *frequencyDisplay = currentFrequency;
}