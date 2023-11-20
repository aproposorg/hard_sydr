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

#include <iostream>
#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "source1.h"
#include "float_calcu.h"
#include <stdio.h>

#include <fstream>
#include "channel.h"
#include "fft.h"
#include "constants.h"
 
using namespace std;

// Function to pack eight 8-bit integers into a single 64-bit integer
int64_t pack_to_64bit(const int8_t *bytes) {
    int64_t packed_data = 0;
    for (int i = 0; i < 8; ++i) {
        packed_data |= (static_cast<int64_t>(bytes[i]) & 0xFF) << (56 - i * 8);
    }
    return packed_data;
}

// Function to unpack a single 64-bit integer into eight 8-bit integers
void unpack_from_64bit_tb(int64_t packed_data, int8_t *bytes) {
    for (int i = 0; i < 8; ++i) {
        bytes[i] = (packed_data >> (56 - i * 8)) & 0xFF;
    }
}


void top_hls_hard_sydr(hls::stream<ap_axis<64, 2, 5, 6>> &IN_0,
              hls::stream<ap_axis<64, 2, 5, 6>> &OUT_0);

int main()
{
    cout << "Starting main" << endl;

    double out_tb[OUT_DEPTH] = {0};


    // Create channel
    ofstream myfile;
    myfile.open("/home/minifu/Documents/GitHub/priv_hls_sydr/hls_csydr/track.txt");

    string filepath = "/home/minifu/Downloads/Novatel_20211130_resampled_10MHz_8bit_IQ_gain25.bin";
    ifstream ifs(filepath, ios::binary | ios::in);

    int size1ms = SIZE1MS;
    int8_t rfdata_int8[size1ms];
    int8_t rfdata_cast_int8[size1ms*2]; // note the size 
    int8_t rfdata_int8_test[size1ms];
    int bufferIdx = 0;
    int nbMilliseconds = NBMILLISECONDS; // now smaller
     ap_axis<64, 2, 5, 6> tmp;

    for (int i = 0; i < nbMilliseconds; i++)
    {
        // Read 1 millisecond
        ifs.read(reinterpret_cast<char *>(rfdata_int8), size1ms * sizeof(int8_t));

      // Recast to array
        for (int idx = 0; idx < size1ms; idx++)
        {
            if (i > 1)
            {
                rfdata_cast_int8[idx] = rfdata_cast_int8[bufferIdx + idx]; // Move previous data
            }
            rfdata_cast_int8[bufferIdx + idx] = (int)rfdata_int8[idx]; // Copy new data
        }
        bufferIdx = size1ms;

        hls::stream<ap_axis<64, 2, 5, 6>> IN_0;
        hls::stream<ap_axis<64, 2, 5, 6>> OUT_0;


        for (int i = 0; i < size1ms * 2; i += 8) // Increment by 8 for 64-bit packing
        {
            int64_t packed_data = pack_to_64bit(&rfdata_cast_int8[i]);
            tmp.data = packed_data;
            tmp.keep = 1;
            tmp.strb = 1;
            tmp.user = 1;
            tmp.id = 0;
            tmp.dest = 1;
            tmp.last = (i >= size1ms * 2 - 8) ? 1 : 0; // Adjusted for 64-bit data
            IN_0.write(tmp);
        }


        top_hls_hard_sydr(IN_0, OUT_0);
        ap_axis<64, 2, 5, 6> tmp_out;

        for (int i = 0; i < OUT_DEPTH; ++i)
        {
            OUT_0.read(tmp_out);
            double out_value = tmp_out.data;
            out_tb[i] = out_value;
        }
        cout << "TB cout_tb[0] = " << out_tb[0] << endl;
        cout << "TB cout_tb[1] = " << out_tb[1] << endl;
        cout << "TB cout_tb[2] = " << out_tb[2] << endl;
        cout << "TB cout_tb[3] = " << out_tb[3] << endl;
        cout << "TB cout_tb[4] = " << out_tb[4] << endl;
        cout << "TB cout_tb[5] = " << out_tb[5] << endl;
        cout << "\n"<< endl;
    }
    ifs.close();
    myfile.close();
    return 0;
}
