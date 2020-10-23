/*
  ==============================================================================
    Docs go here.
  ==============================================================================
*/

#include "WavetableVCOProcessor.h"

using namespace synthvr;

WavetableVCOProcessor::WavetableVCOProcessor() : BaseProcessor(BusesProperties()
    .withInput("Inputs", AudioChannelSet::discreteChannels(3))
    .withOutput("Output", AudioChannelSet::discreteChannels(3)))
{
    // Parameters
    addParameter(coarseTuneParam = new AudioParameterInt("CoarseTune", "Basic tune of both osc.", -coarseTuneRange, coarseTuneRange, 0));
    addParameter(fineTuneParam = new AudioParameterFloat("FineTune", "Fine tune of both osc.", -detuneRange, detuneRange, 0.0f));
    addParameter(coarseOffsetAParam = new AudioParameterInt("CoarseOffsetA", "Offsets A osc by num notes.", 0, coarseTuneRange, 0));
    addParameter(coarseOffsetBParam = new AudioParameterInt("CoarseOffsetB", "Offsets B osc by num notes.", 0, coarseTuneRange, 0));
    addParameter(detuneParam = new AudioParameterFloat("Detune", "Detunes the oscs away from each other.", -detuneRange / 2.0f, detuneRange / 2.0f, 0.0f));

    addParameter(gainParam = new AudioParameterFloat("Gain", "Gain of oscillator", 0.0f, 1.0f, 1.0f));
    addParameter(fmAmountParam = new AudioParameterFloat("FMAmount", "Amount of FM to add from input", 0.0f, 2.0f, 0.5f));
    addParameter(oscMixParam = new AudioParameterFloat("OscMix", "Amount to get from each osc", 0.0f, 1.0f, 0.5f));

    addParameter(oscAWave = new AudioParameterFloat("OscAWave", "Used by Unity to track & serialize wave choice", 0.0f, 1.0f, 0.0f));
    addParameter(oscBWave = new AudioParameterFloat("OscBWave", "Used by Unity to track & serialize wave choice", 0.0f, 1.0f, 0.0f));
    addParameter(totalWaveMixDisplay = new AudioParameterFloat("TotalWaveMixDisplay", "Writes the total mix amount between the two waves.", 0.0f, 1.0f, 0.0f));

    // Initialize float data with room for 2 wave arrays
    floatArrayData = std::vector<std::vector<float>>(2);
    floatArrayData[0] = std::vector<float>(defaultWaveTableLength);
    floatArrayData[1] = std::vector<float>(defaultWaveTableLength);

    masterGain.setRampDurationSeconds(0.1f);
    masterGain.setGainLinear(*gainParam);
}

WavetableVCOProcessor::~WavetableVCOProcessor() {}

void WavetableVCOProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    this->sampleRate = sampleRate;

    // Prepare DSP modules
    computeWaveTables();
    updateFrequencies(0.0f, 0.0f);

    dsp::ProcessSpec processSpec{ sampleRate, static_cast<uint32> (maximumExpectedSamplesPerBlock) };
    this->masterGain.prepare(processSpec);
}

void WavetableVCOProcessor::releaseResources() {}

void WavetableVCOProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    // Update gain per sample block since it is not CV controlled anyway
    masterGain.setGainLinear(*gainParam);

    // Update oscillator waves
    if (waveTablesShouldBeRecomputed)
        computeWaveTables();

    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        updateFrequencies(
            buffer.getSample(fmInputChannel, sample) * *fmAmountParam, 
            buffer.getSample(pitchInputChannel, sample)
        );

        // Process oscillators and write them to buffer
        currentOscASample = oscA->process();
        currentOscBSample = oscB->process();
        buffer.setSample(oscAOutputChannel, sample, masterGain.processSample(currentOscASample));
        buffer.setSample(oscBOutputChannel, sample, masterGain.processSample(currentOscBSample));

        // Crossfade oscillators and write them to buffer
        calculateCrossfade(*oscMixParam + buffer.getSample(waveInputChannel, sample));

        buffer.setSample(
            mixOutputChannel,
            sample, 
            masterGain.processSample(
                currentOscASample * oscATargetGain
                + currentOscBSample * oscBTargetGain));
    }
}

float WavetableVCOProcessor::calculateFrequency(float coarseTune, float fineTune, float offsetTune, float detune, float fmInput, float pitchInput)
{
    // Start with base frequency
    auto freq = baseFrequency;

    // Calculate frequency based on pitch input (1V = 5 OCT)
    freq = freq * std::pow(2, 5.0f * pitchInput);

    // Offset by tune parameters
    freq = freq * std::pow(2, ((coarseTune + offsetTune + fineTune + detune) / 12.0f));

    // Add FM
    freq = freq + freq * fmInput;

    return ParameterUtils::clamp(freq, 0.01f, 12000.0f);
}

void WavetableVCOProcessor::calculateCrossfade(float mix)
{
    mix = ParameterUtils::clamp(mix, 0.0f, 1.0f);
    *totalWaveMixDisplay = mix;

    oscATargetGain = dsp::FastMathApproximations::cos(mix * M_PI_2);
    oscBTargetGain = 1.0f - oscATargetGain;
}

void WavetableVCOProcessor::computeWaveTables()
{
    // TODO: Can oscillator be optimized so it no loger needs to be re-instantiated?
    oscA = std::make_unique<WaveTableOsc>();
    oscB = std::make_unique<WaveTableOsc>();
    setOscWaveTable(oscA.get(), floatArrayData[0].data(), floatArrayData[0].size());
    setOscWaveTable(oscB.get(), floatArrayData[1].data(), floatArrayData[1].size());
    waveTablesShouldBeRecomputed = false;
}

void WavetableVCOProcessor::updateFrequencies(float fmInput, float pitchInput)
{
    freqToSet = calculateFrequency(
        *coarseTuneParam,
        *fineTuneParam,
        *coarseOffsetAParam,
        *detuneParam,
        fmInput,
        pitchInput);

    oscA->setFrequency(ParameterUtils::clamp(freqToSet, 0.0f, 20000.0f), sampleRate);

    freqToSet = calculateFrequency(
        *coarseTuneParam,
        *fineTuneParam,
        *coarseOffsetBParam,
        -*detuneParam,
        fmInput,
        pitchInput);

    oscB->setFrequency(ParameterUtils::clamp(freqToSet, 0.0f, 20000.0f), sampleRate);
}

