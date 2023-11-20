/*
See repo for licence detail
 */

#ifndef FFH_H
#define FFH_H

#include "pocketfft.h"

void cfft(double*, const size_t);
void cifft(double*, const size_t);
void rfft(double*, const size_t);
void rifft(double*, const size_t);

#endif
