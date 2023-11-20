/*
See repo for licence detail
 */

#include "tracking.h"

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
         double* r_correlatorsResults){


    // Init correlators
    double spacings[] = {-correlatorsSpacing, 0.0, correlatorsSpacing};
    for(int j=0; j < 3; j++){
        r_correlatorsResults[j*2]   = 0.0;
        r_correlatorsResults[j*2+1] = 0.0;
    }

    double phase = 0.0;
    double real_part = 0.0;
    double imag_part = 0.0;
    double iReplica = 0.0;
    double qReplica = 0.0;
    // Perform correlation ///////////////// hi
    EPL_label0:for(int i=0; i < nbSamples; i++)
    {
        phase = -(carrierFrequency * 2.0 * PI * (i / samplingFrequency)) + remainingCarrier;
        real_part = std::cos(phase);
        imag_part = std::sin(phase);
        std::complex<double> replica(real_part, imag_part);


        // Mix carrier /////////////
        replica *= double(rfdata[  2*i]) + 1i * double(rfdata[  2*i+1]);
         iReplica = real(replica);
         qReplica = imag(replica);

        // Mix with code
        for(int j=0; j < 3; j++){
#pragma HLS pipeline ii=2
            int idx = (int) ceil(remainingCode + spacings[j] + i*codeStep);
            r_correlatorsResults[j*2]   += code[idx] * iReplica;
            r_correlatorsResults[j*2+1] += code[idx] * qReplica;
        }
    }
    
    return;
}

// ====================================================================================================================
// LOCK LOOPS

double DLL_NNEML(double iEarly, double qEarly, double iLate, double qLate){

    double earlySqare = sqrt(iEarly * iEarly + qEarly * qEarly);
    double lateSquare = sqrt(iLate * iLate + qLate * qLate);
    double codeError = (earlySqare - lateSquare) / (earlySqare + lateSquare);

    return codeError;
}

// --------------------------------------------------------------------------------------------------------------------

double PLL_costa(double iPrompt, double qPrompt){

    double phaseError = atan(qPrompt / iPrompt) / TWO_PI;

    return phaseError;
}

// ====================================================================================================================
// FILTERS

double LoopFilterTau1(double loopNoiseBandwidth, double dampingRatio, double loopGain){
    double Wn = loopNoiseBandwidth * 8.0 * dampingRatio / (4.0 * dampingRatio*dampingRatio + 1);
    double tau1 = loopGain / (Wn*Wn);
    return tau1;
}

// --------------------------------------------------------------------------------------------------------------------

double LoopFilterTau2(double loopNoiseBandwidth, double dampingRatio, double loopGain){
    double Wn = loopNoiseBandwidth * 8.0 * dampingRatio / (4.0 * dampingRatio*dampingRatio + 1);
    double tau2 = 2.0 * dampingRatio / Wn;
    return tau2;
}

// --------------------------------------------------------------------------------------------------------------------

double BorreLoopFilter(double input, double memory, double tau1, double tau2, double pdi){

    double output = tau2 / tau1 * (input - memory) + pdi / tau1 * input;

    return output;
}

// ====================================================================================================================
// INDICATORS

double PLLIndicator(double iprompt, double qprompt, double previous, double alpha){

    // Narrow Band Difference
    double nbd = iprompt*iprompt - qprompt*qprompt;

    // Narrow Band Power
    double nbp = iprompt*iprompt + qprompt*qprompt;
    
    double pllLock = nbd / nbp;

    // Pass through low-pass filter
    pllLock = (1 - alpha) * previous + alpha * pllLock;

    return pllLock;
}

// --------------------------------------------------------------------------------------------------------------------

double CN0_Baulieu(double pdpnRatio, double previous, double nbSamples, double alpha){
    
    double lambda_c = 1 / (pdpnRatio / nbSamples);

    double cn0 = lambda_c / (nbSamples * 1e-3); // Divide by number of milliseconds
    cn0 = (1 - alpha) * previous + alpha * cn0;

    return cn0;
}
