/**
 * @file app_codec.c
 * @brief Source code for audio path control
 *
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

#include <swmTrace_api.h>
#include "app.h"
#include "app_od_dmic.h"
#include "app_audio.h"
#include "app_codec.h"
#include "loader.h"
#include "osj20.h"

/* ----------------------------------------------------------------------------
 * Defines
 * -------------------------------------------------------------------------- */

/* Buffer configuration numbers */
#define CODEC_MAX_PAYLOAD_SIZE                  1024
#define CODEC_INPUT_MEDIA_PACKET_COUNT          (CODEC_INPUT_SIZE / CODEC_MAX_PAYLOAD_SIZE)
#define CODEC_FIRST_BUFFER_INDEX                0

#define SBC_DECODED_TOTAL_FRAME_SIZE_IN_BYTES  240
#define SBC_DECODED_DATA_FRAME_SIZE_IN_BYTES   240
#define SBC_DECODED_DATA_OFFSET_IN_BYTES       0

#define CODEC_MEDIA_PACKET_BUFFER_STATE_FREE        0
#define CODEC_MEDIA_PACKET_BUFFER_STATE_JUST_COPIED 1
#define CODEC_MEDIA_PACKET_BUFFER_STATE_DECODING    2

#define CODEC_NO_ACTION  0
#define CODEC_INIT      (1 << CODEC_ACTION_CONFIGURE)

/* ----------------------------------------------------------------------------
 * Module Variable Definitions
 * --------------------------------------------------------------------------*/


typedef struct
{
    bool is_lost_packet_report;
    uint8_t  state;
    uint16_t encoded_frame_size;
    uint16_t frame_count;
    uint16_t current_frame;
    uint8_t *p_payload_buffer;
}Codec_Media_Packet_Data_t;
typedef struct
{
    uint8_t writing_packet_index;
    uint8_t reading_packet_index;
    uint8_t media_packet_capacity;
    Codec_Media_Packet_Data_t *p_data;
}Codec_Media_Packet_Control_t;
typedef struct
{
    uint32_t samples_per_frame;
    uint32_t writing_buffer_index;
    uint32_t reading_buffer_index;
    uint32_t frame_count_capacity;
    uint8_t *p_output_pool;
}Codec_Output_Control_t;
typedef struct
{
    /*  NOTE: These two backup bytes are to patch the problem caused by the SBC decoder that
     *        corrupts the following two bytes on the input buffer after the encoded frame.
     *        This is a bug that needs to be fixed on the SBC decoder.
     */
    uint8_t backup_byte1;
    uint8_t backup_byte2;

    bool is_codec_active;
    bool codec_init_pending;
    bool is_dsp_running;
    bool ready_to_load;
    bool frame_size_updated;

    Codec_Media_Packet_Control_t input_media_packet;
    Codec_Output_Control_t output_control;
} Codec_Control_t;
static Codec_Control_t codec_control;
static Codec_Media_Packet_Data_t input_media_packet[CODEC_INPUT_MEDIA_PACKET_COUNT];


/* ----------------------------------------------------------------------------
 * Local Function Prototype Definitions
 * --------------------------------------------------------------------------*/

/**
 * @brief  DSP0_IRQHandler interrupt handler
 * @note   Triggered when codec has completed an action
 */
void DSP0_IRQHandler(void);

/* ----------------------------------------------------------------------------
 * Function Definitions
 * --------------------------------------------------------------------------*/

/**
 * @brief Configure codecs
 */
void App_Codec_Init(void)
{

}

/**
 * @brief Load codec
 */
void App_Codec_Load(void)
{
   J20_Codec_Load();
}

/**
 * @brief Stops codec
 */
void App_Codec_Stop(void)
{
    codec_control.is_codec_active = false;
}




uint8_t T_Buf[96*4+1];

void dumpJ20Write(uint8_t* ptr) {
#if 1
	int total_bytes = AUDIO_BLOCK*2*2;
	for (int i=0;i<total_bytes;i++) {
			uint8_t  a = ptr[i];
			uint8_t a1 = (a &0xf0) >>4 ;
			uint8_t a2 = a &0x0f;
			T_Buf[i*2]  =a1+0xA0;
			T_Buf[i*2+1] = a2+0x80;
		}
		T_Buf[total_bytes*2] =0x1;

		unsigned  total_written =SEGGER_RTT_Write(0,(uint8_t*)T_Buf,total_bytes*2+1);
		if (total_written ==0) {
			T_Buf[0] =0x9;
			total_written =SEGGER_RTT_Write(0,(uint8_t*)T_Buf,1);

		}
#endif
}

/**
 * @brief  DSP0_IRQHandler interrupt handler
 * @note   Triggered when codec has completed an action
 */
int Record_outsamples = 0;
void DSP0_IRQHandler(void)
{

    codec_control.is_dsp_running = false;

    //by yang,10个frame 以后，打开OD DMA，后听到OD输出

    static uint32_t out_samples = 0;
	if(out_samples<5*1000)
    	out_samples ++;
	else {
		//过了100个sample 后才会dump
		if(SM_Ptr->Control&MASK16(AUDIO_DUMP))
			dumpJ20Write(0x21808300);
	}

	
	Memory_Log(0xEEEE);
	extern int dmic_int;
	dmic_int--;
    NVIC_ClearPendingIRQ(DSP0_IRQn);


	static bool output_started = false;
	if ((!output_started)&&(out_samples>6))
	{
	   while(((RSL20_Buffer.SM_Dump[0]-out_samples)&1) == 0)
	   {
	   
	   }
	   Record_outsamples = RSL20_Buffer.SM_Dump[0]-out_samples;
	   APP_OD_Start();
	   memset(&RSL20_Buffer.SM_Output[0],0,SM_BLOCK_SIZE);
	   output_started  = true;
	}

}

/**
 * @brief Reconfigures the DSP, reloading code and resetting it
 */
void App_Codec_Reconfig(void)
{
    App_Codec_Load();
}






