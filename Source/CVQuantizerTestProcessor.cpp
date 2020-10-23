/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "CVQuantizerTestProcessor.h"

using namespace synthvr;

CVQuantizerTestProcessor::CVQuantizerTestProcessor() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(4))
    .withOutput("Output", AudioChannelSet::discreteChannels(4)))
{
    addParameter(scale = new AudioParameterChoice("scale", "The musical scale to quantize to.", scaleNames, 0));
}

CVQuantizerTestProcessor::~CVQuantizerTestProcessor() {}

void CVQuantizerTestProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        for (int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            // Store sign of the incoming sample, used at the end
            isPositive = buffer.getSample(channel, sample) >= 0.0f;

            // Map sample into a positive 5 octave range
            remappedSample = buffer.getSample(channel, sample) * notesInOctave * octavesInRange;
            remappedSample = std::round(remappedSample);
            remappedSample = std::abs(remappedSample);

            // Get the desired octave and note from remapped signal
            octaveInSample = remappedSample / notesInOctave;
            noteInSample = remappedSample % notesInOctave;

            // Find the best matching note in desired scale
            for (int note = 0; note < scaleNotes[*scale].size(); note++)
            {
                if (noteInSample >= scaleNotes[*scale][note])
                    selectedNote = scaleNotes[*scale][note];

                if (noteInSample == scaleNotes[*scale][note])
                    break;
            }

            // Map selection back to normalized float range
            buffer.setSample(channel, sample, (
                ((float)(octaveInSample * notesInOctave + selectedNote)
                / (float)notesInOctave)
                / (float)octavesInRange)
                * getSignMultiplier(isPositive)
            );
        }
    }
}

float CVQuantizerTestProcessor::getSignMultiplier(bool positive)
{
    if (positive)
        return 1.0f;
    else
        return -1.0f;
}