/*
See repo for licence detail
 */
#include "utils.h"

void upsampleCode(int* code, size_t size, float codeFrequency, float samplingFrequency, double* r_code){

    size_t sizeUpCode = round(samplingFrequency * size / codeFrequency);

    for(int i=0; i < sizeUpCode; i++){
        r_code[i] = code[(int) trunc(i * codeFrequency / samplingFrequency)];
    }

    return;
}
