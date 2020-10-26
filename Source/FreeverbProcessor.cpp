/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "FreeverbProcessor.h"

using namespace synthvr;

FreeverbProcessor::FreeverbProcessor() : BaseProcessor(BusesProperties()
        .withInput("Inputs", AudioChannelSet::discreteChannels(5))
        .withOutput ("Output", AudioChannelSet::discreteChannels(2)))
{
    addParameter(roomSizeParam = new AudioParameterFloat ("roomSize", "The size of the room to use.", 0.0f, 1.0f, 0.6f));
    addParameter(roomSizeModulationAmountParam = new AudioParameterFloat("roomSizeModulationAmount", "The amount of modulation to apply.", -1.0f, 1.0f, 0.0f));
    addParameter(dampingParam = new AudioParameterFloat ("damping", "The amount of damping to use.", 0.0f, 1.0f, 0.4f));
    addParameter(widthParam = new AudioParameterFloat ("width", "The stereo width of the reverb.", 0.0f, 1.0f, 1.0f));
    addParameter(mixParam = new AudioParameterFloat ("mix", "The mix between dry and wet signal.", 0.0f, 1.0f, 1.0f));
    addParameter(mixModulationAmountParam = new AudioParameterFloat("mixModulationAmount", "The amount of modulation to apply.", -1.0f, 1.0f, 0.0f));
    addParameter(freezeParam = new AudioParameterBool("freeze", "When on, freezes the reverb.", false));
    addParameter(freezeDisplay = new AudioParameterFloat("freezeDisplay", "Displays the result of freeze button + signal.", 0.0f, 1.0f, 0.0f));

    roomSizeParam->range.skew = 1.5f;
}

FreeverbProcessor::~FreeverbProcessor() {}

void FreeverbProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    reverb.reset();
    reverb.setSampleRate(sampleRate);
}

void FreeverbProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &)
{
    // Update block rate reverb params
    reverbParams.damping = *dampingParam;
    reverbParams.width = *widthParam;

    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        // Update audio rate reverb params
        reverbParams.roomSize = ParameterUtils::calculateModulationLinear(
            *roomSizeParam, 
            buffer.getSample(roomSizeInputChannel, sample), 
            *roomSizeModulationAmountParam);

        *freezeDisplay = ParameterUtils::calculateModulationLinear(
            *freezeParam,
            buffer.getSample(freezeInputChannel, sample),
            1.0f);

        reverbParams.freezeMode = *freezeDisplay;

        mixValue = ParameterUtils::calculateModulationLinear(
            *mixParam,
            buffer.getSample(mixInputChannel, sample),
            *mixModulationAmountParam);

        reverbParams.wetLevel = mixValue;
        reverbParams.dryLevel = 1.0f - mixValue;
        reverb.setParameters(reverbParams);

        // Run stereo reverb
        reverb.processStereo(&buffer.getWritePointer(0)[sample], &buffer.getWritePointer(1)[sample], 1);
    }
}

float FreeverbProcessor::calculateModulation(float originalValue, float modulationValue, float modulationAmount, float clampLow, float clampHigh)
{
    return ParameterUtils::clamp(originalValue + (modulationValue * modulationAmount), clampLow, clampHigh);
}