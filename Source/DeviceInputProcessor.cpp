/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "DeviceInputProcessor.h"

using namespace synthvr;

DeviceInputProcessor::DeviceInputProcessor() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(1))
    .withOutput("Output", AudioChannelSet::discreteChannels(1)))
{
    deviceManager.initialiseWithDefaultDevices(1, 0);
    deviceManager.addAudioCallback(&callback);
    
    // Parameters
    // addParameter(gainAmountA = new AudioParameterFloat("gainA", "Gain A", -0.1f, maxGainAmount, 1.0f));
}

DeviceInputProcessor::~DeviceInputProcessor() 
{
    deviceManager.removeAudioCallback(&callback);
}

void DeviceInputProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    //auto currentDevice = deviceManager.getCurrentAudioDevice();
    //
    //if (!currentDevice->isOpen())
    //    currentDevice->open(0, 0, sampleRate, maximumExpectedSamplesPerBlock);

    //if (!currentDevice->isPlaying())
    //    currentDevice->start()
}

void DeviceInputProcessor::releaseResources() {}

void DeviceInputProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    callback.copyTo(&buffer);
}