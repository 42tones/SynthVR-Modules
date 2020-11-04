//
//  WaveTableOsc.h
//
//  Created by Nigel Redmon on 2018-10-05
//  EarLevel Engineering: earlevel.com
//  Copyright 2018 Nigel Redmon
//
//  For a complete explanation of the wavetable oscillator and code,
//  read the series of articles by the author, starting here:
//  www.earlevel.com/main/2012/05/03/a-wavetable-oscillator—introduction/
//
//  This version has optimizations described here:
//  www.earlevel.com/main/2019/04/28/wavetableosc-optimized/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own purposes, free or commercial.
//

#ifndef WaveTableOsc_h
#define WaveTableOsc_h

class WaveTableOsc {
public:
    WaveTableOsc(void) {
        for (int idx = 0; idx < numWaveTableSlots; idx++) {
            mWaveTables[idx].topFreq = 0;
            mWaveTables[idx].waveTableLen = 0;
            mWaveTables[idx].waveTable = 0;
        }
    }
    ~WaveTableOsc(void) {
        for (int idx = 0; idx < numWaveTableSlots; idx++) {
            float *temp = mWaveTables[idx].waveTable;
            if (temp != 0)
                delete [] temp;
        }
    }

    //
    // setPhaseIncrement: Set normalized frequency, typically 0-0.5 (must be positive and less than 1!)
    //
    void setPhaseIncrement(double inc) {
        mPhaseInc = inc;

        // update the current wave table selector
        int curWaveTable = 0;
        while ((mPhaseInc >= mWaveTables[curWaveTable].topFreq) && (curWaveTable < (mNumWaveTables - 1))) {
            ++curWaveTable;
        }
        mCurWaveTable = curWaveTable;
    }

    //
    // SetFrequency: Set frequency in Hertz, converting to phase increment value
    //
    void setFrequency(double hertz, double sampleRate)
    {
        setPhaseIncrement(hertz / sampleRate);
    }

    //
    // setPhaseOffset: Phase offset for PWM, 0-1
    //
    void setPhaseOffset(double offset) {
        mPhaseOfs = offset;
    }

    //
    // updatePhase: Call once per sample
    //
    void updatePhase(void) {
        mPhasor += mPhaseInc;

        if (mPhasor >= 1.0)
            mPhasor -= 1.0;
    }

    void resetPhase()
    {
        mPhasor = 0;
    }

    //
    // process: Update phase and get output
    //
    float process(void) {
        updatePhase();
        return getOutput() * -1.0f;
    }

    //
    // getOutput: Returns the current oscillator output
    //
    float getOutput(void) {
        waveTable *waveTable = &mWaveTables[mCurWaveTable];

        if (waveTable->waveTableLen < 1)
            return 0.0f;

        // linear interpolation
        float temp = mPhasor * waveTable->waveTableLen;
        int intPart = temp;
        float fracPart = temp - intPart;
        float samp0 = waveTable->waveTable[intPart];
        float samp1 = waveTable->waveTable[intPart + 1];
        return samp0 + (samp1 - samp0) * fracPart;
    }

    //
    // getOutputMinusOffset
    //
    // for variable pulse width: initialize to sawtooth,
    // set phaseOfs to duty cycle, use this for osc output
    //
    // returns the current oscillator output
    //
    float getOutputMinusOffset() {
        waveTable *waveTable = &mWaveTables[mCurWaveTable];
        int len = waveTable->waveTableLen;
        float *wave = waveTable->waveTable;

        // linear
        float temp = mPhasor * len;
        int intPart = temp;
        float fracPart = temp - intPart;
        float samp0 = wave[intPart];
        float samp1 = wave[intPart+1];
        float samp = samp0 + (samp1 - samp0) * fracPart;

        // and linear again for the offset part
        float offsetPhasor = mPhasor + mPhaseOfs;
        if (offsetPhasor > 1.0)
            offsetPhasor -= 1.0;
        temp = offsetPhasor * len;
        intPart = temp;
        fracPart = temp - intPart;
        samp0 = wave[intPart];
        samp1 = wave[intPart+1];
        return samp - (samp0 + (samp1 - samp0) * fracPart);
    }

    //
    // addWaveTable
    //
    // add wavetables in order of lowest frequency to highest
    // topFreq is the highest frequency supported by a wavetable
    // wavetables within an oscillator can be different lengths
    //
    // returns 0 upon success, or the number of wavetables if no more room is available
    //
    int addWaveTable(int len, float *waveTableIn, double topFreq) {
        if (mNumWaveTables < numWaveTableSlots) {
            float *waveTable = mWaveTables[mNumWaveTables].waveTable = new float[len + 1];
            mWaveTables[mNumWaveTables].waveTableLen = len;
            mWaveTables[mNumWaveTables].topFreq = topFreq;
            ++mNumWaveTables;

            // fill in wave
            for (long idx = 0; idx < len; idx++)
                waveTable[idx] = waveTableIn[idx];
            waveTable[len] = waveTable[0];  // duplicate for interpolation wraparound

            return 0;
        }
        return mNumWaveTables;
    }

protected:
    double mPhasor = 0.0;       // phase accumulator
    double mPhaseInc = 0.0;     // phase increment
    double mPhaseOfs = 0.5;     // phase offset for PWM

    // array of wavetables
    int mCurWaveTable = 0;      // current table, based on current frequency
    int mNumWaveTables = 0;     // number of wavetable slots in use
    struct waveTable {
        double topFreq;
        int waveTableLen;
        float *waveTable;
    };
    static constexpr int numWaveTableSlots = 40;    // simplify allocation with reasonable maximum
    waveTable mWaveTables[numWaveTableSlots];
};

#endif
