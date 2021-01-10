#include "SingleSampleLadderFilter.h"

//==============================================================================
template <typename SampleType>
SingleSampleLadderFilter<SampleType>::SingleSampleLadderFilter() : state(2)
{
    setSampleRate(SampleType(1000));  // intentionally setting unrealistic default
                                        // sample rate to catch missing initialisation bugs
    setResonance(SampleType(0));
    setDrive(SampleType(1.2));

    mode = Mode::LPF24;
    setMode(Mode::LPF12);
}

//==============================================================================
template <typename SampleType>
void SingleSampleLadderFilter<SampleType>::setMode(Mode newMode) noexcept
{
    if (newMode == mode)
        return;

    switch (newMode)
    {
    case Mode::LPF12:   A = { { SampleType(0), SampleType(0),  SampleType(1), SampleType(0),  SampleType(0) } }; comp = SampleType(0.5);  break;
    case Mode::HPF12:   A = { { SampleType(1), SampleType(-2), SampleType(1), SampleType(0),  SampleType(0) } }; comp = SampleType(0);    break;
    case Mode::BPF12:   A = { { SampleType(0), SampleType(0), SampleType(-1), SampleType(1),  SampleType(0) } }; comp = SampleType(0.5);  break;
    case Mode::LPF24:   A = { { SampleType(0), SampleType(0),  SampleType(0), SampleType(0),  SampleType(1) } }; comp = SampleType(0.5);  break;
    case Mode::HPF24:   A = { { SampleType(1), SampleType(-4), SampleType(6), SampleType(-4), SampleType(1) } }; comp = SampleType(0);    break;
    case Mode::BPF24:   A = { { SampleType(0), SampleType(0),  SampleType(1), SampleType(-2), SampleType(1) } }; comp = SampleType(0.5);  break;
    default:            jassertfalse;                                                                                                         break;
    }

    static constexpr auto outputGain = SampleType(1.2);

    for (auto& a : A)
        a *= outputGain;

    mode = newMode;
    reset();
}

//==============================================================================
template <typename SampleType>
void SingleSampleLadderFilter<SampleType>::prepare(const dsp::ProcessSpec& spec)
{
    setSampleRate(SampleType(spec.sampleRate));
    setNumChannels(std::min(spec.numChannels, (uint32) 1));
    reset();
}

//==============================================================================
template <typename SampleType>
void SingleSampleLadderFilter<SampleType>::reset() noexcept
{
    for (auto& s : state)
        s.fill(SampleType(0));
}

//==============================================================================
template <typename SampleType>
void SingleSampleLadderFilter<SampleType>::setCutoffFrequencyHz(SampleType newCutoff) noexcept
{
    jassert(newCutoff > SampleType(0));
    cutoffFreqHz = newCutoff;
    updateCutoffFreq();
}

//==============================================================================
template <typename SampleType>
void SingleSampleLadderFilter<SampleType>::setResonance(SampleType newResonance) noexcept
{
    jassert(newResonance >= SampleType(0) && newResonance <= SampleType(1));
    resonance = newResonance;
    updateResonance();
}

//==============================================================================
template <typename SampleType>
void SingleSampleLadderFilter<SampleType>::setDrive(SampleType newDrive) noexcept
{
    jassert(newDrive >= SampleType(1));

    drive = newDrive;
    gain = std::pow(drive, SampleType(-2.642)) * SampleType(0.6103) + SampleType(0.3903);
    drive2 = drive * SampleType(0.04) + SampleType(0.96);
    gain2 = std::pow(drive2, SampleType(-2.642)) * SampleType(0.6103) + SampleType(0.3903);
}

//==============================================================================
template <typename SampleType>
SampleType SingleSampleLadderFilter<SampleType>::processSample(SampleType inputValue, size_t channelToUse) noexcept
{
    auto& s = state[channelToUse];

    const auto a1 = cutoffTransformValue;
    const auto g = a1 * SampleType(-1) + SampleType(1);
    const auto b0 = g * SampleType(0.76923076923);
    const auto b1 = g * SampleType(0.23076923076);

    const auto dx = gain * saturationLUT(drive * inputValue);
    const auto a = dx + scaledResonanceValue * SampleType(-4) * (gain2 * saturationLUT(drive2 * s[4]) - dx * comp);

    const auto b = b1 * s[0] + a1 * s[1] + b0 * a;
    const auto c = b1 * s[1] + a1 * s[2] + b0 * b;
    const auto d = b1 * s[2] + a1 * s[3] + b0 * c;
    const auto e = b1 * s[3] + a1 * s[4] + b0 * d;

    s[0] = a;
    s[1] = b;
    s[2] = c;
    s[3] = d;
    s[4] = e;

    return a * A[0] + b * A[1] + c * A[2] + d * A[3] + e * A[4];
}

//==============================================================================
template <typename SampleType>
void SingleSampleLadderFilter<SampleType>::setSampleRate(SampleType newValue) noexcept
{
    jassert(newValue > SampleType(0));
    cutoffFreqScaler = SampleType(-2.0 * juce::MathConstants<double>::pi) / newValue;
    updateCutoffFreq();
}

//==============================================================================
template class SingleSampleLadderFilter<float>;
template class SingleSampleLadderFilter<double>;
