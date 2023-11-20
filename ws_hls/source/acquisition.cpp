/*
See repo for licence detail
 */

#include <gmp.h>
#define __gmp_const const

#include "acquisition.h"
#include "hls_math.h"
#include "constants.h"
using namespace std;



// ====================================================================================================================

void TwoCorrelationPeakComparison(float* correlationMap, size_t sizeMap, int samplesPerCode, int samplesPerCodeChip,
                                  int* r_idxPeak, float* r_acqMetric) {

    // Find largest value
    int idxPeak1 = FindMaxIndex(correlationMap, sizeMap);

    // Find second largest in the same frequency
    int startIdx = (int) idxPeak1 - (idxPeak1 % samplesPerCode);
    int idxPeak2 = FindMaxIndexWithExclude((float*)(correlationMap + startIdx), samplesPerCode,
                                           idxPeak1 % samplesPerCode, samplesPerCodeChip);
    idxPeak2 += startIdx;
    *r_acqMetric = correlationMap[idxPeak1] / correlationMap[idxPeak2];
    *r_idxPeak = idxPeak1;

    return;
}

// ====================================================================================================================

int FindMaxIndex(float* array, size_t size){
    int idxMax = 0;
    for(int i=0; i < size; i++){
        if(array[idxMax] < array[i]){
            idxMax = i;
        }
    }

    return idxMax;
}

// ====================================================================================================================

int FindMaxIndexWithExclude(float* array, size_t size, int excludeIdx, int excludeChipSize){
    int idxMax = 0;
    if (excludeIdx == 0){
        idxMax = 1;
    }
    for(int i=0; i < size; i++){
        if (abs(excludeIdx - i) < excludeChipSize){
            continue;
        }
        if(array[idxMax] < array[i]){
            idxMax = i;
        }
    }

    return idxMax;
}

// ====================================================================================================================

void PCPS(double* rfdata,
          size_t sizeData,
          int* code,
          size_t sizeCode,
          double codeFrequency,
          int cohIntegration,
          int nonCohIntegration,
          double samplingFrequency,
          double interFrequency,
          int dopplerRange,
          int dopplerStep,
          float* r_correlationMap)
{
    // Initialise some variables
    int samplesPerCode = samplingFrequency * sizeCode / codeFrequency;
    const int  const_samplesPerCode  = SAMPLE_FREQ*GPS_L1CA_CODE_SIZE_BITS/GPS_L1CA_CODE_FREQ;

    double iqSignal[const_samplesPerCode * 2];
    double nonCohSum[const_samplesPerCode];
    complex<double> cohSum[const_samplesPerCode];

    int dopplerBins = dopplerRange * 2 / dopplerStep + 1;

    // Prepare code FFT
    double codeUpsampled[const_samplesPerCode];
    complex<double> codeFFT[const_samplesPerCode];
    upsampleCode(code, GPS_L1CA_CODE_SIZE_BITS, GPS_L1CA_CODE_FREQ, samplingFrequency, codeUpsampled); // Upsampling code to sampling frequency
    rfft(codeUpsampled, const_samplesPerCode); // in-place FFT

    // Need to reconstruct the real-to-complex FFT, as pocket FFT only compute the real part
    // For this, we mirror the FFT coefficients with their conjugate. See FFT theory for more details.
    // We also apply another conjugate here for mixing later.
    // Note: in case the number of samples is odd, there might be an issue with this...
    int j = 1;
    for (size_t i=1; i < const_samplesPerCode/2; i++){
        complex<double> _complex = codeUpsampled[j] + 1i * codeUpsampled[j+1];
        codeFFT[i] = conj(_complex);
        codeFFT[const_samplesPerCode-i] = _complex;
        j += 2;
    }
    codeFFT[0] = codeUpsampled[0];
    codeFFT[const_samplesPerCode/2] = codeUpsampled[const_samplesPerCode-1];

    double freq = -dopplerRange;
    for (size_t idxDoppler=0; idxDoppler < dopplerBins; idxDoppler++)
    {
        freq -= interFrequency;

        // Non-coherent integration loop
        for (size_t j=0; j < const_samplesPerCode; j++){
            nonCohSum[j] = 0; // Reset non-coherent integration sum
        }
        for (size_t nonCohIdx=0; nonCohIdx < nonCohIntegration; nonCohIdx++)
        {
            // Coherent integration loop
            for (size_t j=0; j < const_samplesPerCode; j++)
            {
                cohSum[j] = 0;  // Reset coherent integration sum
            }
            for (size_t cohIdx=0; cohIdx < cohIntegration; cohIdx++)
            {

                for (size_t j=0; j < const_samplesPerCode; j++){

                    // Generate carrier
                    double phasePoints = ((cohIdx * const_samplesPerCode + j) << 1) * PI / samplingFrequency;
                    double angle = -freq * phasePoints;

                    // Using Euler's formula to compute the complex exponential
                    double realPart = cos(angle);
                    double imagPart = sin(angle);
                    std::complex<double> signalCarrier(realPart, imagPart);

                    // Mix
                    int _idx = j + (nonCohIdx * cohIntegration + cohIdx) * const_samplesPerCode;
                    std::complex<double> _complex = signalCarrier * std::complex<double>(rfdata[2*_idx], rfdata[2*_idx+1]);
                    iqSignal[2*j] = real(_complex);
                    iqSignal[2*j+1] = imag(_complex);
                }

                // Perform FFT (in-place)
                cfft(iqSignal, const_samplesPerCode);

                // Correlate with C/A code (in-place)
                for (size_t j=0; j < const_samplesPerCode; j++){
                    complex<double> _complex = iqSignal[2*j] + 1i * iqSignal[2*j+1];
                    _complex *= codeFFT[j];
                    iqSignal[2*j] = real(_complex);
                    iqSignal[2*j+1] = imag(_complex);
                }

                // Perform IFFT
                cifft(iqSignal, const_samplesPerCode);

                // Sum coherent integration
                for (size_t j=0; j < const_samplesPerCode; j++)
                    cohSum[j] += ((complex<double>*)iqSignal)[j];
            }
            for (size_t j = 0; j < const_samplesPerCode; j++) {
                double magnitude = sqrt(cohSum[j].real() * cohSum[j].real() + cohSum[j].imag() * cohSum[j].imag());
                nonCohSum[j] += magnitude;
            }
        }
        for (size_t j=0; j < const_samplesPerCode; j++)
            r_correlationMap[idxDoppler * const_samplesPerCode + j] = nonCohSum[j];

        // Increment frequency search step
        freq += dopplerStep;
    }

    return;
}
