/*
See repo for licence detail
 */

#ifndef ACQUISITION_H
#define ACQUISITION_H
#include <gmp.h>
#define __gmp_const const

#include <iostream>
#include <complex>
#include <cmath>

#include "constants.h"
#include "fft.h"
#include "utils.h"


using namespace std;

void TwoCorrelationPeakComparison(float*, size_t, int, int, int*, float*);
int  FindMaxIndex(float*, size_t);
int  FindMaxIndexWithExclude(float*, size_t, int, int);
void PCPS(double*, size_t, int*, size_t, double, int, int, double, double, int, int, float*);




#endif


