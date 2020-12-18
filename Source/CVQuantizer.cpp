/*
  ==============================================================================

    CVQuantizer.cpp
    Created: 18 Dec 2020 7:37:25am
    Author:  pc

  ==============================================================================
*/

#include "CVQuantizer.h"

CVQuantizer::CVQuantizer()
{
    // Use chromatic scale
    auto chromaticScale = std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    setScale(chromaticScale);
}
