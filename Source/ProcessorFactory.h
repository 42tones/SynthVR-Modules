/*
  ==============================================================================

    ProcessorFactory.h
    Created: 15 Mar 2020 4:04:23pm
    Author:  Daniel Rothmann

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

// LAB & test modules
#include "SingleChannelTestProcessor.h"
#include "MultiChannelTestProcessor.h"
#include "OscillatorTestProcessor.h"
#include "AmplifierTestProcessor.h"
#include "SignalSplitterProcessor.h"
#include "MasterClockProcessor.h"
#include "SequencerTestProcessor.h"
#include "EnvelopeGeneratorProcessor.h"
#include "MixerTestProcessor.h"
#include "VelocityTrackingProcessor.h"
#include "CVQuantizerTestProcessor.h"

// Release ready modules
#include "WavetableVCOProcessor.h"
#include "MultiModeLadderFilter.h"
#include "FreeverbProcessor.h"
#include "DelayProcessor.h"
#include "ColoredNoiseProcessor.h"

using Node = AudioProcessorGraph::Node;
using NodeID = AudioProcessorGraph::NodeID;

namespace synthvr
{
    class ProcessorFactory {
    public:
        enum ProcessorID {
            SingleChannelTestProcessor = 0,
            MultiChannelTestProcessor = 1,
            OscillatorTestProcessor = 2,
            AmplifierTestProcessor = 3,
            SignalSplitterProcessor = 4,
            MasterClockProcessor = 5,
            SequencerTestProcessor = 6,
            EnvelopeGeneratorProcessor = 7,
            MixerTestProcessor = 8,
            VelocityTrackingProcessor = 9,
            CVQuantizerTestProcessor = 10,
            WavetableVCOProcessor = 11,
            MultiModeLadderFilter = 12,
            FreeverbProcessor = 13,
            DelayProcessor = 14,
            ColoredNoiseProcessor = 15
        };
        
        enum ParameterType { Continuous = 0, Discrete = 1, Boolean = 2 };

        static BaseProcessor* CreateProcessorFromID(int id)
        {
            switch(id)
            {
                case ProcessorID::SingleChannelTestProcessor:
                    return reinterpret_cast<BaseProcessor *>(new class SingleChannelTestProcessor());
                    
                case ProcessorID::MultiChannelTestProcessor:
                    return reinterpret_cast<BaseProcessor *>(new class MultiChannelTestProcessor());
                    
                case ProcessorID::OscillatorTestProcessor:
                    return reinterpret_cast<BaseProcessor *>(new class OscillatorTestProcessor());

                case ProcessorID::AmplifierTestProcessor:
                    return reinterpret_cast<BaseProcessor *>(new class AmplifierTestProcessor());
                    
                case ProcessorID::SignalSplitterProcessor:
                    return reinterpret_cast<BaseProcessor *>(new class SignalSplitterProcessor());

                case ProcessorID::MasterClockProcessor:
                    return reinterpret_cast<BaseProcessor *>(new class MasterClockProcessor());

                case ProcessorID::SequencerTestProcessor:
                    return reinterpret_cast<BaseProcessor *>(new class SequencerTestProcessor());

                case ProcessorID::EnvelopeGeneratorProcessor:
                    return reinterpret_cast<BaseProcessor *>(new class EnvelopeGeneratorProcessor());

                case ProcessorID::MixerTestProcessor:
                    return reinterpret_cast<BaseProcessor*>(new class MixerTestProcessor());

                case ProcessorID::VelocityTrackingProcessor:
                    return reinterpret_cast<BaseProcessor*>(new class VelocityTrackingProcessor());

                case ProcessorID::CVQuantizerTestProcessor:
                    return reinterpret_cast<BaseProcessor*>(new class CVQuantizerTestProcessor());

                case ProcessorID::WavetableVCOProcessor:
                    return reinterpret_cast<BaseProcessor*>(new class WavetableVCOProcessor());

                case ProcessorID::MultiModeLadderFilter:
                    return reinterpret_cast<BaseProcessor*>(new class MultiModeLadderFilter());

                case ProcessorID::FreeverbProcessor:
                    return reinterpret_cast<BaseProcessor*>(new class FreeverbProcessor());

                case ProcessorID::DelayProcessor:
                    return reinterpret_cast<BaseProcessor*>(new class DelayProcessor());

                case ProcessorID::ColoredNoiseProcessor:
                    return reinterpret_cast<BaseProcessor*>(new class ColoredNoiseProcessor());

                default:
                    return reinterpret_cast<BaseProcessor *>(new class SingleChannelTestProcessor());
            }
        }
    };
}

