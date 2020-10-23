//
//  WaveUtils.h
//
//  Created by Nigel Redmon on 2/18/13
//
//

#ifndef WaveUtils_h
#define WaveUtils_h

#include "WaveTableOsc.h"

int fillTables(WaveTableOsc *osc, double *freqWaveRe, double *freqWaveIm, int numSamples);
float makeWaveTable(WaveTableOsc *osc, int len, double *ar, double *ai, double scale, double topFreq);

WaveTableOsc * createOscFromWaveTable(float *waveSamples, int tableLen);
void setOscWaveTable(WaveTableOsc *osc, float*waveSamples, int tableLen);

#endif
