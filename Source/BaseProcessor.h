/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "json.hpp"

using namespace juce;
using json = nlohmann::json;

namespace synthvr
{
    class BaseProcessor : public AudioProcessor {
    public:
        BaseProcessor(const BusesProperties& properties) : AudioProcessor(properties) {}
        BaseProcessor() {}
        ~BaseProcessor() {}

        virtual void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override {}
        virtual void releaseResources() override {}

        // DEFAULT PROPERTIES
        //==============================================================================
        bool acceptsMidi() const override                      { return false; }
        bool producesMidi() const override                     { return false; }
        double getTailLengthSeconds() const override           { return 0; }
        int getNumPrograms() override                          { return 1; }
        int getCurrentProgram() override                       { return 0; }
        void setCurrentProgram (int) override                  {}
        const String getProgramName (int) override             { return {}; }
        void changeProgramName (int, const String&) override   {}
        bool isBusesLayoutSupported (const BusesLayout& layouts) const override { return true; }
        AudioProcessorEditor* createEditor() override { return nullptr; }
        bool hasEditor() const override { return false; }

        // TODO: Implement functions to get and set state with JSON binary instead of JSON string
        void setStateInformation(const void* data, int sizeInBytes) override {}
        void getStateInformation(MemoryBlock& destData) override {}

        const std::string getStateInformationJSON()
        {
            json parameterStates;

            // Store values of all parameters, using their ID as the JSON field
            auto parameters = getParameters();
            for (int i = 0; i < parameters.size(); ++i)
                if (AudioProcessorParameterWithID* p = dynamic_cast<AudioProcessorParameterWithID*> (parameters[i]))
                {
                    json parameterState;
                    parameterState["ID"] = p->paramID.toStdString();
                    parameterState["Value"] = p->getValue();
                    parameterStates.push_back(parameterState);
                }

            json state;
            state["ProcessorName"] = getName().toStdString();
            state["Parameters"] = parameterStates;

            return state.dump();
        }

        const void setStateInformationJSON(std::string jsonString)
        {
            try {
                auto state = json::parse(jsonString);

                auto parameters = getParameters();
                auto parameterStates = state["Parameters"];

                for (int i = 0; i < parameters.size(); ++i)
                    if (AudioProcessorParameterWithID* p = dynamic_cast<AudioProcessorParameterWithID*> (parameters[i]))
                    {
                        auto id = p->paramID.toStdString();

                        for (int j = 0; j < parameterStates.size(); j++)
                            if (parameterStates[j]["ID"] == id)
                                p->setValue(parameterStates[j]["Value"]);
                    }
            } catch(const std::exception& e) {
#if JUCE_DEBUG
                // TODO: Log exception to file or something
#endif
            }

            onStateUpdated();
        }

        void setFloatArrayData(const float* pointer, int length, int index)
        {
            // Only write float data if the processor preallocated some memory for it
            if (index < floatArrayData.size())
            {
                floatArrayData[index] = std::vector<float>(pointer, pointer + length);
                onFloatArrayDataUpdated();
            }
        }

        void setChannelConnected(int channel, bool isInput, bool isConnected)
        {
            if (isInput && channel < isInputConnected.size())
                isInputConnected[channel] = isConnected;
            else if (channel < isOutputConnected.size())
                isOutputConnected[channel] = isConnected;
        }

    protected:
        std::vector<std::vector<float>> floatArrayData = std::vector<std::vector<float>>();
        std::vector<bool> isInputConnected = std::vector<bool>(getNumInputChannels());
        std::vector<bool> isOutputConnected = std::vector<bool>(getNumOutputChannels());

        virtual void onStateUpdated() {}
        virtual void onConnectionsUpdated() {}
        virtual void onFloatArrayDataUpdated() {}

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseProcessor)
    };
}