/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/
#include "ProcessorAPI.h"

using namespace synthvr;

BaseProcessor* CreateProcessor(int processorID)
{
    return ProcessorFactory::CreateProcessorFromID(processorID);
}

void DestroyProcessor(BaseProcessor* processor)
{
    delete processor;
}

void PrepareToPlay(BaseProcessor* processor, double sampleRate, int maximumExpectedSamplesPerBlock)
{
    processor->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
}

void ReleaseResources(BaseProcessor* processor)
{
    processor->releaseResources();
}

void ProcessBlock(BaseProcessor* processor, float** buffer, int numChannels, int numSamplesPerChannel)
{
    int expectedNumChannels = std::max(processor->getTotalNumInputChannels(), processor->getTotalNumOutputChannels());

    if(numChannels == expectedNumChannels) {
        MidiBuffer* midiData = new MidiBuffer();
        AudioBuffer<float>* audioData = new AudioBuffer<float>(buffer, numChannels, numSamplesPerChannel);
        processor->processBlock(*audioData, *midiData);
    }
}

int GetNumInputs(BaseProcessor* processor)
{
    return processor->getTotalNumInputChannels();
}

int GetNumOutputs(BaseProcessor* processor)
{
    return processor->getTotalNumOutputChannels();
}

void CopyChannelPointer(float** fromBuffer, int fromChannel,
                        float** toBuffer, int toChannel)
{
    toBuffer[toChannel] = fromBuffer[fromChannel];
}

void CopyBuffer(float** fromBuffer, int numFromChannels, int targetFromChannel,
                float** toBuffer, int numToChannels, int targetToChannel,
                int samplesPerChannel)
{
    AudioBuffer<float>* toAudioBuffer = new AudioBuffer<float>(toBuffer, numToChannels, samplesPerChannel);
    toAudioBuffer->copyFrom(targetToChannel, 0, fromBuffer[targetFromChannel], 0, samplesPerChannel);
}

float GetParameter(BaseProcessor* processor, int index)
{
    auto parameters = processor->getParameters();
    if (index < 0 || index >= parameters.size())
        return 0.0f;
    else
        return parameters[index]->getValue();
}

float GetParameterByName(BaseProcessor* processor, char* name)
{
    int index = ProcessorAPIUtilities::GetIndexForName(processor, name);
    if (index == -1)
        return 0.0f;
    else
        return GetParameter(processor, index);
}

void SetParameter(BaseProcessor* processor, int index, float value)
{
    auto parameters = processor->getParameters();
    if (!(index < 0 || index >= parameters.size()))
        parameters[index]->setValue(value);
}

void SetParameterByName(BaseProcessor* processor, char* name, float value)
{
    int index = ProcessorAPIUtilities::GetIndexForName(processor, name);
    if (index != -1)
        SetParameter(processor, index, value);
}

void GetParameterName(BaseProcessor* processor, int index, char* destinationString, int stringLength)
{
    auto parameters = processor->getParameters();
    std::string name = "PARAMETER_DOES_NOT_EXIST";

    // Get the name of the parameter if it exists.
    if (index >= 0 && index < parameters.size())
        name = parameters[index]->getName(100).toStdString();

    ProcessorAPIUtilities::WriteToManagedString(name, (int)name.size(), destinationString, stringLength);
}

int GetNumParameters(BaseProcessor* processor)
{
    return processor->getParameters().size();
}

int GetParameterType(BaseProcessor* processor, int index)
{
    auto parameters = processor->getParameters();
    if (index < 0 || index >= parameters.size())
        return ProcessorFactory::ParameterType::Continuous;
    else
    {
        if (parameters[index]->isBoolean())
            return ProcessorFactory::ParameterType::Boolean;
        else if (parameters[index]->isDiscrete())
            return ProcessorFactory::ParameterType::Discrete;
        else
            return ProcessorFactory::ParameterType::Continuous;
    }
}

void GetParameterValueAsString(BaseProcessor* processor, int index, char* destinationString, int stringLength)
{
    auto parameters = processor->getParameters();
    std::string value = "VALUE_COULD_NOT_BE_CONVERTED";

    // Get the name of the parameter if it exists.
    if (index >= 0 && index < parameters.size())
        value = parameters[index]->getText(parameters[index]->getValue(), stringLength).toStdString();

    ProcessorAPIUtilities::WriteToManagedString(value, (int)value.size(), destinationString, stringLength);
}

void GetStateInformationJSON(BaseProcessor* processor, char* destinationString, int stringLength)
{
    auto state = processor->getStateInformationJSON();
    ProcessorAPIUtilities::WriteToManagedString(state, (int)state.size(), destinationString, stringLength);
}

void SetStateInformationJSON(BaseProcessor* processor, char* jsonString)
{
    auto jsonStdString = std::string(jsonString);
    processor->setStateInformationJSON(jsonStdString);
}

void SetFloatArrayData(BaseProcessor* processor, float* data, int length, int index)
{
    processor->setFloatArrayData(data, length, index);
}

DLLExport void SetChannelConnected(BaseProcessor* processor, int channel, bool isInput, bool isConnected)
{
    processor->setChannelConnected(channel, isInput, isConnected);
}
