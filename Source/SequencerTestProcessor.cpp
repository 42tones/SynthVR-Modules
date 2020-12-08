/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "SequencerTestProcessor.h"

using namespace synthvr;

SequencerTestProcessor::SequencerTestProcessor() : BaseProcessor(BusesProperties()
        .withInput("Input", AudioChannelSet::discreteChannels(2))
        .withOutput ("Output", AudioChannelSet::discreteChannels(2)))
{
    addParameter(gateLength = new AudioParameterFloat ("gateLength", "The length of the gate as a ratio of the current step size", 0.0f, 1.0f, 0.5f));
    addParameter(clockDivision = new AudioParameterInt ("clockDivision", "The factor to divide clock rate by.", 0, 5, 1));

    stepsPitchIndices = std::vector<int>();
    stepsOnIndices = std::vector<int>();

    // For each step in the sequence
    for (int i = 0; i < numSteps; i++)
    {
        // Add step pitch parameter
        addParameter(new AudioParameterFloat("stepPitch_" + std::to_string(i), "Sets the pitch of a step.", 0.0f, 1.0f, 0.5f));
        stepsPitchIndices.push_back((i * 2) + 2);
        addParameter(new AudioParameterBool("stepOn_" + std::to_string(i), "Enables or disables a step.", true));
        stepsOnIndices.push_back((i * 2) + 3);
    }

    addParameter(currentStepDisplay = new AudioParameterInt ("currentStepDisplay", "Displays current step that sequencer is on", 0, numSteps, 0));
    addParameter(currentlyTriggeredDisplay = new AudioParameterBool ("currentlyTriggeredDisplay", "Displays if the sequencer is triggered right now.", false));
}

SequencerTestProcessor::~SequencerTestProcessor() {}

void SequencerTestProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
}

void SequencerTestProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &)
{
    for(int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        // Reset if applicable
        if (buffer.getSample(1, sample) >= 0.5f &&
            std::abs(buffer.getSample(1, sample) - previousResetValue) >= 0.5f)
        {
            ResetState();
            HandleClockTrigger();
        }

        // If sample is over threshold and we're not on a clock division step, trigger next step
        if (buffer.getSample(0, sample) >= 0.5f &&
            std::abs(buffer.getSample(0, sample) - previousClockValue) >= 0.5f &&
            ++ticksSinceLastSequencerTrigger >= std::max(*clockDivision * 2, 1))
        {
            HandleClockTrigger();
        }
        else
        {
            currentlyTriggered = false;
        }

        // Store clock value
        previousClockValue = buffer.getSample(0, sample);
        previousResetValue = buffer.getSample(1, sample);
        samplesSinceLastClockTrigger++;

        // The trigger status for the sample can be overridden if there was a previous trigger step that is still running
        if (!currentlyTriggered)
            currentlyTriggered = samplesSinceLastOutputTrigger++ < currentClockTriggerRate * *gateLength;

        // Write results to output buffer
        buffer.setSample(0, sample, currentPitchValue);
        buffer.setSample(1, sample, currentlyTriggered);
        *currentlyTriggeredDisplay = currentlyTriggered;
    }
}

void SequencerTestProcessor::ResetState()
{
    currentStep = -1;
    ticksSinceLastSequencerTrigger = 0;
    samplesSinceLastOutputTrigger = 0;
    samplesSinceLastClockTrigger = -1;
    currentlyTriggered = false;
}

void SequencerTestProcessor::HandleClockTrigger()
{
    // Reset all the counters
    ticksSinceLastSequencerTrigger = 0;

    // Get clock rate in samples
    currentClockTriggerRate = samplesSinceLastClockTrigger;
    samplesSinceLastClockTrigger = -1;

    // Cycle steps and reset to step 0 if we reached the end of steps
    if (++currentStep >= numSteps)
        currentStep = 0;

    *currentStepDisplay = currentStep;

    // Update pitch output based on param

    currentlyTriggered = getParameters()[stepsOnIndices[currentStep]]->getValue() >= 0.5f;
    
    if (currentlyTriggered)
    {
        samplesSinceLastOutputTrigger = 0;
        currentPitchValue = getParameters()[stepsPitchIndices[currentStep]]->getValue();
    }
}