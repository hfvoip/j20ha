/**
 * @file app_dmic.c
 * @brief Source code for supporting Digital Microphone (DMIC)
 *        and Output Driver (OD) peripherals
 * @copyright @parblock
 * Copyright (c) 2023 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 */

/* ----------------------------------------------------------------------------
 * Include files
 * --------------------------------------------------------------------------*/

#include "app.h"
#include "app_od_dmic.h"
#include "app_audio.h"
#include "app_codec.h"
#include "osj20.h"

/* ----------------------------------------------------------------------------
 * Module Variable Definitions
 * --------------------------------------------------------------------------*/

/* Circular audio buffer between DMIC and audio processor */
#if 0
int16_t dmic_input_buffer[AUDIO_IN_BUFFER_SIZE];
int16_t od_output_buffer[AUDIO_OUT_BUFFER_SIZE];
#else
int16_t* dmic_input_buffer = (int16_t*)&RSL20_Buffer.SM_Input[0];
int16_t* od_output_buffer = (int16_t*)&RSL20_Buffer.SM_Output[0];
ShareMemoryData* SM_Ptr = (ShareMemoryData*) &RSL20_Buffer.Config_Data;

#endif
uint16_t *dmic_readybuf;
bool dmic_inready;
uint32_t dmic_errcnt = 0;
uint32_t od_errcnt = 0;
uint32_t limit_errcnt = 0;



#if DMIC_SINE_IN

int16_t sine_ptr = 0;
#define NUM_SAMPLES_SINE440HZ_48KFS     108

const int16_t samples_sine440Hz_48kFs[NUM_SAMPLES_SINE440HZ_48KFS] =
{
        0,      1524,   3043,   4552,
        6045,   7518,   8966,   10383,
        11765,  13107,  14405,  15654,
        16850,  17989,  19068,  20081,
        21027,  21902,  22702,  23426,
        24070,  24633,  25113,  25508,
        25816,  26037,  26170,  26214,
        26170,  26037,  25816,  25508,
        25113,  24633,  24070,  23426,
        22702,  21902,  21027,  20081,
        19068,  17989,  16850,  15654,
        14405,  13107,  11765,  10383,
        8966,   7518,   6045,   4552,
        3043,   1524,   0,      -1524,
        -3043,  -4552,  -6045,  -7518,
        -8966,  -10383, -11765, -13107,
        -14405, -15654, -16850, -17989,
        -19068, -20081, -21027, -21902,
        -22702, -23426, -24070, -24633,
        -25113, -25508, -25816, -26037,
        -26170, -26214, -26170, -26037,
        -25816, -25508, -25113, -24633,
        -24070, -23426, -22702, -21902,
        -21027, -20081, -19068, -17989,
        -16850, -15654, -14405, -13107,
        -11765, -10383, -8966,  -7518,
        -6045,  -4552,  -3043,  -1524
};
#endif

/**
 * @brief       Configure the DMIC interface
 */
void APP_DMIC_Init(void)
{
    /* Configure DMIC I/O */
    Sys_Audio_DMIC_GPIOConfig(DMIC_GPIO_CONFIG, DMIC_CLK_DIO, DMIC_DATA_DIO);

    /* Configure but don't enable DMIC */
    Sys_Audio_Set_DMICConfig(DMIC_CFG_NO_ENABLE, 0);

    AUDIO->CFG |= DMIC0_DATA_MSB_ALIGNED;
    AUDIO->DMIC0_GAIN = APP_DMIC0_GAIN;

    dmic_inready = false;

    APP_DMIC_DMAInit();
    NVIC_EnableIRQ(AUDIO_IRQn);

    return;
}

/**
 * @brief Configure the OD interface
 */
void APP_OD_Init(void)
{
    /* Enable OD regulator */
    SYSCTRL->VDDOD_CTRL |= VDDOD_ENABLE;

    AUDIO->CFG |= OD_DATA_MSB_ALIGNED;
    AUDIO->OD_GAIN = APP_OD_GAIN;
    AUDIO->OUTPUT_LIMITER = APP_OUTPUT_LIMITER;
    AUDIO->SDM_DCRM_CTRL = APP_SDM_DCRM_CTRL;

    APP_OD_DMAInit();
    NVIC_EnableIRQ(AUDIO_IRQn);
}

/**
 * @brief Initialize the OD DMA
 */
void APP_OD_DMAInit(void)
{
    /* OD output DMA */
	//by yang 20240828, od_output_buffer=>codec_output_buffer
    Sys_DMA_ChannelConfig(DMA_NUM(OD_DMA),
                          OD_DMA_CFG,
						  AUDIO_BLOCK_SIZE*2 /*  AUDIO_IN_BUFFER_SIZE*/,
						  AUDIO_BLOCK_SIZE,
						  (uint32_t)&RSL20_Buffer.SM_Output[0],
                          (uint32_t)OD_DATA_16_MSB_ADDR);
}

/**
 * @brief Initialize the DMIC DMA
 */
void APP_DMIC_DMAInit(void)
{
    /* DMIC input DMA */
    Sys_DMA_ChannelConfig(DMA_NUM(DMIC_DMA),
                          DMIC_DMA_CFG,
						  AUDIO_BLOCK_SIZE*2 /*  AUDIO_IN_BUFFER_SIZE*/,
						AUDIO_BLOCK_SIZE /* AUDIO_IN_BUFFER_SIZE / 2 */,
                          (uint32_t)&AUDIO->DMIC0_DATA,
						  (uint32_t)&RSL20_Buffer.SM_Input[0]

    );
}

/**
 * @brief Get pointer to most recently filled DMIC input buffer and
 * clear Inready status
 */
int16_t *APP_DMIC_GetReadyBufPtr(void)
{
    dmic_inready = false;
    return(dmic_readybuf);
}

/**
 * @brief Get pointer to OD output buffer
 */
int16_t *APP_OD_GetBufPtr(void)
{
    return (int16_t*)(&od_output_buffer[0]);

}

/**
 * @brief DMIC input buffer ready status
 */
bool APP_DMIC_Inready(void)
{
    return(dmic_inready);
}

/**
 * @brief Enable DMIC interface
 */
void APP_DMIC_Start(void)
{
    Sys_DMA_Mode_Enable(DMA_NUM(DMIC_DMA), DMA_ENABLE_WRAP_RESTART);
    AUDIO->CFG = AUDIO->CFG | DMIC0_ENABLE;
	Memory_Log(0xDCDC);
}

/**
 * @brief Enable OD interface
 */
void APP_OD_Start(void)
{
    Sys_DMA_Mode_Enable(DMA_NUM(OD_DMA), DMA_ENABLE_WRAP_RESTART);
    AUDIO->CFG = AUDIO->CFG | OD_ENABLE;
	Memory_Log(0x0D0D);
}

/**
 * @brief Disable OD & DMIC interfaces
 */
void APP_OD_DMIC_Stop(void)
{
    AUDIO->CFG = AUDIO->CFG & ~OD_ENABLE &
                 ~DMIC1_ENABLE & ~DMIC0_ENABLE;
    Sys_DMA_Mode_Enable(DMA_NUM(DMIC_DMA), DMA_DISABLE);
    Sys_DMA_Mode_Enable(DMA_NUM(OD_DMA), DMA_DISABLE);
}


/**
 * @brief  AUDIO_IRQHandler interrupt handler
 */
void AUDIO_IRQHandler(void)
{
    if (AUDIO->STATUS & (DMIC1_OVERRUN_DETECTED | DMIC0_OVERRUN_DETECTED))
    {
        /* Handle DMIC errors here then clear */
        if (dmic_errcnt < UINT32_MAX_VAL)
            dmic_errcnt++;
        AUDIO->STATUS = (DMIC1_OVERRUN_FLAG_CLEAR | DMIC0_OVERRUN_FLAG_CLEAR);
    }
    if (AUDIO->STATUS & OD_UNDERRUN_DETECTED)
    {
        /* Handle OD errors here then clear */
        if (od_errcnt < UINT32_MAX_VAL)
            od_errcnt++;
        AUDIO->STATUS = OD_UNDERRUN_FLAG_CLEAR;
    }
    if (AUDIO->STATUS & OUTPUT_LIMITING_DETECTED)
    {
        /* Handle OD errors here then clear */
        if (limit_errcnt < UINT32_MAX_VAL)
            limit_errcnt++;
        AUDIO->STATUS = OUTPUT_LIMITER_FLAG_CLEAR;
    }
	Memory_Log(0xAAAA);
}

volatile uint16_t dmic_int = 0;
volatile uint16_t count_int = 0;
void DMA_IRQ_FUNC(DMIC_DMA) (void)
{
	

	dmic_int++;
	count_int++;
	Memory_Log(0xDDDD);
	
	
}
#if  0
/**
 * @brief Interrupt Handler for the DMIC DMA
 */
void DMA_IRQ_FUNC(DMIC_DMA)(void)
{

    /* Check interrupt source to determine which half-buffer has been filled
     * If the DMA_COMPLETE_INT occurred, the second half of the buffer has just been filled,
     *   -> set dmic_readybuf pointer to the start of the second half of the buffer
     * If the DMA_CNT_INT occurred, the first half of the buffer has just been filled,
     *   -> set dmic_readybuf pointer to the start of the buffer
     *
     * NOTE: If the count is set to half of the buffer size, the DMA_CNT_INT also occurs
     *       when DMA_COMPLETE_INT occurs. In this case, clear the count interrupt when the
     *       complete interrupt occurs so we only respond to one interrupt at a time.
     */
    if ((DMA_NUM(DMIC_DMA)->STATUS & DMA_COMPLETE_INT_TRUE) == DMA_COMPLETE_INT_TRUE)
    {
        DMA_NUM(DMIC_DMA)->STATUS = (DMA_COMPLETE_INT_CLEAR | DMA_CNT_INT_CLEAR);
        dmic_readybuf = (uint16_t*)&dmic_input_buffer[AUDIO_IN_BUFFER_SIZE / 2];
        dmic_inready = true;
        app_audio_int++;
    }
    else if ((DMA_NUM(DMIC_DMA)->STATUS & DMA_CNT_INT_TRUE) == DMA_CNT_INT_TRUE)
    {
        DMA_NUM(DMIC_DMA)->STATUS = DMA_CNT_INT_CLEAR;
        dmic_readybuf = (uint16_t*)&dmic_input_buffer[0];
        dmic_inready = true;
        app_audio_int++;
    }
#if DMIC_SINE_IN
    /* Overwrite the readybuf with samples from the constant sine wave */
    for(int8_t i = 0; i < AUDIO_IN_BUFFER_SIZE / 2; i++)
    {
        dmic_readybuf[i] = samples_sine440Hz_48kFs[sine_ptr++];
        if(sine_ptr >= NUM_SAMPLES_SINE440HZ_48KFS)
        {
            sine_ptr = 0;
        }
    }
#endif

#define AUDIO_PASS_OD_XX
#ifndef AUDIO_PASS_OD
    //这里是交给DSP echo  处理,DMIC DMA中断可以在DSP中处理，参考LPDSP32_low_delay_audio.c
    App_Codec_ExecutePendingAction();
#else
    //这里直接给OD 输出
    int16_t *pOutBuf;
    pOutBuf = APP_OD_GetBufPtr();
    static int write_ptr = 0;
    static int out_count = 0;
    static bool output_started = false;
    static int sample_cnt =0;

    /* Get number of samples to transfer */
    sample_cnt = AUDIO_IN_BUFFER_SIZE/2;

    int idx =  write_ptr ;

	  for (int i = 0; i < sample_cnt; i++)
	  {
		  pOutBuf[idx++] =  dmic_readybuf[i];
		  if (idx >= AUDIO_OUT_BUFFER_SIZE)
			  idx = 0;
	  }

	  /* update write pointer for next time */
	  write_ptr  = idx;
	  //第一次enable od 是在满足一个od dma buffer以后
	   out_count  += sample_cnt;
	  if (!output_started)
	  {
		  if (out_count >= AUDIO_OUT_BUFFER_SIZE / 2)
		  {
			   APP_OD_Start();
			   output_started  = true;
		  }
	  }
#endif

}
#endif

/**
 * @brief Interrupt Handler for the OD DMA
 */
void DMA_IRQ_FUNC(OD_DMA)(void)
{
    if ((DMA_NUM(OD_DMA)->STATUS & DMA_COMPLETE_INT_TRUE) == DMA_COMPLETE_INT_TRUE)
    {
        DMA_NUM(OD_DMA)->STATUS = DMA_COMPLETE_INT_CLEAR;
    }
    app_audio_int++;
}
