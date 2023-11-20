/*
See repo for licence detail
 */
#ifndef TRACKING_H
#define TRACKING_H

#include <iostream>
#include <complex>
#include <cmath>
#include "constants.h"
#include "fft.h"
#include "utils.h"

using namespace std;

void EPL(int8_t rfdata[],
         int nbSamples,
         int* code,
         size_t sizeCode,
         int codeOffset,
         double samplingFrequency,
         double carrierFrequency,
         double remainingCarrier,
         double remainingCode,
         double codeStep,
         double correlatorsSpacing,
         double* r_correlatorsResults);

// Lock loops
double DLL_NNEML(double iEarly, double qEarly, double iLate, double qLate);
double PLL_costa(double iPrompt, double qPrompt);

// Discriminators
double LoopFilterTau1(double loopNoiseBandwidth, double dampingRatio, double loopGain);
double LoopFilterTau2(double loopNoiseBandwidth, double dampingRatio, double loopGain);
double BorreLoopFilter(double input, double memory, double tau1, double tau2, double pdi);

// Indicators
double PLLIndicator(double iprompt, double qprompt, double previous, double alpha);
double CN0_Baulieu(double pdpnRatio, double previous, double nbSamples, double alpha);
 

#endif