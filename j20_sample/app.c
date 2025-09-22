/**
 * @file app.c
 * @brief Sample application code for Bidirectional PCM Audio Passthrough
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
#include "sample_input.h"
#include "app_audio.h"
#include "app_od_dmic.h"
#include "app_codec.h"
#include "device_init.h"
#include "app_bt.h"
#include "mcu_parser.h"


volatile uint16_t app_audio_int = 0;

//这是传给libosj20 作为license 验证，此处没有实现，默认lib 会隔断时间播放滴滴声,正式商用需采用正确的security_key
uint8_t security_key[64] = {0x1,0x2,0x3,0x4};

/* ----------------------------------------------------------------------------
 * Function Definitions
 * --------------------------------------------------------------------------*/

/**
 * @brief Initialize system for application
 */
void APP_Initialize (void)
{
    /* Set DMICCLK = 4MHz with ODCLK 16MHz */
	//by yang:  注意DMIC 的PRESCALE 是相对于ODCLK，不是SYSCLK,所以这里是16M/4= 4M
	// 能否改为2M,2025-04-24测试


    CLK->DIV_CFG0 = (CLK->DIV_CFG0 &
				  ~(CLK_DIV_CFG0_DMICCLK_PRESCALE_Mask)) |
				  DMICCLK_PRESCALE_4;


    /* Reset all interfaces to ensure clean system at start */
    APP_DisableAudioPath();

   /* 这里去掉了ASRC,ASCC,PCM */

    /* Configure OD and DMIC0, but don't enable */
    APP_Audio_Init();
    /* Load Codec */
    App_Codec_Load();

	APP_DMIC_Init();
	APP_OD_Init();



    /* Enable interrupts */
    NVIC_ClearPendingIRQ(DMA_IRQn(DMIC_DMA));
    NVIC_ClearPendingIRQ(DMA_IRQn(OD_DMA));
    NVIC_ClearPendingIRQ(AUDIO_IRQn);
    NVIC_ClearPendingIRQ(TIMER0_IRQn);
	NVIC_ClearPendingIRQ(DSP0_IRQn);


    NVIC_SetPriority(DMA_IRQn(DMIC_DMA), 2);
    NVIC_SetPriority(DMA_IRQn(OD_DMA), 2);
    NVIC_SetPriority(AUDIO_IRQn, 2);
    NVIC_SetPriority(TIMER0_IRQn, 0);
	NVIC_SetPriority(DSP0_IRQn, 2);



    NVIC_EnableIRQ(DMA_IRQn(DMIC_DMA));
    NVIC_EnableIRQ(DMA_IRQn(OD_DMA));
    NVIC_EnableIRQ(AUDIO_IRQn);
    NVIC_EnableIRQ(TIMER0_IRQn);
	NVIC_EnableIRQ(DSP0_IRQn);

}

/**
 * @brief Disable interrupts
 */
void APP_DisableInterrupts(void)
{
    /* Mask all interrupts */
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);

    /* Disable all existing interrupts, clearing all pending source */
    Sys_NVIC_DisableAllInt();
    Sys_NVIC_ClearAllPendingInt();
}

/**
 * @brief Enable interrupts
 */
void APP_UnmaskInterrupts(void)
{
    /* Unmask all interrupts */
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
}

/**
 * @brief Clear CNTS and buffers for all DMA channels used
 */
void APP_ClearDMAChannels(void)
{

    DMA_NUM(DMIC_DMA)->CTRL = DMA_CLEAR_BUFFER | DMA_CLEAR_CNTS;
    DMA_NUM(OD_DMA)->CTRL = DMA_CLEAR_BUFFER | DMA_CLEAR_CNTS;

}

/**
 * @brief Reset and disable all interfaces used in the audio path
 */
void APP_DisableAudioPath(void)
{
	APP_Audio_Stop();
    APP_ClearDMAChannels();

}


extern int sine_wav[];
void APP_PlayPCM() {
	//这是 16k采样率， int32 数组
	int total_bytes =  48*4;
	int sine_wav_gained[48];
	for (int i=0;i<48;i++)
		sine_wav_gained[i] = sine_wav[i]>>8;

	uint8_t* arr_playfile = (uint8_t*)sine_wav_gained;
	 int index_encoded =0;
	 int buffer_idx = 0;
	 //16个int *3
	 //这里假设arr_encodeddata是uint8_t
	 //total 3+2+3+48*4 =200
	 //uint8_t arr_flags[3];
	 //uint8_t mixe;
	 //uint8_t pcm_process;
	 //int    arr_data[16][3];

	 uint8_t* Buffer_PCM  = (uint8_t*)&RSL20_Buffer.SM_Dec[0];
	 //头3个字节是3个buffer的flag
	 uint8_t* arr_flags = (uint8_t*)Buffer_PCM;
	 //Decode 区块一共 16*2*4 = 128 byte
	  memset(Buffer_PCM,0,AUDIO_BLOCK_SIZE*2*4);
	  //1，1 配置没有电流音，耳朵里可听到滴滴，对MIC吹气，也可听到吹气
	  //1 ,0 才表示经过算法处理 ,无电流音，可感知AFC,也通过调节volume ,知道经过算法
	  // processed = 0,才表示经过算法处理 ,无电流音，可感知AFC,也通过调节volume ,知道经过算法

	  //0,0 电流音
	  //0,1  也有电流音
	  Buffer_PCM[3] = 1;  //mixed
	  Buffer_PCM[4] = 1;  //processed

	 //以下两句是mute 了DMIC ,可以不Mute的（如果希望同时听到音乐和助听器声音)
	  AUDIO->DMIC0_GAIN =0x0;
	  AUDIO->DMIC1_GAIN =0x0;



	  int loops_256 =0;

    while (index_encoded <= (total_bytes-64) ) {

		if (arr_flags[buffer_idx] ==0) {

				memcpy((uint8_t*)&Buffer_PCM[8+64*buffer_idx],
						(void*)&arr_playfile[index_encoded],
				16*4);
				index_encoded +=64;
				if (index_encoded >=192)  {
					index_encoded = 0;
					loops_256++;

				}

				arr_flags[buffer_idx]=1;

				buffer_idx ++;
				buffer_idx &=0x02;
				SYS_WATCHDOG_REFRESH();

				if (loops_256>100)  break;

		}
    }
    AUDIO->DMIC0_GAIN =0x800;
   	AUDIO->DMIC1_GAIN =0x800;

}


int log_tickcnt = 0;
int ccc = 0;
int main_simple(void)
{
    /* General System Initialization */
    Device_Initialize();

    /* APP-specific Initialization */
    APP_Initialize();

    /* Unmask Interrupts */
    APP_UnmaskInterrupts();


    /* Start DMIC to run audio path */
    APP_Audio_Start();

    APP_PlayPCM();



	int loops =0;
	int i;
	extern void swmLog(uint32_t level, const char *sFormat, ...);
	//swmLog(SWM_LOG_LEVEL_WARNING,"j20 start\n");
	
    while (true)
    {
    #if 0
	loops++;
	if (loops >400000)
	{


		#define LOG_RMS_DBFS(A) 		((int)((float)SM_Ptr->UPLOAD.RMS_dBSPL[A]/128.0f))
		#define GAIN_DB(A) 			((int)(20*log10(MAX(1,SM_Ptr->UPLOAD.Gain_8Band[A])/32.0f)))
		#define W_DBFS(A) 			((int)(20*log10(MAX(1,SM_Ptr->UPLOAD.NC_Dump[37+A])/32.0f)))
		loops = 0;
		SM_UPLOAD_DATA* Ptr = &SM_Ptr->UPLOAD;
		swmLog(SWM_LOG_LEVEL_WARNING,"RMS_dBFs:%d,%d,%d,%d,%d,%d,%d,%d\n",LOG_RMS_DBFS(0),LOG_RMS_DBFS(1),LOG_RMS_DBFS(2),LOG_RMS_DBFS(3),LOG_RMS_DBFS(4),LOG_RMS_DBFS(5),LOG_RMS_DBFS(6),LOG_RMS_DBFS(7));
		swmLog(SWM_LOG_LEVEL_WARNING,"8Band Gain_dB:%d,%d,%d,%d,%d,%d,%d,%d\n",GAIN_DB(0),GAIN_DB(1),GAIN_DB(2),GAIN_DB(3),GAIN_DB(4),GAIN_DB(5),GAIN_DB(6),GAIN_DB(7));
		swmLog(SWM_LOG_LEVEL_WARNING,"Weight DBFS 0:%d,%d,%d,%d,%d,%d,%d,%d\n",W_DBFS(0),W_DBFS(1),W_DBFS(2),W_DBFS(3),W_DBFS(4),W_DBFS(5),W_DBFS(6),W_DBFS(7));
		swmLog(SWM_LOG_LEVEL_WARNING,"Weight DBFS 2000:%d,%d,%d,%d,%d,%d,%d,%d\n",W_DBFS(8+0),W_DBFS(8+1),W_DBFS(8+2),W_DBFS(8+3),W_DBFS(8+4),W_DBFS(8+5),W_DBFS(8+6),W_DBFS(8+7));
		swmLog(SWM_LOG_LEVEL_WARNING,"Weight DBFS 4000:%d,%d,%d,%d,%d,%d,%d,%d\n",W_DBFS(16+0),W_DBFS(16+1),W_DBFS(16+2),W_DBFS(16+3),W_DBFS(16+4),W_DBFS(16+5),W_DBFS(16+6),W_DBFS(16+7));
		swmLog(SWM_LOG_LEVEL_WARNING,"Weight DBFS 6000:%d,%d,%d,%d,%d,%d,%d,%d\n",W_DBFS(24+0),W_DBFS(24+1),W_DBFS(24+2),W_DBFS(24+3),W_DBFS(24+4),W_DBFS(24+5),W_DBFS(24+6),W_DBFS(24+7));

		swmLog(SWM_LOG_LEVEL_WARNING,"Misc:Energy:%x  Howl flag:%x\r\n", SM_Ptr->UPLOAD.MISC[2],SM_Ptr->UPLOAD.MISC[3]);
		extern short dmic_int;
		if((dmic_int>1)||(dmic_int<0))
			swmLog(SWM_LOG_LEVEL_WARNING,"dmic_int error!:%d\r\n", dmic_int);
		// NVIC_SystemReset();
	}

	#endif
	#if 1
    	/* Wait for interrupts */
        __WFI();
		//Check_Timing();

	#endif


        /* Refresh the watchdog timer */
        SYS_WATCHDOG_REFRESH();

        /* Wait for interrupts */
        //__WFI();
		Check_Timing();





		//if(app_audio_int>0)
			//Audio_Task();
        /* Refresh the watchdog timer */
        SYS_WATCHDOG_REFRESH();
    }
}


#define ENABLE_HA  1
int main_full() {
	  /* Initialize the application*/
	    App_Init();



	    /* Initialize sleep-related parameters for the Bluetooth stack */
	    static struct bt_sleep_api_param_tag bt_sleep_api_param =
	    {
	        .app_sleep_request = false, /* No sleep */
	    };

	    /* Initialize Bluetooth Stack */
	    BT_InitOptions_t options;
	    options.power_supply_cfg = power_supply_cfg;
	    BT_Stack_Init(&options);

	    /* APP-specific Initialization */

	       APP_Initialize();


	    /* Enable Interrupts that were disabled in Device_Initialize() */
	    App_EnableInterrupts();

	    /* Initialize Bluetooth Services */
	    BatteryServiceServerInit();
	    DeviceInformationServiceServerInit();
	    CustomServiceServerInit();

	    /* Prepare advertising and scan response data (device name + company ID) */
	    PrepareAdvScanData();

	    App_RegisterHandlers();

	    /* Configure the Bluetooth stack and app */
	    App_BTConfig();

	    //2025-05-09：此时频响曲线和第一次打开网页，点save config 的不一样，是不是网页的默认值和这里的默认值不太对？强制执行J20_UPDATE_DSP ?
	    //  app_env_cs.rx_changed ==1;

	    //另外markgain 参数还没起作用，以后改

	    /* Start DMIC to run audio path */
	       APP_Audio_Start();
	     //2025-08-04：此处展示了如何播放纯音，也可以改为从BT 传来的音乐数据，或任何语音提示
	       //APP_PlayPCM 支持和助听器DMIC的数据混音，以及对BT数据进行LPDSP32的算法处理
	       APP_PlayPCM();

	    while(1)
	    {
	    	SYS_WATCHDOG_REFRESH();

	   // 	Check_Timing();

	         J20_UPDATE_DSP();


	         extern short dmic_int;
			if((dmic_int>1)||(dmic_int<0))
				swmLog(SWM_LOG_LEVEL_WARNING,"dmic_int error!:%d\r\n", dmic_int);

	        /* Process events */
	        rwip_process();

	        /* Disable interrupts */
	        GLOBAL_INT_DISABLE();
	        /* Check if the processor clock can be gated */
	        if (rwip_sleep(&bt_sleep_api_param) != RWIP_ACTIVE)
	        {
	            /* Wait for interrupt */
	            WFI();
	        }
	        /* Enable interrupts */
	        GLOBAL_INT_RESTORE();
	    }

	    return 0;
}

int main() {
	/* j20 不是免费软件，如果你用于商业用途，请联系JHEARING 关于license 事宜 */
	/* main_full是带BLE的,main_simple()不带， JHEARING 只负责libopenj20.385.a , 其他功能由SDK使用方处理，这里的按键，BLE等可由开发者修改 */
	/* 已知BUG: SM_Ptr->Control 中包括了LPDSP32运行的算法模块，如果在运行中改变，会导致LPDSP32处理时间发生变化，进而与OD DMA中断时间不同步，声音有沙沙响 */

	main_full();
}


