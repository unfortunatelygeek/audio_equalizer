#include "arm_math.h"
#include <peaking_filter.h>

# define M_PI           3.14159265358979323846

void peaking_filter_init (IFX_peaking_filter *filt, float sampleRate_hz) {
    
    /*Find Sample Time in Seconds*/
    filt -> sampleTime_s = 1.0f / sampleRate_hz;

    /*Clear Filter Memory*/
    for(uint8_t n = 0; n < 3; n++) {

        filt -> x[n] = 0.0f;
        filt -> y[n] = 0.0f;
    }

    peaking_filter_setParams(filt, 1.0f, 0.0f, 1.0f);

}

void peaking_filter_setParams (IFX_peaking_filter *filt, float centerFreq_hz, float bw_hz, float gain_lin) {


    /*Convert to rad/s, then do the pre-warp for the */
    float wcT = 2.0f * tanf( M_PI * centerFreq_hz * (filt -> sampleTime_s));

    float Q = centerFreq_hz / bw_hz;

    filt -> a[0] = 4.0f + 2.0f * (gain_lin / Q) * wcT + wcT * wcT;
    filt -> a[1] = 2.0f * wcT * wcT - 8.0f;
    filt -> a[2] = 4.0f - 2.0f * (gain_lin / Q) * wcT + wcT * wcT;

    filt -> b[0] = 1.0f / (4.0f + 2.0f / Q * wcT + wcT * wcT); 
    filt -> b[1] = -(2.0f * wcT * wcT - 8.0f);
    filt -> b[2] = -(4.0f - 2.0f / Q * wcT + wcT * wcT);    

}

float peaking_filter_update(IFX_peaking_filter *filt, float in) {

    filt -> x[2] = filt -> x[1];
    filt -> x[1] = filt -> x[0];
    filt -> x[0] = in;

    filt -> y[2] = filt -> y[1];
    filt -> y[1] = filt -> y[0];

    /* Compute new filter output */
    filt->y[0] = (filt->a[0] * filt->x[0] + filt->a[1] * filt->x[1] + filt->a[2] * filt->x[2] + (filt->b[1] * filt->y[1] + filt->b[2] * filt->y[2])) * filt->b[0];

    /* Return current output sample */
    return (filt->y[0]);

}