/*
See repo for licence detail
 */

#include "fft.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
void cfft(double* signal, const size_t size)
{
#ifdef FORCE_ACCQ
    std::cout << "fft.cpp is executing" << std::endl;
    cfft_plan plan = make_cfft_plan (size);
    cfft_forward(plan, signal, 1.);
    destroy_cfft_plan (plan);
#endif
    return;
}

void cifft(double* signal, const size_t size)
{
#ifdef FORCE_ACCQ
    cfft_plan plan = make_cfft_plan (size);
    cfft_backward(plan, signal, 1./size);
    destroy_cfft_plan (plan);
#endif
    return;
}

void rfft(double* signal, const size_t size)
{
#ifdef FORCE_ACCQ
    rfft_plan plan = make_rfft_plan (size);
    rfft_forward(plan, signal, 1.);
    destroy_rfft_plan (plan);
#endif

    return;
}

void rifft(double* signal, const size_t size)
{
#ifdef FORCE_ACCQ
    rfft_plan plan = make_rfft_plan (size);
    rfft_backward(plan, signal, 1./size);
    destroy_rfft_plan (plan);
#endif

    return;
}
