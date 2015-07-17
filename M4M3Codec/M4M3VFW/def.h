/****************************************************
	H.265 decoder software codec
    reference document ITU-T H.265
	**H.265 video for win define**
	developed by mojo
	Copyright (c)2002-2015 mojo,all rights reserved
****************************************************/
#ifndef _def_h
#define _def_h

#define DLLExport _declspec(dllexport)

#define GOP_III						0
#define GOP_IPP						(1<<8)
#define GOP_IBP						(GOP_IPP+1)	//IBP=GOP_IPP+1,IBBP=GOP_IBP+2
#define GOP_IBBP					(GOP_IPP+2)
//#define GOP_IBBBP					(GOP_IPP+3)

enum H265Profile
{
	eMainProfile=1,
	eMain10Profile=2,
	eMainStillPictureProfile=3
};

//#define codec_ver "ver 1.12 RC5"
//#define build_num "build number RC041231"
#define mail_addr  "mojo@pchome.com.tw"

#define FOURCC_M4M3 mmioFOURCC('M','4','M','3')

#define FOURCC_HEVC mmioFOURCC('H','E','V','C')
#define FOURCC_hevc mmioFOURCC('h','e','v','c')
#define FOURCC_X265 mmioFOURCC('X','2','6','5')
#define FOURCC_x265 mmioFOURCC('x','2','6','5')
#define FOURCC_H265 mmioFOURCC('H','2','6','5')
#define FOURCC_h265 mmioFOURCC('h','2','6','5')

#define M4M3_NAME L"M4M3"
#ifdef __X64__
//	#define M4M2_Description L"M4M2 H.264 codec (64bit)"
	#define M4M3_Description L"M4M3 H.265 codec (64bit)"
#else
//	#define M4M2_Description L"M4M2 H.264 codec"
	#define M4M3_Description L"M4M3 H.265 codec"
#endif




#define NULL_M4M_CS  -1 /* decoder only: dont output anything 

/* yuyu		4:2:2 16bit, y-u-y-v, packed*/
#define FOURCC_YUYV	mmioFOURCC('Y','U','Y','V')
#define FOURCC_YUY2	mmioFOURCC('Y','U','Y','2')
/* yvyu		4:2:2 16bit, y-v-y-u, packed*/
#define FOURCC_YVYU	mmioFOURCC('Y','V','Y','U')
/* uyvy		4:2:2 16bit, u-y-v-y, packed */
#define FOURCC_UYVY	mmioFOURCC('U','Y','V','Y')
/* i420		y-u-v, planar */
#define FOURCC_I420	mmioFOURCC('I','4','2','0')
#define FOURCC_IYUV	mmioFOURCC('I','Y','U','V')
/* yv12		y-v-u, planar */
#define FOURCC_YV12	mmioFOURCC('Y','V','1','2')
/* yv16		y-v-u,4:2:2 planar */
#define FOURCC_YV16	mmioFOURCC('Y','V','1','6')
/* yv24		y-v-u,4:4:4 planar */
#define FOURCC_YV24	mmioFOURCC('Y','V','2','4')

#endif
