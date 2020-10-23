/*
  ==============================================================================
  ==============================================================================
*/

#pragma once

#include <algorithm>

namespace synthvr
{
    class ParameterUtils {
    public:
        static float clamp(float value, float min, float max)
        {
            return std::max(min, std::min(value, max));
        }
    };
}

