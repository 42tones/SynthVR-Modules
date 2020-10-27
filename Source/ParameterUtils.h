/*
  ==============================================================================
  ==============================================================================
*/

#pragma once

#include <algorithm>
#include "JuceHeader.h"

using namespace juce;

namespace synthvr
{
    class ParameterUtils {
    public:
        static float clamp(float value, float min, float max)
        {
            return std::max(min, std::min(value, max));
        }

        static float calculateModulationLinear(float originalValue, float modulationValue, float modulationAmount, float clampLow = 0.0f, float clampHigh = 1.0f)
        {
            return ParameterUtils::clamp(originalValue + (modulationValue * modulationAmount), clampLow, clampHigh);
        }

        static float calculateModulationMultiply(float originalValue, float modulationValue, float modulationAmount, float clampLow = 0.0f, float clampHigh = 1.0f)
        {
            return ParameterUtils::clamp(originalValue + (originalValue * (modulationValue * modulationAmount)), clampLow, clampHigh);
        }

        static float calculateModulationFrequency(float originalValue, float modulationValue, float modulationAmount, float clampLow = 0.0f, float clampHigh = 1.0f)
        {
            return ParameterUtils::clamp(originalValue + (originalValue * std::pow(2.0f, 5.0f * modulationValue * modulationAmount) - originalValue), clampLow, clampHigh);
        }

        static float equalPowerCrossFade(float sampleA, float sampleB, float mix)
        {
            auto targetGain = dsp::FastMathApproximations::sin(mix * MathConstants<float>::halfPi);
            return (sampleA * (1.0f - targetGain)) + (sampleB * targetGain);
        }
    };
}

