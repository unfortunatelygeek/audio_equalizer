#define IIR_PEAKING_FILTER_H

#include <math.h>
#include "main.h"
#include <stdint.h>

typedef struct {

    float sampleTime_s;

    /*Filter input x[0] = current input sample*/
    float x[3];

    /*Filter ouput y[0] = current sample output*/
    float y[3];


    /*x[n] coeff*/
    float a[3];

    /*y[n] coeff*/
    float b[3];
} IFX_peaking_filter;


void peaking_filter_init(IFX_peaking_filter *filt, float sampleRate_Hz);
void peaking_filter_setParams(IFX_peaking_filter *filt, float centerFreq_hz, float bw_hz, float gain_lin);
float peaking_filter_update(IFX_peaking_filter *filt, float in);

