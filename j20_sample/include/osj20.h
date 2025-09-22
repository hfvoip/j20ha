/*
 * osj20.h
 *
 *  Created on: 2025年8月3日
 *      Author: xeeen
 */

#ifndef INCLUDE_OSJ20_H_
#define INCLUDE_OSJ20_H_


/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif /* ifdef __cplusplus */


#include <stdint.h>



#define CODEC_CONFIGURATION_SIZE     0x100
#define CODEC_SCRATCH_MEMORY_SIZE    0x10

/* Size of structure that is defined for codec specific parameters*/
#define CODEC_STRUCT_SIZE              0x100

typedef struct _sharedMemory
{
    unsigned char configuration[CODEC_CONFIGURATION_SIZE];
    unsigned char scratch[CODEC_SCRATCH_MEMORY_SIZE];
} SharedMemory_t;

/* Shared memory size */
#define SHARED_MEMORY_SIZE			 sizeof(SharedMemory_t)

extern SharedMemory_t shared_buffer;



typedef enum
{
	PRE_BQ = 0,
	POST_BQ = 1,
	NC = 2,
	EQ = 3,
	WDRC = 4,
	UPLOAD = 5,
	AUDIO_DUMP = 6,
	AFC = 7,
	LOOPBACK = 8,
	DPEQ = 9,
	AGCO = 10,
	TONE_GEN = 12,
	SOUND_GEN = 13

}Control_Bit;



#define Q4 	0xf
#define Q5 	0x1f
#define Q6 	0x3f
#define Q7 	0x7f
#define Q8 	0xff
#define Q9 	0x1ff
#define Q10 0x3ff
#define Q11 0x7ff
#define Q13	0x1fff
#define Q15	0x7fff
#define Q16 0xffff
#define Q17 0x1ffff
#define Q18 0x3ffff
#define Q19 0x7ffff
#define Q20 0xfffff
#define Q21 0x1fffff
#define Q23 0x7fffff
#define Q24 0xffffff
#define Q25 0x1ffffff
#define Q26 0x3ffffff
#define Q27 0x7ffffff
#define Q28 0xfffffff
#define Q29 0x1fffffff
#define Q30 0x3fffffff
#define Q31 0x7fffffff


typedef signed char Word8;
typedef	short	Word16;
typedef	short	INT16;
typedef int 	Word32;
typedef	int		INT32;

typedef	unsigned short 	UWord16;
typedef	unsigned short 	UINT16;
typedef unsigned int	UWord32;
typedef unsigned int	UINT32;

#define MAX_32 ((Word32)0x7fffffffL)
#define MIN_32 ((Word32)0x80000000L)

#define MAX_16 (Word16)0x7fff
#define MIN_16 (Word16)0x8000

#define MAX(a,b)		((a>b) ? (a):(b))
#define MIN(a,b)		((a<b) ? (a):(b))
#define MASK32(bit)		((UWord32)(1<<bit))
#define MASK16(bit)		((UWord16)(1<<bit))

#define MaxBandNum 4

typedef struct
{
	int Fix_Gain[33];
	int BQ_Enable_Cnt;
	int BQ[6*3];
}SM_CONFIG_EQ;

typedef struct
{
	short BandNum;
	short bin_num[17];
	int alfa[16];
	int beta[16];
	int maxdB;
    int exp_end_knee[16];
    int tkgain[16];
    int cr[16];
    int bolt[16];
	int tk_tmp[16];
    int cr_const[16];
    int tkgo[16];
    int pblt[16];
    int gain_at_exp_end_knee[16];
    int exp_cr_const[16];
	int limit_cr[16];
	char INR_enable[16];			// 0:disable 1:enable
	char INR_onset_threshold[16];	//-30~30dB
	char INR_offset_threshold[16];	//-30~30dB
	int INR_reduction_ratio[16];	//Ratio: 1~10
}SM_CONFIG_WDRC;


typedef struct
{
	Word32 sm_max[MaxBandNum];
	Word32 x_max[MaxBandNum];
	Word32 gain[MaxBandNum];
}WDRC_T;

/********************************* Share Memory **************************************/
typedef struct {
	short	RMS_dBSPL[8];	// 8-Band dBFs Q7
	short	Gain_8Band[8];	// 8-Band Gain
	short		NC_Dump[70]; 	// 40 NC Dump Value
	// Low Noise Flag 		(0/1)
	// NC Vox Flag			(0/1)
	// NC Gain (33 bins)	// Q15 NC_Gain =20*log10(X/2^15)
	// NC SNR (33 bins)
	short 	MISC[10];		// 10 Misc Data
} SM_UPLOAD_DATA;

typedef struct {
	INT32   nc_common_param[16];
	INT32   nc_personal_param[3];
	unsigned char   normal_max_depth_dB[32];		//0~F  (Normal Mode: max denoise 15dB)
	unsigned char   low_noise_max_depth_dB[32];	//0~F  (Low Noise Mode: max denoise 15dB)
} SM_CONFIG_NC;

typedef struct {
	INT32   point;
	INT32   drcgaininv;
	INT32   drcpeakline;
	INT32   drc_Table[65];
} SM_CONFIG_AGC;


typedef struct {
	INT32 NS_LEVEL;
	INT32 ATTACK;
	INT32 RELEASE;
} SM_CONFIG_AI_NS;

typedef struct {
	INT32 Pre_Enable_Cnt; 			//Enable Cnt: 0~3
	INT32 PreBQs[3][6];
	INT32 Post_Enable_Cnt;			//Enable Cnt: 0~3
	INT32 PostBQs[3][6];
} SM_CONFIG_FILTER;

typedef struct {
	short OnOff;
    short Mix;
	short Gain;
    short FirstToneA1Coef_WB;
    short SecondToneA1Coef_WB;
    short FirstToneB0Coef_WB;
    short SecondToneB0Coef_WB;
} SM_CONFIG_DTMF;

typedef struct {
	int Volume;
} SM_CONFIG_VOLUME;

typedef struct
{
	int Energy_Tconst;						//DPEQ Sense Energy Time constant
	unsigned int K_Const_Offset;			//DPEQ Threshold Offset	DPEQ Threshold Gain
} SM_CONFIG_DPEQ;

typedef struct
{
	int Threshold;
	int Attack;
	int Release;
} SM_CONFIG_AGCO;

typedef struct
{
	// Time-Domain
	int TFBC_Enable;
	int MuDivNumTaps;	//=mult_r(MU,InvNumOfTaps);
	int InvNumOfTaps;	//=32768/NumOfTaps
	int DerivedQshift;	//=round((Ref2EchoPowerRatio-3)/6)
	int EcTaps;
	int ConvergenceSpeed;
	int Echo2InRatioLineGain;

	//CrossOver Filters
	INT32 Hp_BQs[5];
	INT32 Lp_BQs[5];

	//Frequency-Domain
	char Gain_Management_Enable;
	short Gain_Margin[32];
	// Pre-FDAF	(Pre-Frequency Domain Adaptive Filter)
	int Pre_Adaptive_Filter;
	// PRE-Delay
	char Pre_Delay;						//0~63
	char Normalized_Adapt_Speed[32];	//0~15 Normalized adaptation speed. Zero is the fastest speed.
	char Idle_Speed;
	char Power_NormalizationBias;
	char LeakNorm;
} SM_CONFIG_FBC;


typedef struct
{
	int Type;
	int Duration;
} Sound_Event_Cfg;

typedef enum
{
	MUTE = 0,
	WHITE_NOISE = 1,
	POP = 2,
	DTMF_ON = 3,
	DTMF_OFF = 4,
}Pattern_Mode;

typedef enum
{
	NO_MIX_MIC = 0,
	MIX_MIC = 1,
}Sound_Mix_Mode;

typedef enum
{
	ALWAYS_ON = 0x7FFFFFFF,
}Sound_Generator_Mode;

typedef struct
{
	int Sound_Event_Count;
	int Mix_Control;
	int Noise_Gain;
	Sound_Event_Cfg Event[10];
} SM_CONFIG_SG;


/***************************************************************/
/********************* VERSION INFORMATION *********************/
/***************************************************************/
typedef struct
{
	unsigned char VER_A[32];
	unsigned char VER_B[32];
	unsigned char Log[4];
} SM_CONFIG_VERSION;


typedef struct {
	SM_UPLOAD_DATA  UPLOAD;
	SM_CONFIG_WDRC  WDRC_ShareMem;
	SM_CONFIG_EQ 	EQ_ShareMem;
	SM_CONFIG_DPEQ	DPEQ_ShareMem;
	SM_CONFIG_FILTER FILTER_ShareMem;
	SM_CONFIG_AI_NS AI_NS_ShareMem;
	SM_CONFIG_DTMF 	DTMF_ShareMem;
	SM_CONFIG_VOLUME VOLUME_ShareMem;
	SM_CONFIG_AGCO AGCO_ShareMem;
	SM_CONFIG_NC NS_ShareMem;
	SM_CONFIG_SG SG_ShareMem;
	SM_CONFIG_FBC FBC_ShareMem;
	SM_CONFIG_VERSION VER_ShareMem;
	UINT16  Control;

} ShareMemoryData;

#define AUDIO_BLOCK	16

typedef struct {
	char Byte[3];			// Only Use 3 Chars
	char Mix;				// 0:Replace 1:Mix
	char PCM_Process;		// 0:Process 1:No_Process
	char Reserved[3];		// Reserved 3chars
	char Dec_Data[3*4*16];	// 16Int32 (32bit = 4Byte) *3
} Decode_PCM_Data;

typedef enum
{
	PROCESS = 0,
	NO_PROCESS =1,
}Decode_PCM_Control_Bit;


/********************************* MCU ONLY **************************************/
//MCU Float Value
typedef struct {
	float maxdB;				// MPO default 120
	float exp_cr[16];			// Expansion: Ratio (exp_cr)
	float exp_end_knee[16];		// Expansion: End Kneepoint (exp_end_knee)
	float tkgain[16];			// Linear: Gain (tkgain)
	float tk[16];				// Compression Start Kneepoint (tk)
	float cr[16];				// Compression: Ratio (cr)
	float bolt[16];				// Limiter: Start Kneepoint (bolt)
	float limit_cr[16];			// Limiter Ratio (Default:10)
	float Attack_time[16];			// 200ms
	float Release_time[16];			// 200ms
	short BandNum;					// default = 8
	char INR_enable[16];			// 0:disable 1:enable
	char INR_onset_threshold[16];	//-30~30dB
	char INR_offset_threshold[16];	//-30~30dB
	float INR_reduction_ratio[16];	//Ratio: 1~10
	float CrossOverFreq[15];		// CrossOver Frequency
} MCU_Config_WDRC;

typedef struct {
	float dB_Gain_float[33];
	int BQ_Enable_Cnt;
	double BQs[6*3];
}MCU_Config_EQ;

typedef struct {
	INT32   nc_common_param[16];
	INT32   nc_personal_param[3];
	char   normal_max_depth_dB[32];		//0~F  (Normal Mode: NR max_depth  -15dB)
	char   low_noise_max_depth_dB[32];	//0~F  (Low Noise Mode: NR max depth -15dB)
} MCU_Config_NC;

typedef struct {
	float dB_gain;
	float dB_peak;
} MCU_Config_AGC;

typedef struct {
	float ADC_GAIN;
	float DAC_GAIN;
} MCU_Config_MULTI_GAIN;


typedef struct {
	float NS_LEVEL;
	float ATTACK;
	float RELEASE;
} MCU_Config_AI_NS;

typedef struct {
	int Pre_Enable_Cnt;				//Enable Cnt: 0~3
	double PreBQs[3][6];
	int Post_Enable_Cnt;			//Enable Cnt: 0~3
	double PostBQs[3][6];
	float ADC_Gain_dB;
	float DAC_Gain_dB;
} MCU_Config_FILTER;

typedef struct {
	short OnOff;
    short Mix;
	short Gain_dB;
    float FreqLow;
	float FreqHigh;
} MCU_Config_DTMF;

typedef struct {
	float Volume;		// 0dB  ~ -N dB
} MCU_Config_VOLUME;

/***************************************************************/
/*************************** DPEQ ******************************/
/***************************************************************/
typedef struct
{
	double Energy_Time;
	double Threshold_High;
	double Threshold_Low;
} MCU_Config_DPEQ;


/***************************************************************/
/*************************** AGC-O ******************************/
/***************************************************************/
typedef struct
{
	double Threshold;			//RMS: -15~0dB
	double Attack_Time;			//0.5~20ms
	double Release_Time;		//2~20ms
} MCU_Config_AGCO;


/***************************************************************/
/*************************** FBC ******************************/
/***************************************************************/
typedef struct
{
	// Time-Domain FBC
	int TFBC_Enable;		// Time-Domain FBC On/Off
	int SampleDelay;		//reference delay to echo
	int NumOfTaps;		//Fir filter length
	int ConvergenceSpeed;		//slowdown
	int Ref2EchoPowerRatio;		//ratio of ref to echo,[-12~30]db, decimal number

	// CrossOver Filters
	int Hp_Xover_Cnt;			//Enable Cnt: 0~2
	double Hp_BQs[2][6];
	int Lp_Xover_Cnt;			//Enable Cnt: 0~2
	double Lp_BQs[2][6];

	// Frequency-Domain FBC
	char Gain_Management_Enable;
	char Gain_Margin[32];
	// Pre-FDAF	(Pre-Frequency Domain Adaptive Filter)
	int Pre_Adaptive_Filter;
	// PRE-Delay
	char Pre_Delay;						//0~63
	char Normalized_Adapt_Speed[32];	//0~15 Normalized adaptation speed. Zero is the fastest speed.
	char Idle_Speed;
	char Power_NormalizationBias;
	char LeakNorm;
} MCU_Config_FBC;



/***************************************************************/
/*********************** SOUND GENERATOR ***********************/
/***************************************************************/

typedef struct
{
	int Sound_Event_Count;
	int Mix_Control;
	float Noise_Gain;
	Sound_Event_Cfg Event[10];
} MCU_Config_SG;


/***************************************************************/
/********************* VERSION INFORMATION *********************/
/***************************************************************/
typedef struct
{
	unsigned char VER_A[32];
	unsigned char VER_B[32];
} MCU_Config_VERSION;

#define AUDIO_BLOCK_SIZE  (32)


#define SM_BLOCK_SIZE   (32*2*2)
typedef struct RSL20_ShareMemory{
	int SM_Input[AUDIO_BLOCK_SIZE*2];   //20808200
	int SM_Dump[AUDIO_BLOCK_SIZE*2];
	int SM_Output[AUDIO_BLOCK_SIZE*2];
	int SM_Dec[AUDIO_BLOCK_SIZE*2];
	ShareMemoryData Config_Data;						// 0x00808800
} RSL20_MCU_DSP_ShareMemory;


extern struct RSL20_ShareMemory RSL20_Buffer;
extern ShareMemoryData* SM_Ptr;

extern unsigned char security_key[];

void J20_Codec_Initialize();
uint8_t  J20_UpdateDSP(void* ble_key,int key_len);
void J20_Handler(void* ptr);

#endif /* INCLUDE_OSJ20_H_ */
