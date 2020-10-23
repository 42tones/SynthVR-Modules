//
//  WaveUtils.cpp
//
//  Test wavetable oscillator
//
//  Created by Nigel Redmon on 2/18/13
//  EarLevel Engineering: earlevel.com
//  Copyright 2013 Nigel Redmon
//
//  For a complete explanation of the wavetable oscillator and code,
//  read the series of articles by the author, starting here:
//  www.earlevel.com/main/2012/05/03/a-wavetable-oscillator—introduction/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own purposes, free or commercial.
//
//  1.01  njr  2016-01-03   changed "> minVal" to "< minVal" to fix optimization of number of tables
//  1.1   njr  2019-04-30   changed addWaveTable to addWaveTable to accomodate WaveTableOsc update
//                          added filleTables2, which allows selection of minimum and maximum frequencies
//

#include "WaveUtils.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <stdexcept>
#include <string>

void fft(int N, double *ar, double *ai);
float makeWaveTable(WaveTableOsc *osc, int len, double *ar, double *ai, double scale, double topFreq);

//
// fillTables:
//
// The main function of interest here; call this with a pointer to an new, empty oscillator,
// and the real and imaginary arrays and their length. The function fills the oscillator with
// all wavetables necessary for full-bandwidth operation, based on one table per octave,
// and returns the number of tables.
//
int fillTables(WaveTableOsc *osc, double *freqWaveRe, double *freqWaveIm, int numSamples) {
    int idx;
    
    // zero DC offset and Nyquist
    freqWaveRe[0] = freqWaveIm[0] = 0.0;
    freqWaveRe[numSamples >> 1] = freqWaveIm[numSamples >> 1] = 0.0;
    
    // determine maxHarmonic, the highest non-zero harmonic in the wave
    int maxHarmonic = numSamples >> 1;
    const double minVal = 0.000001; // -120 dB
    while ((fabs(freqWaveRe[maxHarmonic]) + fabs(freqWaveIm[maxHarmonic]) < minVal) && maxHarmonic) --maxHarmonic;

    // calculate topFreq for the initial wavetable
    // maximum non-aliasing playback rate is 1 / (2 * maxHarmonic), but we allow aliasing up to the
    // point where the aliased harmonic would meet the next octave table, which is an additional 1/3
    double topFreq = 2.0 / 3.0 / maxHarmonic;
    
    // for subsquent tables, double topFreq and remove upper half of harmonics
    double *ar = new double [numSamples];
    double *ai = new double [numSamples];
    double scale = 0.0;
    int numTables = 0;

    // TODO: This assumes there is something in the wavetable. 
    // Should it be reworked to support empty ones?
    while (maxHarmonic) {
        // fill the table in with the needed harmonics
        for (idx = 0; idx < numSamples; idx++)
            ar[idx] = ai[idx] = 0.0;
        for (idx = 1; idx <= maxHarmonic; idx++) {
            ar[idx] = freqWaveRe[idx];
            ai[idx] = freqWaveIm[idx];
            ar[numSamples - idx] = freqWaveRe[numSamples - idx];
            ai[numSamples - idx] = freqWaveIm[numSamples - idx];
        }
        
        // make the wavetable
        scale = makeWaveTable(osc, numSamples, ar, ai, scale, topFreq);
        numTables++;

        // prepare for next table
        topFreq *= 2;
        maxHarmonic >>= 1;
    }
    return numTables;
}


WaveTableOsc *createOscFromWaveTable(float *waveSamples, int tableLen) {
    WaveTableOsc *osc = new WaveTableOsc();
    setOscWaveTable(osc, waveSamples, tableLen);
    return osc;
}

void setOscWaveTable(WaveTableOsc *osc, float *waveSamples, int tableLen) {
    double *freqWaveRe = new double [tableLen];
    double *freqWaveIm = new double [tableLen];
    
    // take FFT
    bool nonZeroSampleEncountered = false;
    for (int i = 0; i < tableLen; i++) {
        freqWaveIm[i] = waveSamples[i];
        freqWaveRe[i] = 0.0;

        // Track that a non-zero sample has been encountered
        if (!nonZeroSampleEncountered)
            if (waveSamples[i] != 0.0f)
                nonZeroSampleEncountered = true;
    }

    // Ensure that at least one sample is not zero
    if (!nonZeroSampleEncountered)
        freqWaveIm[0] = 1.0;

    fft(tableLen, freqWaveRe, freqWaveIm);
    fillTables(osc, freqWaveRe, freqWaveIm, tableLen);
}


//
// if scale is 0, auto-scales
// returns scaling factor (0.0 if failure), and wavetable in ai array
//
float makeWaveTable(WaveTableOsc *osc, int len, double *ar, double *ai, double scale, double topFreq) {
    fft(len, ar, ai);
    
    if (scale == 0.0) {
        // calc normal
        double max = 0;
        for (int idx = 0; idx < len; idx++) {
            double temp = fabs(ai[idx]);
            if (max < temp)
                max = temp;
        }
        scale = 1.0 / max * .999;        
    }
    
    // normalize
    float *wave = new float [len];
    for (int idx = 0; idx < len; idx++)
        wave[idx] = ai[idx] * scale;
        
    if (osc->addWaveTable(len, wave, topFreq))
        scale = 0.0;
    
    return scale;
}

constexpr bool is_powerof2(int v) {
    return v && ((v & (v - 1)) == 0);
}

//
// fft
//
// I grabbed (and slightly modified) this Rabiner & Gold translation...
//
// (could modify for real data, could use a template version, blah blah--just keeping it short)
//
void fft(int N, double *ar, double *ai)
/*
 in-place complex fft
 
 After Cooley, Lewis, and Welch; from Rabiner & Gold (1975)
 
 program adapted from FORTRAN 
 by K. Steiglitz  (ken@princeton.edu)
 Computer Science Dept. 
 Princeton University 08544          */
{    
    int i, j, k, L;            /* indexes */
    int M, TEMP, LE, LE1, ip;  /* M = log N */
    int NV2, NM1;
    double t;               /* temp */
    double Ur, Ui, Wr, Wi, Tr, Ti;
    double Ur_old;
    
    // if ((N > 1) && !(N & (N - 1)))   // make sure we have a power of 2
    if (!is_powerof2(N))
        throw std::runtime_error(std::string("Time domain signal must be a power of 2!"));

    NV2 = N >> 1;
    NM1 = N - 1;
    TEMP = N; /* get M = log N */
    M = 0;
    while (TEMP >>= 1) ++M;
    
    /* shuffle */
    j = 1;
    for (i = 1; i <= NM1; i++) {
        if(i<j) {             /* swap a[i] and a[j] */
            t = ar[j-1];     
            ar[j-1] = ar[i-1];
            ar[i-1] = t;
            t = ai[j-1];
            ai[j-1] = ai[i-1];
            ai[i-1] = t;
        }
        
        k = NV2;             /* bit-reversed counter */
        while(k < j) {
            j -= k;
            k /= 2;
        }
        
        j += k;
    }
    
    LE = 1.;
    for (L = 1; L <= M; L++) {            // stage L
        LE1 = LE;                         // (LE1 = LE/2) 
        LE *= 2;                          // (LE = 2^L)
        Ur = 1.0;
        Ui = 0.; 
        Wr = cos(M_PI/(float)LE1);
        Wi = -sin(M_PI/(float)LE1); // Cooley, Lewis, and Welch have "+" here
        for (j = 1; j <= LE1; j++) {
            for (i = j; i <= N; i += LE) { // butterfly
                ip = i+LE1;
                Tr = ar[ip-1] * Ur - ai[ip-1] * Ui;
                Ti = ar[ip-1] * Ui + ai[ip-1] * Ur;
                ar[ip-1] = ar[i-1] - Tr;
                ai[ip-1] = ai[i-1] - Ti;
                ar[i-1]  = ar[i-1] + Tr;
                ai[i-1]  = ai[i-1] + Ti;
            }
            Ur_old = Ur;
            Ur = Ur_old * Wr - Ui * Wi;
            Ui = Ur_old * Wi + Ui * Wr;
        }
    }
}
