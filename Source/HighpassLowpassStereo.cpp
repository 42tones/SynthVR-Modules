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
    addParameter(gainParam = new AudioParameterFloat("InputGain", "Input Gain", 0.0f, 1.0f, 1.0f));
    
}



HighpassLowpassStereo::~HighpassLowpassStereo() {}

void HighpassLowpassStereo::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{

}

void HighpassLowpassStereo::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    if (isInputConnected[inputLeft] || isInputConnected[inputRight])
    {
        

       
    }
}
