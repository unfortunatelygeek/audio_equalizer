#include <stdint.h>
#include <arm_math.h>

typedef struct
{
    uint8_t numStages;              /**number of 2nd order stages in the filter.  Overall order is 2*numStages. */
    float32_t *pState;              /**points to the array of state coefficients.  The array is of length 2*numStages. */
    const float32_t *pCoeffs;       /**points to the array of coefficients.  The array is of length 5*numStages. */
} biquad_iir_d2ft_instance;


void biquad_iir_d2ft(
  const biquad_iir_d2ft_instance * S,
  const float32_t * pSrc,
        float32_t * pDst,
        uint32_t blockSize)
{
    const   float32_t *pIn = pSrc;                         /* Source pointer */
            float32_t *pOut = pDst;                        /* Destination pointer */
            float32_t *pState = S->pState;                 /* State pointer */
    const   float32_t *pCoeffs = S->pCoeffs;               /* Coefficient pointer */
            float32_t acc1;                                /* Accumulator */
            float32_t b0, b1, b2, a1, a2;                  /* Filter coefficients */
            float32_t Xn1;                                 /* Temporary input */
            float32_t d1, d2;                              /* State variables */
            uint32_t sample, stage = S->numStages;         /* Loop counters */
    
    do
    {
        b0 = pCoeffs[0];
        b1 = pCoeffs[1];
        b2 = pCoeffs[2];
        a1 = pCoeffs[3];
        a2 = pCoeffs[4];

        /* Reading the state values */
        d1 = pState[0];
        d2 = pState[1];

        pCoeffs += 5U;

        /* Initialize blkCnt with number of samples */
        sample = blockSize;

        while (sample > 0U) {
            Xn1 = *pIn++;

            acc1 = b0 * Xn1 + d1;

            d1 = b1 * Xn1 + d2;
            d1 += a1 * acc1;

            d2 = b2 * Xn1;
            d2 += a2 * acc1;

            *pOut++ = acc1;

            /* decrement loop counter */
            sample--;
      }

      /* Store the updated state variables back into the state array */
      pState[0] = d1;
      pState[1] = d2;

      pState += 2U;

      /* The current stage input is given as the output to the next stage */
      pIn = pDst;

      /* Reset the output working pointer */
      pOut = pDst;

      /* decrement loop counter */
      stage--;

   } 
   
   while (stage > 0U);

}