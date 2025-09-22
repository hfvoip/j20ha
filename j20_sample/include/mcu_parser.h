/*
 * mcu_parser.h
 *
 *  Created on: 2024年9月21日
 *      Author: xeeen
 */

#ifndef INCLUDE_MCU_PARSER_H_
#define INCLUDE_MCU_PARSER_H_

#include "osj20.h"

extern MCU_Config_WDRC MCU_WDRC;
extern MCU_Config_EQ MCU_EQ ;
extern MCU_Config_DPEQ MCU_DPEQ ;
extern MCU_Config_NC MCU_NC ;
extern MCU_Config_AI_NS MCU_AI_NS ;
extern MCU_Config_FILTER MCU_FILTER ;
extern MCU_Config_DTMF MCU_DTMF;
extern MCU_Config_VOLUME MCU_VOLUME;
extern MCU_Config_NC MCU_NS_WIENER;
extern MCU_Config_AGCO MCU_AGCO;
extern MCU_Config_VERSION MCU_VERSION;

void EQ_Parser(MCU_Config_EQ* Cfg,SM_CONFIG_EQ* EQ_Cfg);
void DPEQ_Init(MCU_Config_DPEQ* UI_DPEQ,SM_CONFIG_DPEQ* DPEQ_Cfg);
void WDRC_Parser(MCU_Config_WDRC* Cfg,SM_CONFIG_WDRC* WDRC_Cfg);
void NC_Config_Init(MCU_Config_NC* Cfg,SM_CONFIG_NC* NC_Cfg);
void MCU_Config_Parser();
void AI_NS_Init(MCU_Config_AI_NS* Cfg,SM_CONFIG_AI_NS* AI_NS_Cfg);
void Filter_Init(MCU_Config_FILTER* Cfg,SM_CONFIG_FILTER* FILTER_Cfg);
void DTMF_Init(MCU_Config_DTMF* Cfg,SM_CONFIG_DTMF* DTMF_Cfg);
void Volume_Init(MCU_Config_VOLUME* Cfg,SM_CONFIG_VOLUME* Vol_Cfg);

void Fill_SmData_Buffer() ;




#endif /* INCLUDE_MCU_PARSER_H_ */
