/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/
#pragma once

#ifdef _MSC_VER
#define DLLExport __declspec(dllexport)
#else
#define DLLExport 
#endif

#include "JuceHeader.h"
#include "BaseProcessor.h"
#include "ProcessorAPIUtilities.h"
#include "ProcessorFactory.h"

using namespace synthvr;

extern "C"
{
    // ESSENTIAL
    // ==============================================================================
    DLLExport BaseProcessor* CreateProcessor(int processorID);
    DLLExport void DestroyProcessor(BaseProcessor* processor);

    // AUDIO PROCESSING
    // ==============================================================================
    DLLExport void PrepareToPlay(BaseProcessor* processor, double sampleRate, int maximumExpectedSamplesPerBlock);
    DLLExport void ReleaseResources(BaseProcessor* processor);
    DLLExport void ProcessBlock(BaseProcessor* processor, float** buffer, int numChannels, int numSamplesPerChannel);
    DLLExport int GetNumInputs(BaseProcessor* processor);
    DLLExport int GetNumOutputs(BaseProcessor* processor);
    DLLExport void CopyChannelPointer(float** fromBuffer, int fromChannel, float** toBuffer, int toChannel);
    DLLExport void CopyBuffer(float** fromBuffer, int numFromChannels, int targetFromChannel,
                    float** toBuffer, int numToChannels, int targetToChannel,
                    int samplesPerChannel);

// PARAMETERS
    // ==============================================================================
    DLLExport float GetParameter(BaseProcessor* processor, int index);
    DLLExport float GetParameterByName(BaseProcessor* processor, char* name);
    DLLExport void SetParameter(BaseProcessor* processor, int index, float value);
    DLLExport void SetParameterByName(BaseProcessor* processor, char* name, float value);

    DLLExport void GetParameterName(BaseProcessor* processor, int index, char* destinationString, int stringLength);
    DLLExport int GetParameterType(BaseProcessor* processor, int index);
    DLLExport int GetNumParameters(BaseProcessor* processor);

    DLLExport void GetParameterValueAsString(BaseProcessor* processor, int index, char* destinationString, int stringLength);

    // SAVE / LOAD PARAMETER STATE
    // ==============================================================================
    DLLExport void GetStateInformationJSON(BaseProcessor* processor, char* destinationString, int stringLength);
    DLLExport void SetStateInformationJSON(BaseProcessor* processor, char* jsonString);
    DLLExport void SetFloatArrayData(BaseProcessor* processor, float* data, int length, int index);
}
