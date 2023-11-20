/*
 * APROPOS - Organization for the EU Horizon 2020 APROPOS project, grant no. 956090.
 * Copyright 2023 Antoine Grenier, Jie Lei, Hans Jakob Damsgaard
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gmp.h>
#define __gmp_const const

#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "hls_print.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <stdbool.h>
#include <assert.h>

#include <stdio.h>
#include <string.h>

#include "source1.h"
#include "float_calcu.h"
#include "channel.h"
#include "fft.h"

#include "saveChannelData.h"



int8_t rfdata[SIZE1MS * 2];
static int static_cnt = 0;


// Function to unpack a single 64-bit integer into eight 8-bit integers
void unpack_from_64bit(int64_t packed_data, int8_t *bytes) {
	#pragma HLS UNROLL
    for (int i = 0; i < 8; ++i) {
        bytes[i] = (packed_data >> (56 - i * 8)) & 0xFF;
    }
}


// top level function for HLS synthesis
void top_hls_hard_sydr(hls::stream<ap_axis<64, 2, 5, 6>> &IN_0,
             		 hls::stream<ap_axis<64, 2, 5, 6>> &OUT_0)
{

#pragma HLS INTERFACE axis port = IN_0
#pragma HLS INTERFACE axis port = OUT_0
#pragma hls interface s_axilite port = return

    // Csydr init
    /// =======================================================================
    // CONFIGURATION
    // Define signal parameters
    static st_SignalConfig signalConfig;
    signalConfig.codeFreqBasis = GPS_L1CA_CODE_FREQ;
    signalConfig.codeLengthBits = GPS_L1CA_CODE_SIZE_BITS;
    signalConfig.samplingFreq = SAMPLE_FREQ;
    signalConfig.intermediateFreq = INTERMEDIATEFREQ;

    static st_ChannelConfig channelConfig;
    //    channelConfig.signalConfig = &signalConfig; // original pointer to pointer
    channelConfig.signalConfig = signalConfig;

    // Acquisition
    channelConfig.dopplerRange = DOPPLERRANGE;
    channelConfig.dopplerStep = DOPPLERSTEP;
    channelConfig.acqThreshold = 1.0;
    channelConfig.cohIntegration = 1;
    channelConfig.nonCohIntegration = 1;

    // Tracking
    channelConfig.correlatorSpacing = 0.5;
    channelConfig.dllDampRatio = 0.7;
    channelConfig.dllNoiseBW = 1.0;
    channelConfig.dllLoopGain = 1.0;
    channelConfig.dllPDI = 1e-3;

    channelConfig.pllDampRatio = 0.7;
    channelConfig.pllNoiseBW = 15.0;
    channelConfig.pllLoopGain = 0.25;
    channelConfig.pllPDI = 1e-3;
    channelConfig.pllIndicatorAlpha = 0.01;

    channelConfig.cn0Alpha = 0.1;

    static ChannelData channeldata_0;
    /// =======================================================================
    // MAIN PROGRAM

    // Create channel
    int prn = 9;
    int channelID = 0;
    static Channel channel_0(channelID, channelConfig);
    channel_0.setSatellite(prn);


    ap_axis<64, 2, 5, 6> data_in, data_out;
  
      int count = 0;
    do {
        data_in = IN_0.read(); // Read 64-bit data

        int8_t bytes[8];
        unpack_from_64bit(data_in.data.to_int64(), bytes); // Unpack the 64-bit data into 8-bit

        // Store unpacked 8-bit data
        for (int i = 0; i < 8; ++i) {
            int index = count * 8 + i; // Calculate the index for rfdata
            if (index < HLS_IN_DEPTH) {
				rfdata[index] = (int)bytes[i];
            }
        }
        count++; // Increment count for the number of 64-bit blocks read

    } while (!data_in.last && count < (HLS_IN_DEPTH / 8));

	        if (static_cnt == 0) {
	            // Do 
	            // Nothing
	        } else if (static_cnt == 1){
	            oneTimeLoadChannelParameter(&channeldata_0);
	            loadChannelData(channel_0, &channeldata_0);
	            channel_0.run(rfdata, SIZE1MS);
	            storeChannelData(channel_0, &channeldata_0);
	        }
	        else {
	            loadChannelData(channel_0, &channeldata_0);
	            channel_0.run(rfdata, SIZE1MS);
	            storeChannelData(channel_0, &channeldata_0);
	        } 


    // Prepare the data to be sent out
	for (int i = 0; i < 6; i++){
		data_out.data = (int) channel_0.m_correlatorsResults[i];
    	data_out.keep = -1; // Optional: setting all 'keep' bits to 1
		data_out.strb = -1; // Optional: setting all 'strb' bits to 1
		data_out.user = data_in.user; // Optional: Just as an example, re-using 'user' from last input
		data_out.id = 0;   // Optional: setting ID to 0
    	data_out.last = (i == 5) ? 1 : 0;
		// Write the summed data to the output stream
		OUT_0.write(data_out);
	}

    static_cnt++;
}
