/**
 * @file app_audio.c
 * @brief Source code for audio path control
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
#include "app_audio.h"
#include "app_od_dmic.h"
#include "mcu_parser.h"

/* ----------------------------------------------------------------------------
 * Module Variable Definitions
 * --------------------------------------------------------------------------*/

static bool app_run;

/** Forward definition of the context structure type */
struct asrc_context;

/**
 * @brief Standard state prototype.
 * @details All states in the system will conform to this structure.
 *  This defines a pointer to a state function.
 */
typedef void (*state_t)(struct asrc_context *context);

/** @brief Definition of the context structure for the application */
typedef struct asrc_context
{
    state_t     execute;
    bool        finished;
    volatile bool   asrc_in_done;
    volatile bool   asrc_done;
    uint8_t     channel;        /* channel being processed by ASRC */
    bool        output_started[AUDIO_STREAMS];
    uint32_t    out_count[AUDIO_STREAMS];
    int32_t     write_ptr[AUDIO_STREAMS];
} asrc_context_t;

struct asrc_context audioContext;

/* ----------------------------------------------------------------------------
 * Local Function Prototype Definitions
 * --------------------------------------------------------------------------*/
static void stateReset(asrc_context_t *context);
static void stateIdle(asrc_context_t *context);
static void statePCMInputDataReady(asrc_context_t *context);
static void stateDMICInputDataReady(asrc_context_t *context);
static void stateASRCInDMADone(asrc_context_t *context);
static void stateASRCDone(asrc_context_t *context);
static void Enable_Output(uint32_t channel);
static void Reset_Audio_State(void);
/* ----------------------------------------------------------------------------
 * Function Definitions
 * --------------------------------------------------------------------------*/

/**
 * @brief Initialize Audio State Machine
 */
void APP_Audio_Init(void)
{
    int32_t i;

    Reset_Audio_State();
    Fill_SmData_Buffer();

    /* Disable Done interrupts */
    //然后decmimate 4M/160=25K ,要不要改为4M/128= 31.** K,先不改
    Sys_Audio_Set_Config(APP_AUDIO_CFG_NO_ENABLE);

    AUDIO->INT_CFG = APP_AUDIO_INT_CFG;







}

int Check_cnt = 0;
int Err_cnt0 =0;
extern int dmic_int;

void Check_Timing()
{
	int T32;
	if(Check_cnt==0)
	{
		if(SM_Ptr->UPLOAD.MISC[1]!= 0x2502)
			Err_cnt0++;
		else
			Check_cnt = SM_Ptr->UPLOAD.MISC[0];
	}

}



/**
 * @brief Enable Audio FSM execution
 */
void APP_Audio_Start(void)
{
    app_run = true;
    APP_DMIC_Start();
}

/**
 * @brief Disable Audio FSM execution and clear state
 */
void APP_Audio_Stop(void)
{
    APP_OD_DMIC_Stop();
    Reset_Audio_State();
}


//下面的类似状态机处理都不要了，我们没有用到ASRC,不过记得在DSP处理后APP_OD_Start()

/**
 * @brief Update Audio State
 */
void APP_Audio_Run(void)
{
	int in_Buf[48];
	for(int i=0; i<48; i++){

		in_Buf[i] = 0;
	}
	int32_t low_noise_flg;
	int32_t	ret=0;
	int i,j,kk;
	// int Mask[SF_EQ_BandNum];
	int Envelope[AUDIO_BLOCK];
	int Gain[AUDIO_BLOCK];

	int Ref_Buf[AUDIO_BLOCK];
	int Hp_Buf[AUDIO_BLOCK];
	int Lp_Buf[AUDIO_BLOCK];

}

int Index = 0;
int Log_Memory[64];
void Memory_Log(int Cnt)
{
	if(Index<28)
	{
		int*Ptr = 0xE000E010;
		Log_Memory[2*Index] = Cnt;
		Log_Memory[2*Index+1] = (*Ptr)>>3;
		Index++;
		Index&=0x1F;
	}
}


static void stateReset(asrc_context_t *context)
{


}

static void stateIdle(asrc_context_t *context)
{

}
static void stateDMICInputDataReady(asrc_context_t *context)
{


}

static void stateASRCInDMADone(asrc_context_t *context)
{

}

static void stateASRCDone(asrc_context_t *context)
{


}

/**
 * @brief Enable OD peripheral
 */
static void Enable_Output(uint32_t channel)
{
    switch(channel)
    {
    case 0:
        APP_OD_Start();
        break;
    default:
        break;
    }
}

/**
 * @brief Reset audioContext state variables and counters
 */
static void Reset_Audio_State(void)
{
    int i;

    audioContext.asrc_done = false;
    audioContext.asrc_in_done = false;
    audioContext.finished = true;
    audioContext.execute = stateReset;
    audioContext.channel = 0;
    app_run = false;

    for (i = 0; i < AUDIO_STREAMS; i++)
    {
        audioContext.out_count[i] = 0;
        audioContext.write_ptr[i] = 0;
        audioContext.output_started[i] = false;
    }
}

