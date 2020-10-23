/*
  ==============================================================================

    ProcessorAPIUtilities.h
    Created: 9 Mar 2020 7:24:03pm
    Author:  Daniel Rothmann

  ==============================================================================
*/


#pragma once

#include "JuceHeader.h"
#include "BaseProcessor.h"

namespace synthvr
{
    class ProcessorAPIUtilities {
    public:
        static int GetIndexForName(BaseProcessor* processor, char* name)
        {
            auto nameString = String(CharPointer_UTF8(name));
            auto nameLength = nameString.length();
            auto parameters = processor->getParameters();

            // Search all parameters for one with this name.
            for (int i = 0; i < parameters.size(); i++)
            {
                if (parameters[i]->getName(nameLength) == nameString)
                {
                    return i;
                }
            }

            // If no matching parameter was found, return -1
            return -1;
        }

        static void WriteToManagedString(std::string writeString,
                                         int writeStringLength,
                                         char* managedString,
                                         int managedStringLength)
        {
            writeString = writeString.substr(0, static_cast<unsigned long>(writeStringLength));
            std::copy(writeString.begin(), writeString.end(), managedString);
            managedString[std::min(managedStringLength-1, (int)writeString.size())] = 0;
        }
    };
}
