#include "main.h"
#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include "arm_math.h"
#include "input_signal.h"

#define VOLFIR 60
#define VOLIIR 74

#define AUDIO_BLOCK_SIZE   ((uint32_t)32)
#define SIGNAL_SAMPLES		AUDIO_BLOCK_SIZE/2


#define AUDIO_BUFFER_IN    AUDIO_REC_START_ADDR     /* In SDRAM */
#define AUDIO_BUFFER_OUT   (AUDIO_REC_START_ADDR + (AUDIO_BLOCK_SIZE * 2)) /* In SDRAM */
#define float_buffer_in    (AUDIO_REC_START_ADDR + (AUDIO_BLOCK_SIZE * 4))
#define float_buffer_out   (AUDIO_REC_START_ADDR + (AUDIO_BLOCK_SIZE * 6))
#define NUM_TAPS_FIR			((uint16_t)232)
#define numStages_IIR 6
#define NUM_TAPS_IIR 5*numStages_IIR

float32_t pStateIIR [2*numStages_IIR];
arm_biquad_cascade_df2T_instance_f32 S;

unsigned char logmsgbuff[128];
char buf[BUFSIZE];

arm_fir_instance_q15 FIRstructINT, FIRstructINT2, FIRstructINT3, FIRstructINT4,FIRstructINT5 ;

q15_t pFIRStateINT[NUM_TAPS_FIR + AUDIO_BLOCK_SIZE -1];
q15_t pFIRStateINT2[NUM_TAPS_FIR + AUDIO_BLOCK_SIZE -1];
q15_t pFIRStateINT3[NUM_TAPS_FIR + AUDIO_BLOCK_SIZE -1];
q15_t pFIRStateINT4[NUM_TAPS_FIR + AUDIO_BLOCK_SIZE -1];
q15_t pFIRStateINT5[NUM_TAPS_FIR + AUDIO_BLOCK_SIZE -1];

const char * logmsg(text,value, base)
{
	logmsgbuff[0]='\0';
	char locmessage[128];
	strcat(logmsgbuff,text);
	itoa (value,locmessage,base);
    strcat(logmsgbuff,locmessage);
    return logmsgbuff;
}

void filterinit (void)
{
	  AudioLoopback_SetHint();

	  /* Initialize Audio RecINPUT_DEVICE_INPUT_LINE order INPUT_DEVICE_INPUT_LINE_1 */
	  if (BSP_AUDIO_IN_OUT_Init(INPUT_DEVICE_INPUT_LINE_1, OUTPUT_DEVICE_HEADPHONE, DEFAULT_AUDIO_IN_FREQ, DEFAULT_AUDIO_IN_BIT_RESOLUTION, DEFAULT_AUDIO_IN_CHANNEL_NBR) == AUDIO_OK)
	  {
	   BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	   BSP_AUDIO_IN_SetVolume(VOLFIR);
	   BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 235,  (uint8_t *)logmsg("Audio buffer size ",AUDIO_BLOCK_SIZE,10), LEFT_MODE);
	   BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 225, (uint8_t *)"Audio record Init OK", LEFT_MODE);
	  }
	  else
	  {
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
	    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 95, (uint8_t *)" AUDIO RECORD INIT FAILED", CENTER_MODE);
	    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 65, (uint8_t *)" Please reset the board ", CENTER_MODE);
	  }

	  /* Display the state on the screen */
	  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 215, (uint8_t *)"Line-In --> Line-Out", LEFT_MODE);

	  /* Initialize SDRAM buffers */
	  memset((uint16_t*)AUDIO_BUFFER_IN, 0, AUDIO_BLOCK_SIZE*2);
	  memset((uint16_t*)AUDIO_BUFFER_OUT, 0, AUDIO_BLOCK_SIZE*2);
	  audio_rec_buffer_state = BUFFER_OFFSET_NONE;

	  /* Start Recording */
	  // Number of elements not size,
	  BSP_AUDIO_IN_Record((uint16_t*)AUDIO_BUFFER_IN, AUDIO_BLOCK_SIZE);

	  /* Start Playback */
	  // Data in bytes, left and right elements
	  BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
	  BSP_AUDIO_OUT_Play((uint16_t*)AUDIO_BUFFER_OUT, AUDIO_BLOCK_SIZE * 2);

}

void filter (int ftype)
{
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_FillRect(0,70,300,200);
  if (ftype==0)
  {
	  BSP_LCD_SetTextColor(LCD_COLOR_RED);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 200, (uint8_t *)"Filtering OFF", LEFT_MODE);
	  BSP_AUDIO_IN_SetVolume(VOLFIR);
  }
  else if (ftype==1)
  {
	  arm_fir_init_q15(&FIRstructINT, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT, SIGNAL_SAMPLES);
	  arm_fir_init_q15(&FIRstructINT2, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT2, SIGNAL_SAMPLES);
	  arm_fir_init_q15(&FIRstructINT3, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT3, SIGNAL_SAMPLES);
	  arm_fir_init_q15(&FIRstructINT4, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT4, SIGNAL_SAMPLES);
	  arm_fir_init_q15(&FIRstructINT5, NUM_TAPS_FIR, firCoeffINT_BP, pFIRStateINT5, SIGNAL_SAMPLES);

	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 200, (uint8_t *)"FIR filtering is ON, BandPass x5", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 190, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_FIR,10), LEFT_MODE);
	  BSP_AUDIO_IN_SetVolume(VOLFIR);

  }
  else if (ftype==2)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Papoulis, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 200, (uint8_t *)"IIR type: Papoulis BW 150Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 190, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 180, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
	  BSP_AUDIO_IN_SetVolume(VOLIIR);
  }

  else if (ftype==3)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Bessel, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 200, (uint8_t *)"IIR type: Bessel BW 150Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 190, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 180, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
	  BSP_AUDIO_IN_SetVolume(VOLIIR);
  }

  else if (ftype==4)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Chebyshev, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 200, (uint8_t *)"IIR type: Chebyshev BW 150Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 190, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 180, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
	  BSP_AUDIO_IN_SetVolume(VOLIIR);
  }

  else if (ftype==5)
   {
 	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Butterworth, pStateIIR);
 	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
 	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 200, (uint8_t *)"IIR type: Butterworth BW 150Hz", LEFT_MODE);
 	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 190, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
 	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 180, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
	  BSP_AUDIO_IN_SetVolume(VOLIIR);
   }


  else if (ftype==6)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Gauss, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 200, (uint8_t *)"IIR type: Gauss BW 150Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 190, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 180, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
	  BSP_AUDIO_IN_SetVolume(VOLIIR);
  }

  else if (ftype==7)
  {
	  arm_biquad_cascade_df2T_init_f32(&S, numStages_IIR, pCoeffs_Gauss_narrow, pStateIIR);
	  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 200, (uint8_t *)"IIR type: Gauss BW 100Hz", LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 190, (uint8_t *)logmsg("Order ",2*numStages_IIR,10), LEFT_MODE);
	  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 180, (uint8_t *)logmsg("Coeff taps ",NUM_TAPS_IIR,10), LEFT_MODE);
	  BSP_AUDIO_IN_SetVolume(VOLIIR);
  }

  while (1)
  {
    /* Wait end of half block recording */
    while(audio_rec_buffer_state != BUFFER_OFFSET_HALF)
    {
    BSP_PB_GetState(BUTTON_KEY);
    }

    audio_rec_buffer_state = BUFFER_OFFSET_NONE;

    /* Copy recorded 1st half block */
    if (ftype==0){
    	// No DSP
       	memcpy((uint16_t *)(AUDIO_BUFFER_OUT),(uint16_t *)(AUDIO_BUFFER_IN),AUDIO_BLOCK_SIZE);
    }
    else if (ftype==1){
    	// DSP FIR
        arm_fir_fast_q15(&FIRstructINT, (q15_t *)AUDIO_BUFFER_IN, (q15_t *)AUDIO_BUFFER_OUT, SIGNAL_SAMPLES);
        arm_fir_fast_q15(&FIRstructINT2, (q15_t *)AUDIO_BUFFER_OUT, (q15_t *)AUDIO_BUFFER_IN, SIGNAL_SAMPLES);
        arm_fir_fast_q15(&FIRstructINT3, (q15_t *)AUDIO_BUFFER_IN, (q15_t *)AUDIO_BUFFER_OUT, SIGNAL_SAMPLES);
        }
    else if (ftype>=2){
    	// DSP IIR
    	arm_q15_to_float (AUDIO_BUFFER_IN, float_buffer_in, SIGNAL_SAMPLES);
    	arm_biquad_cascade_df2T_f32(&S, float_buffer_in, float_buffer_out, SIGNAL_SAMPLES);
    	arm_float_to_q15 (float_buffer_out, AUDIO_BUFFER_OUT, SIGNAL_SAMPLES);
    	}

    /* Wait end of one block recording */
    while(audio_rec_buffer_state != BUFFER_OFFSET_FULL)
    {
    	BSP_PB_GetState(BUTTON_KEY);
    }
    audio_rec_buffer_state = BUFFER_OFFSET_NONE;

    /* Copy recorded 2nd half block */
    if (ftype==0){
    	memcpy((uint16_t *)((AUDIO_BUFFER_OUT) + (AUDIO_BLOCK_SIZE)),(uint16_t *)((AUDIO_BUFFER_IN) + (AUDIO_BLOCK_SIZE)),AUDIO_BLOCK_SIZE);
    }
    else if (ftype==1){
    	//DSP FIR
        arm_fir_fast_q15(&FIRstructINT, (q15_t *)(AUDIO_BUFFER_IN + (AUDIO_BLOCK_SIZE)), (q15_t *)(AUDIO_BUFFER_OUT + (AUDIO_BLOCK_SIZE)), SIGNAL_SAMPLES);
        arm_fir_fast_q15(&FIRstructINT2, (q15_t *)(AUDIO_BUFFER_OUT + (AUDIO_BLOCK_SIZE)), (q15_t *)(AUDIO_BUFFER_IN + (AUDIO_BLOCK_SIZE)), SIGNAL_SAMPLES);
        arm_fir_fast_q15(&FIRstructINT3, (q15_t *)(AUDIO_BUFFER_IN + (AUDIO_BLOCK_SIZE)), (q15_t *)(AUDIO_BUFFER_OUT + (AUDIO_BLOCK_SIZE)), SIGNAL_SAMPLES);
    }
    else if (ftype>=2){
    	// DSP IIR
    	arm_q15_to_float((AUDIO_BUFFER_IN + (AUDIO_BLOCK_SIZE)), float_buffer_in, SIGNAL_SAMPLES);
    	arm_biquad_cascade_df2T_f32(&S, float_buffer_in, float_buffer_out, SIGNAL_SAMPLES);
    	arm_float_to_q15(float_buffer_out, AUDIO_BUFFER_OUT + (AUDIO_BLOCK_SIZE), SIGNAL_SAMPLES);
    }

    if (CheckForUserInputTP() > 0)
    {
    	return;
    }
  }
}