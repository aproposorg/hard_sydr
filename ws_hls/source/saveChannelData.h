/*
See repo for licence detail
 */

#ifndef C_SYDR_SAVECHANNELDATA_H
#define C_SYDR_SAVECHANNELDATA_H

#include "constants.h"
#include "channel.h"

#include <iostream>

struct ChannelData {
    float m_acqMetric;
    int m_indexPeak;
    bool m_isTrackingInitialised;
    int m_trackRequiredSamples;
    double m_iPromptSum;
    double m_qPromptSum;
    int m_nbPromptSum;
    double m_remainingCarrier;
    double m_remainingCode;
    double m_pllDiscrim;
    double m_dllDiscrim;
    double m_codeError;
    double m_phaseError;
    double m_dllTau1;
    double m_dllTau2;
    double m_dllPDI;
    double m_pllTau1;
    double m_pllTau2;
    double m_pllPDI;
    int m_codeCounter;
    double m_pllLock;
    double m_pdpnRatio; // For C/N0 estimation
    double m_cn0;

    int m_channelID; // save
    int m_satelliteID;  // save
    int m_channelState;  // save
    double m_results[RESULTS_SIZE];
    int m_code[GPS_L1CA_CODE_SIZE_BITS + 2]; // save
    int m_codeOffset;   // save
    double m_carrierFrequency;  // save
    double m_codeFrequency;  // save
    double m_correlatorsResults[NB_CORRELATORS * 2];  // save
};

void storeChannelData(Channel& channel_X, struct ChannelData* channeldata_X);
void loadChannelData(Channel& channel_X, const struct ChannelData* channeldata_X);
void oneTimeLoadChannelParameter(struct ChannelData* channeldata_X);




#endif //C_SYDR_SAVECHANNELDATA_H
