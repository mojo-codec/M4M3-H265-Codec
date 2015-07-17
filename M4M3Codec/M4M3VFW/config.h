/****************************************************
	M4M3 H.265 VFW software codec
	developed by mojo
	Copyright (c)2002-2015 mojo,all rights reserved
****************************************************/
#ifndef _config_h
#define _config_h

#include "def.h"
#include "../M4M3Lib/API/M4M3API.h"

class ConfigItem
{
public:
	bool m_bEnable;

	int m_ItemNameIdx;
	int m_MaxValue;
	int m_SelectValue;

	int m_Value[16];
	int m_ValueNameIdx[16];
};

class CVFWConfig
{
public:
	CVFWConfig();
	~CVFWConfig();

	ConfigItem	//m_BasicStreamSetting,
				//m_AdvanceSetting,
				//m_Language,
				//m_Profile,
				m_Performance,
				m_ColorSpace,
				m_BitDepth,
				m_IDR,
				m_GOP,
				m_RateControl,
				m_RateControlPar[3],
				m_FrameRate,
				m_DeblockingFilter,
				m_SAO,
				m_InterlacedVideo,
				m_QuantizaMatrix,
				m_Threads
				
				//m_MaxRefFrame,
				
				//m_MotionSearch,
				//m_Coding,
				//m_8x8dct,
				//m_Interlace,
				//m_MBAFF,
				//m_PAFF,
				
				//m_ZeroByPass,
				//m_WeightedPred
				;

	void LoadDefault();
	int LoadConfig();
	int SaveConfig();
//	void ProfileCheck(_M4M3EncoderInfo *pM4M3EncoderInfo);
};

#endif
