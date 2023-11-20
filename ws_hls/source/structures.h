/*
See repo for licence detail
 */
#ifndef STRUCTURES_H
#define STRUCTURES_H

/// ===========================================================================

typedef enum {
    OFF,
    IDLE,
    ACQUIRING,
    TRACKING
} ChannelState;

// ----------------------------------------------------------------------------

typedef enum {
    NONE,
    CHANNEL_UPDATE,
    ACQUISITION_UPDATE,
    TRACKING_UPDATE,
    DECODING_UPDATE
} ChannelMessage;

/// ===========================================================================

struct st_SignalConfig{
    //RF 
    double samplingFreq;
    double intermediateFreq;

    // GNSS
    double codeFreqBasis;
    int codeLengthBits;
};

/// ===========================================================================

struct st_ChannelConfig {

    // General
//    st_SignalConfig* signalConfig; // RF and GNSS signal parameters
    st_SignalConfig signalConfig; // direct embed the structures
    int bufferSize;

    // Acquisition
    int dopplerRange;
    int dopplerStep;
    int cohIntegration;   // Coherent integration
    int nonCohIntegration; // Non coherent integration
    float acqThreshold;

    // Tracking
    float correlatorSpacing;
    float dllNoiseBW;
    float dllDampRatio;
    float dllLoopGain;
    float dllPDI;
    float pllNoiseBW;
    float pllDampRatio;
    float pllLoopGain;
    float pllPDI;

    // Inidicators
    float pllIndicatorAlpha;  // Low-pass smoother coefficient
    float cn0Alpha;

};

/// ===========================================================================

#endif
