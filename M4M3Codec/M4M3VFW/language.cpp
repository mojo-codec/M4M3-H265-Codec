/****************************************************
	M4M3 H.265 VFW software codec
	developed by mojo
	Copyright (c)2002-2015 mojo,all rights reserved
****************************************************/
//#include "stdafx.h"

#include "language.h"

txt CLanguage::m_Language[Name_All][MaxTXT]=
{
	__TEXT("Performance"),
	__TEXT("QUALITY"),
	__TEXT("NORMAL"),
	__TEXT("SPEED"),
		
	__TEXT("Color space"),
	__TEXT("YUV 4:0:0"),
	__TEXT("YUV 4:2:0"),
	__TEXT("YUV 4:2:2"),
	__TEXT("YUV 4:4:4"),
	__TEXT("Color Plane"),
	__TEXT("Bit depth"),

	__TEXT("Max key frame interval"),
	__TEXT("GOP structure"),

	__TEXT("Rate Control"),
	__TEXT("constant Qp"),
	__TEXT("constant bitrate"),
	__TEXT("constant quality"),
	__TEXT("Qp"),
	__TEXT("Bitrate(kbits/s) "),
	__TEXT("Quality"),

	__TEXT("FrameRate"),
	__TEXT("DeblockingFilter"),
	__TEXT("SAO"),
	__TEXT("QuantizaMatrix"),
	__TEXT("InterlacedVideo"),
	__TEXT("Threads"),

	__TEXT("Disable"),
	__TEXT("H265 default"),
	__TEXT("low pass"),
	__TEXT("low bitrate"),
	__TEXT("animation"),
};


