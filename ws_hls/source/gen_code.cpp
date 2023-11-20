/**
 * @file gen_code.cpp
 *
 * @brief code generation source file for SDR functions in C++
 *
 * Project Title: GNSS-R SDR
 * Author: John Bagshaw
 * Co-author: Surabhi Guruprasad
 * Contact: jotshaw@yorku.ca
 * Supervisor: Prof. Sunil Bisnath
 * Project Manager: Junchan Lee
 * Institution: Lassonde School of Engineering, York University, Canada.
 **/

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

static const short s_g2Shifts[33] = { 0, 5, 6, 7, 8, 17, 18, 139, 140, 141, 251, 252,
254, 255, 256, 257, 258, 469, 470, 471, 472, 473, 474, 509, 512, 513, 514,
515, 516, 859, 860, 861, 862};

// ----------------------------------------------------------------------------

void generateCAcode(int PRN, int* caCode)
{
    int reg1[10] = { 1,1,1,1,1,1,1,1,1,1 };
    int reg2[10] = { 1,1,1,1,1,1,1,1,1,1 };
    int nx;
    int saveBit;
    double samplesPerCode;

    // int *g1  = new int [1023];
    // int *g2  = new int [1023];
    // int* g2b = new int [1023];

    int g1[1023];
    int g2[1023];
    int g2b[1023];


    /* --- Generate G1 code ----------------------------------------------------- */
    /* --- Generate all G1 signal chips based on the G1 feedback polynomial ----- */
    for (nx = 0; nx < 1023; nx++) {
        g1[nx] = reg1[9];
        saveBit = reg1[9]^reg1[2];
        for (size_t i = 9; i >= 1; i--)
        {
            reg1[i] = reg1[i - 1];
        }
        reg1[0] = saveBit;
    }

    /* --- Generate G2 code ----------------------------------------------------- */
    /* --- Initialize g2 output to speed up the function --- */
    /* --- Generate all G2 signal chips based on the G2 feedback polynomial ----- */
    for (nx = 0; nx < 1023; nx++) {
        g2[nx] = reg2[9];
        saveBit = reg2[9];
        saveBit ^= reg2[8];
        saveBit ^= reg2[7];
        saveBit ^= reg2[5];
        saveBit ^= reg2[2];
        saveBit ^= reg2[1];
        for (size_t i = 9; i >= 1; i--)
        {
            reg2[i] = reg2[i - 1];
        }
        reg2[0] = saveBit;
    }

    /* --- Shift G2 code -------------------------------------------------------- */
    /* The idea: g2 = concatenate[ g2_right_part, g2_left_part ]; */
    /* --- Form single sample C/A code by multiplying G1 and G2 ----------------- */
    int g2shift = s_g2Shifts[PRN];
    if (g2shift)
    {
        int idx = 0;
        for (nx = 1023 - g2shift; nx < 1023; nx++)
        {
            g2b[idx] = g2[nx];
            idx++;
        }
        for (nx = 0; nx < 1023-g2shift; nx++)
        {
            g2b[idx] = g2[nx];
            idx++;
        }
    }

    for (nx = 0; nx < 1023; nx++) {
        caCode[nx] = g1[nx] == g2b[nx] ? -1 : 1;
    }

}
