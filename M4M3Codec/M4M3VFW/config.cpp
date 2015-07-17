/****************************************************
	M4M3 H.265 VFW software codec
	developed by mojo
	Copyright (c)2002-2015 mojo,all rights reserved
****************************************************/
#include <windows.h>
#include "config.h"
#include "language.h"

CVFWConfig::CVFWConfig()
{
	LoadDefault();
	if (LoadConfig()<0)
	{
		SaveConfig();
	}
}
CVFWConfig::~CVFWConfig()
{

}


void CVFWConfig::LoadDefault()
{
	m_Performance.m_ItemNameIdx = Name_Performance;
	m_Performance.m_MaxValue = 2;
	m_Performance.m_SelectValue = 0;
	m_Performance.m_Value[0] = 1;
	m_Performance.m_Value[1] = 2;
	m_Performance.m_Value[2] = 4;
	m_Performance.m_ValueNameIdx[0] = Name_QUALITY;
	m_Performance.m_ValueNameIdx[1] = Name_NORMA;
	m_Performance.m_ValueNameIdx[2] = Name_SPEED;
	m_Performance.m_bEnable = true;

	m_ColorSpace.m_ItemNameIdx = Name_ColorSpace;
	m_ColorSpace.m_MaxValue = 1;
	m_ColorSpace.m_SelectValue = 1;
	m_ColorSpace.m_Value[0] = 0;
	m_ColorSpace.m_Value[1] = 1;
	m_ColorSpace.m_Value[2] = 2;
	m_ColorSpace.m_Value[3] = 3;
	m_ColorSpace.m_ValueNameIdx[0] = Name_YUV400;
	m_ColorSpace.m_ValueNameIdx[1] = Name_YUV420;
	m_ColorSpace.m_ValueNameIdx[2] = Name_YUV422;
	m_ColorSpace.m_ValueNameIdx[2] = Name_YUV444;
	m_ColorSpace.m_bEnable = false;

	m_BitDepth.m_ItemNameIdx = Name_BitDepth;
	m_BitDepth.m_SelectValue = 0;
	m_BitDepth.m_Value[0] = 8;
	m_BitDepth.m_Value[1] = 10;
	m_BitDepth.m_Value[2] = 12;
	m_BitDepth.m_Value[3] = 14;
	m_BitDepth.m_MaxValue = 3;
	m_BitDepth.m_bEnable = false;

	m_IDR.m_ItemNameIdx = Name_IDR;
	m_IDR.m_MaxValue = 900;
	m_IDR.m_SelectValue = 300;
	m_IDR.m_Value[0] = 15;
	m_IDR.m_Value[1] = 900;
	m_IDR.m_bEnable = false;

	m_GOP.m_ItemNameIdx = Name_GOP;
	m_GOP.m_MaxValue = 2;
	m_GOP.m_SelectValue = 2;
	m_GOP.m_Value[0] = GOP_IPP;
	m_GOP.m_Value[1] = GOP_IBP;
	m_GOP.m_Value[2] = GOP_IBBP;
	m_GOP.m_bEnable = true;

	m_RateControl.m_ItemNameIdx = Name_RateControl;
	m_RateControl.m_MaxValue = 2;
	m_RateControl.m_SelectValue = 2;
	m_RateControl.m_Value[0] = 0;
	m_RateControl.m_Value[1] = 1;
	m_RateControl.m_Value[2] = 2;
	m_RateControl.m_ValueNameIdx[0] = Name_ConstQp;
	m_RateControl.m_ValueNameIdx[1] = Name_ConstBitrate;
	m_RateControl.m_ValueNameIdx[2] = Name_ConstQuality;
	m_RateControl.m_bEnable = true;

	m_RateControlPar[0].m_ItemNameIdx = Name_RateControlPar0;
	m_RateControlPar[0].m_MaxValue = 51;
	m_RateControlPar[0].m_SelectValue = 25;
	m_RateControlPar[0].m_Value[0] = 0;
	m_RateControlPar[0].m_Value[1] = 51;
	m_RateControlPar[0].m_bEnable = true;

	m_RateControlPar[1].m_ItemNameIdx = Name_RateControlPar1;
	m_RateControlPar[1].m_MaxValue = 20 * 1024;
	m_RateControlPar[1].m_SelectValue = 1500;
	m_RateControlPar[1].m_Value[0] = 100;
	m_RateControlPar[1].m_Value[1] = 20 * 1024;
	m_RateControlPar[1].m_bEnable = true;

	m_RateControlPar[2].m_ItemNameIdx = Name_RateControlPar2;
	m_RateControlPar[2].m_MaxValue = 100;
	m_RateControlPar[2].m_SelectValue = 80;
	m_RateControlPar[2].m_Value[0] = 0;
	m_RateControlPar[2].m_Value[1] = 100;
	m_RateControlPar[2].m_bEnable = true;

//	m_FrameRate.m_ItemNameIdx = Name_FrameRate;
//	m_FrameRate.m_MaxValue = 120;
//	m_FrameRate.m_SelectValue = 30;
//	m_FrameRate.m_Value[0] = 1;
//	m_FrameRate.m_Value[1] = 120;

	m_DeblockingFilter.m_ItemNameIdx = Name_DeblockingFilter;
	m_DeblockingFilter.m_SelectValue = 1;
	m_DeblockingFilter.m_bEnable = true;

	m_SAO.m_ItemNameIdx = Name_SAO;
	m_SAO.m_SelectValue = 1;
	m_SAO.m_bEnable = true;

	m_QuantizaMatrix.m_ItemNameIdx = Name_QuantizaMatrix;
	m_QuantizaMatrix.m_SelectValue = 0;
	m_QuantizaMatrix.m_ValueNameIdx[0] = Name_Disable;
	m_QuantizaMatrix.m_ValueNameIdx[1] = Name_H265Default;
	m_QuantizaMatrix.m_ValueNameIdx[2] = Name_LowPass;
	m_QuantizaMatrix.m_ValueNameIdx[3] = Name_LowBitrate;
	m_QuantizaMatrix.m_ValueNameIdx[4] = Name_Animation;
	m_QuantizaMatrix.m_MaxValue = 4;
	m_QuantizaMatrix.m_bEnable = true;

	m_InterlacedVideo.m_ItemNameIdx = Name_InterlacedVideo;
	m_InterlacedVideo.m_SelectValue = 0;
	m_InterlacedVideo.m_bEnable = true;


	m_Threads.m_ItemNameIdx = Name_Threads;
	m_Threads.m_MaxValue = 64;
	m_Threads.m_Value[0] = 0;
	m_Threads.m_Value[1] = 64;
	m_Threads.m_SelectValue = 1;
	m_Threads.m_bEnable = false;
}

/*!*************************************************
*	@brief Load configure
*	@return return result	if success return >=0,else return <0
*	@author mojo
***************************************************/
int CVFWConfig::LoadConfig()
{
	HKEY Key;
	DWORD size;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, __TEXT("Software\\H265\\M4M3")
		, 0, KEY_READ, &Key) == ERROR_SUCCESS)
	{
		size = sizeof(int);
		RegQueryValueEx(Key, __TEXT("RateControl"), 0, 0, (LPBYTE)&m_RateControl.m_SelectValue, &size);
		size = sizeof(int);
		RegQueryValueEx(Key, __TEXT("ConstQp"), 0, 0, (LPBYTE)&m_RateControlPar[0].m_SelectValue, &size);
		size = sizeof(int);
		RegQueryValueEx(Key, __TEXT("ConstBitrate"), 0, 0, (LPBYTE)&m_RateControlPar[1].m_SelectValue, &size);
		size = sizeof(int);
		RegQueryValueEx(Key, __TEXT("ConstQuality"), 0, 0, (LPBYTE)&m_RateControlPar[2].m_SelectValue, &size);
		size = sizeof(int);
		RegQueryValueEx(Key, __TEXT("Threads"), 0, 0, (LPBYTE)&m_Threads.m_SelectValue, &size);
		if (m_Threads.m_SelectValue<m_Threads.m_Value[0])
			m_Threads.m_SelectValue = m_Threads.m_Value[0];
		else if (m_Threads.m_SelectValue>m_Threads.m_Value[1])
			m_Threads.m_SelectValue = m_Threads.m_Value[1];

		size = sizeof(int);
		RegQueryValueEx(Key, __TEXT("IDR"), 0, 0, (LPBYTE)&m_IDR.m_SelectValue, &size);
		size = sizeof(int);
		RegQueryValueEx(Key, __TEXT("GOP"), 0, 0, (LPBYTE)&m_GOP.m_SelectValue, &size);
		size = sizeof(int);
		RegQueryValueEx(Key, __TEXT("InterlacedVideo"), 0, 0, (LPBYTE)&m_InterlacedVideo.m_SelectValue, &size);
		size = sizeof(int);
		RegQueryValueEx(Key, __TEXT("QuantizaMatrix"), 0, 0, (LPBYTE)&m_QuantizaMatrix.m_SelectValue, &size);
		size = sizeof(int);
		RegQueryValueEx(Key, __TEXT("DeblockingFilter"), 0, 0, (LPBYTE)&m_DeblockingFilter.m_SelectValue, &size);
		size = sizeof(int);
		RegQueryValueEx(Key, __TEXT("ColorSpace"), 0, 0, (LPBYTE)&m_ColorSpace.m_SelectValue, &size);
		size = sizeof(int);
		RegQueryValueEx(Key, __TEXT("BitDepth"), 0, 0, (LPBYTE)&m_BitDepth.m_SelectValue, &size);

		RegCloseKey(Key);
		return 0;
	}

	return -1;
}
/*!*************************************************
*	@brief save configure
*	@return return result	if success return >=0,else return <0
*	@author mojo
***************************************************/
int CVFWConfig::SaveConfig()
{
	HKEY Key;
	DWORD pdwDisposition;

	int size = sizeof(int);

	if (RegCreateKeyEx(
		HKEY_CURRENT_USER,
		__TEXT("Software\\H265\\M4M3"),
		0,
		__TEXT("VFW config"),
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		0,
		&Key,
		&pdwDisposition) == ERROR_SUCCESS)
	{
		RegSetValueEx(Key, __TEXT("RateControl"), 0, REG_DWORD, (LPBYTE)&m_RateControl.m_SelectValue, size);
		RegSetValueEx(Key, __TEXT("ConstQp"), 0, REG_DWORD, (LPBYTE)&m_RateControlPar[0].m_SelectValue, size);
		RegSetValueEx(Key, __TEXT("ConstBitrate"), 0, REG_DWORD, (LPBYTE)&m_RateControlPar[1].m_SelectValue, size);
		RegSetValueEx(Key, __TEXT("ConstQuality"), 0, REG_DWORD, (LPBYTE)&m_RateControlPar[2].m_SelectValue, size);
		RegSetValueEx(Key, __TEXT("Threads"), 0, REG_DWORD, (LPBYTE)&m_Threads.m_SelectValue, size);
		RegSetValueEx(Key, __TEXT("IDR"), 0, REG_DWORD, (LPBYTE)&m_IDR.m_SelectValue, size);
		RegSetValueEx(Key, __TEXT("GOP"), 0, REG_DWORD, (LPBYTE)&m_GOP.m_SelectValue, size);
		RegSetValueEx(Key, __TEXT("InterlacedVideo"), 0, REG_DWORD, (LPBYTE)&m_InterlacedVideo.m_SelectValue, size);
		RegSetValueEx(Key, __TEXT("QuantizaMatrix"), 0, REG_DWORD, (LPBYTE)&m_QuantizaMatrix.m_SelectValue, size);
		RegSetValueEx(Key, __TEXT("DeblockingFilter"), 0, REG_DWORD, (LPBYTE)&m_DeblockingFilter.m_SelectValue, size);
		RegSetValueEx(Key, __TEXT("ColorSpace"), 0, REG_DWORD, (LPBYTE)&m_ColorSpace.m_SelectValue, size);
		RegSetValueEx(Key, __TEXT("BitDepth"), 0, REG_DWORD, (LPBYTE)&m_BitDepth.m_SelectValue, size);
		RegCloseKey(Key);
		return 0;
	}
	return -1;
}
