#include "JuceHeader.h"

class StoreChannelAudioCallback : public AudioIODeviceCallback
{
public:
    StoreChannelAudioCallback()
    {
    }

    ~StoreChannelAudioCallback()
    {
    }

    AudioBuffer<float>* getBuffer()
    {
        return &internalBuffer;
    }

    void copyTo(AudioBuffer<float>* buffer)
    {
        buffer->copyFrom(0, 0, internalBuffer.getReadPointer(0), internalBuffer.getNumSamples());
    }

    //==============================================================================
    void audioDeviceAboutToStart(AudioIODevice* device)
    {
        currentBufferSize = device->getCurrentBufferSizeSamples();
        internalBuffer = AudioBuffer<float>(1, currentBufferSize);
    }

    void audioDeviceStopped()
    {
    }

    void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
        float** outputChannelData, int numOutputChannels,
        int numSamples)
    {
        // Copy data to internal buffer
        for (int sample = 0; sample < currentBufferSize; sample++)
            internalBuffer.setSample(0, sample, inputChannelData[0][sample]);

        // Clear the output buffers, in case they're full of junk..
        for (int i = 0; i < numOutputChannels; ++i)
            if (outputChannelData[i] != 0)
                zeromem(outputChannelData[i], sizeof(float) * numSamples);
    }

private:
    int currentBufferSize;
    AudioBuffer<float> internalBuffer;
};