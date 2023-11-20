/*
See repo for licence detail
 */
#ifndef CHANNEL_H
#define CHANNEL_H
#include <gmp.h>
#define __gmp_const const

#include <iostream>
#include <ccomplex>
#include "gen_code.h"
#include "acquisition.h"
#include "tracking.h"
#include "structures.h"
#include <cstring>
#include "constants.h"
#define RESULTS_SIZE 32
#define NB_CORRELATORS 3
#define I_EARLY_IDX  0
#define Q_EARLY_IDX  1
#define I_PROMPT_IDX 2
#define Q_PROMPT_IDX 3
#define I_LATE_IDX   4
#define Q_LATE_IDX   5

/// ===========================================================================



using namespace std;

class Channel {

private:

    size_t m_rfdataSize;

    // Acquisition, save all
    float m_acqMetric;
    int m_indexPeak;

    // Tracking, save all
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

    // Indicators, save all
    double m_pllLock;
    double m_pdpnRatio; // For C/N0 estimation
    double m_cn0;

public:
    int m_channelID; // save
    int m_satelliteID;  // save
    int m_channelState;  // save
    double m_results[RESULTS_SIZE];
    int m_code[GPS_L1CA_CODE_SIZE_BITS + 2]; // save
    st_ChannelConfig m_config;  // remove the original pointer, store config inside the calss, rather than a pointer

    int m_codeOffset;   // save
    double m_carrierFrequency;  // save
    double m_codeFrequency;  // save

    double m_correlatorsResults[NB_CORRELATORS * 2];  // save

    Channel(int , const st_ChannelConfig& );

    // Destructor
    ~Channel();

    // General processing

    void run(int8_t rfdata[], size_t size);

    void processHandler();

    void setSatellite(int satelliteID);

    void resetChannel();

    void resetCounters();

    // Acquisition
    void runAcquisition(int8_t rfdata[]);

    void runSignalSearch(int8_t rfdata[], float *);

    void runPeakFinder(float *, size_t);

    void postAcquisitionUpdate();

    // Tracking
    void initTracking();

    void runTracking(int8_t rfdata[]);

    void runCorrelators(int8_t rfdata[]);

    void runDiscriminatorsFilters();

    void runLoopIndicators();

    void postTrackingUpdate();

    void trackingStateUpdate();

    void runFrequencyDiscriminator();

    void runPhaseDiscriminator();

    void runCodeDiscriminator();

    void prepareResultsTracking();

    // Decoding
    void runDecoding();

    void decodeBit();

    void decodeSubframe();

    void postDecodingUpdate();

    void prepareResultsDecoding();


    // to get all
    // Accessor function for m_acqMetric
    float getAcqMetric() const {
        return m_acqMetric;
    }

    // update m_acqMetric using setter function
    void setAcqMetric(float acqMetric) {
        m_acqMetric = acqMetric;
    }

    // Accessor function for m_indexPeak
    int getIndexPeak() const {
        return m_indexPeak;
    }

    // update m_indexPeak using setter function
    void setIndexPeak(int indexPeak) {
        m_indexPeak = indexPeak;
    }

    bool getIsTrackingInitialised() const {
        return m_isTrackingInitialised;
    }

    // update m_isTrackingInitialised using setter function
    void setIsTrackingInitialised(bool isTrackingInitialised) {
        m_isTrackingInitialised = isTrackingInitialised;
    }

    int getTrackRequiredSamples() const {
        return m_trackRequiredSamples;
    }

    // update m_trackRequiredSamples using setter function
    void setTrackRequiredSamples(int trackRequiredSamples) {
        m_trackRequiredSamples = trackRequiredSamples;
    }

    double getIPromptSum() const {
        return m_iPromptSum;
    }

    // update m_iPromptSum using setter function
    void setIPromptSum(double iPromptSum) {
        m_iPromptSum = iPromptSum;
    }

    double getQPromptSum() const {
        return m_qPromptSum;
    }

    // update m_qPromptSum using setter function
    void setQPromptSum(double qPromptSum) {
        m_qPromptSum = qPromptSum;
    }

    int getNbPromptSum() const {
        return m_nbPromptSum;
    }

    // update m_nbPromptSum using setter function
    void setNbPromptSum(int nbPromptSum) {
        m_nbPromptSum = nbPromptSum;
    }

    double getRemainingCarrier() const {
        return m_remainingCarrier;
    }

    // update m_remainingCarrier using setter function
    void setRemainingCarrier(double remainingCarrier) {
        m_remainingCarrier = remainingCarrier;
    }

    double getRemainingCode() const {
        return m_remainingCode;
    }

    // update m_remainingCode using setter function
    void setRemainingCode(double remainingCode) {
        m_remainingCode = remainingCode;
    }

    double getPllDiscrim() const {
        return m_pllDiscrim;
    }

    // update m_pllDiscrim using setter function
    void setPllDiscrim(double pllDiscrim) {
        m_pllDiscrim = pllDiscrim;
    }

    double getDllDiscrim() const {
        return m_dllDiscrim;
    }

    // update m_dllDiscrim using setter function
    void setDllDiscrim(double dllDiscrim) {
        m_dllDiscrim = dllDiscrim;
    }

    double getCodeError() const {
        return m_codeError;
    }

    // update m_codeError using setter function
    void setCodeError(double codeError) {
        m_codeError = codeError;
    }

    double getPhaseError() const {
        return m_phaseError;
    }

    // update m_phaseError using setter function
    void setPhaseError(double phaseError) {
        m_phaseError = phaseError;
    }

    double getDllTau1() const {
        return m_dllTau1;
    }

    // update m_dllTau1 using setter function
    void setDllTau1(double dllTau1) {
        m_dllTau1 = dllTau1;
    }


    double getDllTau2() const {
        return m_dllTau2;
    }

    // update m_dllTau2 using setter function
    void setDllTau2(double dllTau2) {
        m_dllTau2 = dllTau2;
    }

    double getDllPDI() const {
        return m_dllPDI;
    }

    // update m_dllPDI using setter function
    void setDllPDI(double dllPDI) {
        m_dllPDI = dllPDI;
    }

    double getPllTau1() const {
        return m_pllTau1;
    }

    // update m_pllTau1 using setter function
    void setPllTau1(double pllTau1) {
        m_pllTau1 = pllTau1;
    }

    double getPllTau2() const {
        return m_pllTau2;
    }

    // update m_pllTau2 using setter function
    void setPllTau2(double pllTau2) {
        m_pllTau2 = pllTau2;
    }

    double getPllPDI() const {
        return m_pllPDI;
    }

    // update m_pllPDI using setter function
    void setPllPDI(double pllPDI) {
        m_pllPDI = pllPDI;
    }

    int getCodeCounter() const {
        return m_codeCounter;
    }

    // update m_codeCounter using setter function
    void setCodeCounter(int codeCounter) {
        m_codeCounter = codeCounter;
    }

    // Getter for m_pllLock
    double getPllLock() const {
        return m_pllLock;
    }

    // Setter for m_pllLock
    void setPllLock(double pllLock) {
        m_pllLock = pllLock;
    }

    // Getter for m_pdpnRatio
    double getPdpnRatio() const {
        return m_pdpnRatio;
    }

    // Setter for m_pdpnRatio
    void setPdpnRatio(double pdpnRatio) {
        m_pdpnRatio = pdpnRatio;
    }

    // Getter for m_cn0
    double getCn0() const {
        return m_cn0;
    }

    // Setter for m_cn0
    void setCn0(double cn0) {
        m_cn0 = cn0;
    }


};

#endif

