/****************************************************
	M4M3 H.265 VFW software codec
	developed by mojo
	Copyright (c)2002-2015 mojo,all rights reserved
****************************************************/
#ifndef _language_h
#define _language_h

#define txt wchar_t
#define __TEXT(a)	L ## a
#define __strcmp	wcscmp
#define __strcpy	wcscpy_s
#define __strlen	wcslen



enum
{
	Name_Performance,
	Name_QUALITY, 
	Name_NORMA, 
	Name_SPEED,

	Name_ColorSpace,
	Name_YUV400,
	Name_YUV420,
	Name_YUV422,
	Name_YUV444,
	Name_ColorPlane,


	Name_BitDepth,
	Name_IDR,
	Name_GOP,

	Name_RateControl,
	Name_ConstQp,
	Name_ConstBitrate,
	Name_ConstQuality,
	Name_RateControlPar0,
	Name_RateControlPar1,
	Name_RateControlPar2,

	Name_FrameRate,
	Name_DeblockingFilter,
	Name_SAO,
	Name_QuantizaMatrix,
	Name_InterlacedVideo,
	Name_Threads,
	
	Name_Disable,
	Name_H265Default,
	Name_LowPass,
	Name_LowBitrate,
	Name_Animation,

	
	Name_All
};

#define MaxTXT			32






class CLanguage
{
public:
	
	static txt	*GetLanguage(int TxtIndex)					{	return m_Language[TxtIndex];}
protected:
	static txt m_Language[Name_All][MaxTXT];

};
#endif
