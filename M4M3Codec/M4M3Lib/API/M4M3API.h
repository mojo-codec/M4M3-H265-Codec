/*****************************************************************
	H.265 software codec
    reference document ITU-T H.265
	**H.265 M4M3 codec API	**
	developed by mojo
	Copyright (c)2002-2015 mojo,all rights reserved
******************************************************************/
#ifndef _M4M3API_H
#define _M4M3API_H

#include "H265Info.h"
#include "ColorSpaceType.h"

#ifdef M4M3API_IMPORT
	#define M4M3API _declspec(dllimport)
#else
	#define M4M3API
#endif

#ifdef __cplusplus
extern "C"
{
#endif
	typedef void* CodecHanle;

/**************************************************
 *	@brief Get x86 SIMD Flag

 *	@return SIMD flag,	flag:bit0	MMX
 							:bit1	SSE
 							:bit2	SSE2
 							:bit3	SSE3
							:bit4	SSSE3
 							:bit5	SSE4.1
							:bit6	SSE4.2
							:bit7	AVX
							:bit8	AVX2
							:bit9
 *	@author mojo					          *
***************************************************/
M4M3API int Getx86SIMDFlag();

/////////////////////////////////////Decoder API
/**********************************************************************
 *	@brief create M4M3 decoder handle,MAXCODEC decoder handlers can be created
 *	@param CM4M3DecPar *pPar		(i) decoder par		  *
 *	@return	decoder handle
 *	@author mojo						          *
***********************************************************************/
M4M3API CodecHanle CreateM4M3Decoder(M4M3DecPar *pPar);

/**********************************************************************
 *	@brief Destroy M4M3 decoder handle	
 *	@param void *pM4M3Decoder		(i) decoder handle		  *
 *	@return	status
 *	@author mojo						          *
***********************************************************************/
M4M3API int DestroyM4M3Decoder(CodecHanle pM4M3Decoder);

/**********************************************************************
 *	@brief decode one frame 
 *	@param void *pM4M3Decoder		(i) decoder handle		
 *	@param	char *pCompressData,		(i)one frame H264 Compress Data
 *	@param	int_t CompressDataSize,		(i)Compress Data size in byte
 *	@param	VideoOutPar *pVideoOutPar	(i/o)video out parameter
 *	@param	int SkipDecodedBPicFlag		(i)0:not skip any decoded picture,1:skip B decoded picture*
 *	@return	status
 *	@author mojo						          *
***********************************************************************/
M4M3API int M4M3Decode(CodecHanle pM4M3Decoder,unsigned char *pCompressData,int CompressDataSize,VideoOutPar *pVideoOutPar,int SkipDecodedBPicFlag=0);

/*!*************************************************
*	@brief seek to pCompressData end frame
*	@param void *pM4M3Decoder,			(i)decoder handle
*	@param char *pCompressData,			(i)one frame H264 Compress Data
*	@param int CompressDataSize,		(i)Compress Data size in byte
*	@param VideoOutPar *pVideoOutPar	(i/o)video out parameter
*	@param int SkipDecodedBPicFlag		(i)0:not skip any decoded picture,1:skip B decoded picture
*	@return return decode result	if success return >=0,else return <0
*	@author mojo					          *
***************************************************/
M4M3API int M4M3DecSeek(CodecHanle pM4M3Decoder,unsigned char *pCompressData,int CompressDataSize,VideoOutPar *pVideoOutPar,int SkipDecodedBPicFlag=0);
/*!*******************************************************************
*	@brief get M4M3 decoder information										*
*	@param void *pM4M3Decoder				  (i) decoder handle
*	@param M4M3DecoderInfo *pM4M3DecoderInfo (o) M4M3 deocder information
*	@author mojo												*
*********************************************************************/
M4M3API int GetDecoderInfo(CodecHanle pM4M3Decoder,M4M3DecoderInfo *pM4M3DecoderInfo);

/**************************************************
*	@brief set auto skip inloop filter flag		         		  *
*	@param void *pM4M3Decoder,		(i)decoder handle
*	@param int flag					(i)Auto Skip Inloop filter Flag
*	@return decode result			if success return >=0,else return <0
*	@author  mojo					          *
***************************************************/
M4M3API int SetAutoSkipInloopFilterFlag(CodecHanle pM4M3Decoder,int flag);

/************************************************************
*	@brief read one Parameter Set NAL
*	@brief uint8 *pParameterSetNAL		(i) Parameter Set NAL
*	@brief int_t len					(i) Parameter Set NAL len
*	@return decode result	if success return >=0,else return <0
*	@author mojo					          *
*************************************************************/
M4M3API int ReadParameterSetNAL(void* pM4M3Decoder,unsigned char *pParameterSetNAL,int len);

/************************************************************
 *	@brief clear Segment NAL buffer
 *
 *	@author mojo					          *
*************************************************************/
M4M3API int ClearSegmentNAL(CodecHanle pM4M3Decoder);

/**********************************************************************
 *	@brief Destroy M4M3 decoder handle	
 *	@param void *pM4M3Decoder					(i) decoder handle	
 *	@param	H265StreamInfo *pH265StreamInfo		(o)  H.265 bitstream information
 *	@return	status
 *	@author mojo						          *
***********************************************************************/
M4M3API int GetH265StreamInfo(CodecHanle pM4M3Decoder,H265StreamInfo *pH265StreamInfo);

//////////////////////////////////////////////////////////Encoder API
/**********************************************************************
 *	@brief create M4M3 encoder handle,MAXCODEC encoder handlers can be created
 *	@param CM4M3DecPar *pPar		(i) decoder par		  *
 *	@return	decoder handle
 *	@author mojo						          *
***********************************************************************/
M4M3API CodecHanle CreateM4M3Encoder(M4M3EncPar *pPar);

/**********************************************************************
 *	@brief Destroy M4M3 encoder handle	
 *	@param void *pM4M3Decoder		(i) encoder handle		  *
 *	@return	status
 *	@author mojo						          *
***********************************************************************/
M4M3API int DestroyM4M3Encoder(CodecHanle pM4M3Encoder);

/**************************************************
*	@brief encode one frame 	
*	@param void *pM4M3Encoder,			(i)encode handle
*	@param unsigned char *pImageData	(i)raw image data
*	@param const int_t WidthStride		(i)video frame width stride
*	@param char *pCompressData,			(o)one frame H264 Compress Data
*	@param int_t CompressDataSize,		(i/o)Max Compress Data buffer size in byte/used Compress Data buffer size in byte
*	@param bool &bKeyFrame				(o) this frame is key frame or not
*	@return encode result	if success return >=0,else return <0
*	@author mojo					          *
***************************************************/
M4M3API int M4M3Encode(CodecHanle pM4M3Encoder,unsigned char *pImageData,const int WidthStride,unsigned char *pCompressData,int &CompressDataSize,bool &bKeyFrame,VideoOutPar *pVideoOutPar);

/**************************************************
*	@brief set encoder parameter
*	@param M4M3EncPar *pEncPar			(i/o)encoder parameter
*	@param int_t width					(i)video width
*	@param int_t height					(i)video height
*	@param int_t SrcVideoColorSpace		(i)1:CS_YUV420_88 for YUV 4:2:0 8bit, 2:CS_BGR24 for BGR 24bit, 3:CS_BGR32 for BGR 32bit
*	@param int_t Performance			(i)1:QUALITY,2:NORMAL,4:SPEED
*	@param int_t chroma_format_idc		(i)0 for 4:0:0, 1 for 4:2:0, 2for 4:2:2, 3for 4:4:4
*	@param int_t BitDepth				(i)video bit depth
*	@param int_t BFrame					(i)B frame number 0~2
*	@param int_t RateControl			(i)0:constant Qp,1:constant bit rate,2:constant quality
*	@param int_t RateControlPar			(i)Qp for constant Qp,kbit/s for constant bit rate,quality(1~100) for constant quality
*	@param float32 FrameRate			(i)frame rate
*	@param int_t Deblock				(i)0:disable deblocking filter,1:enable deblocking filter
*	@param int_t SAO					(i)1:enable sample adaptive offset
*	@param int_t QM						(i)quantization matrix,0:disable,1:H.265 default matrix,2:Low pass,3:Low bit,4:animation
*	@param ,bool bInterlace				(i) video is interlace or not
*	@param bool SrcVideoVFlip			(i) source video flip or not,for BGR only
*	@param int_t ThreadNum				(i)thread number
*	@param int_t CPUFlag				(i)0:auto detector,SSE2CPU=(1<<3),AVXCPU=(1<<8),AVX2CPU	=(1<<9)
*	@return encode result	if success return >=0,else return <0
*	@author mojo					          *
***************************************************/
M4M3API int SetM4M3EncPar(M4M3EncPar *pEncPar,int width,int height,int SrcVideoColorSpace=CS_YUV420_88,
					int Performance=2,int chroma_format_idc=1,int BitDepth=8,int BFrame=2,int RateControl=2,int RateControlPar=80,float FrameRate=29.97f,int Deblock=1,int SAO=1,int QM=0,bool bInterlace=false,bool SrcVideoVFlip=false,int ThreadNum=0,int CPUFlag=0);

/*!*******************************************************************
*	@brief get M4M3 encoder information										*
*	@param void *pM4M3Decoder				  (i) encoder handle
*	@param M4M3DecoderInfo *pM4M3DecoderInfo (o) M4M3 encoder information
*	@author mojo												*
*********************************************************************/
M4M3API int GetEncoderInfo(CodecHanle pM4M3Encoder,M4M3EncoderInfo *pM4M3EncoderInfo);

/**************************************************
*	@brief Get color space	
*	@param	int chroma_format_idc		(i) chroma format idc
*	@param	int BitDepthY				(i)luma bit depth
*	@param	int BitDepthC				(i)chroma bitdepth
*	@return color space	
*	@author mojo					          *
***************************************************/
M4M3API int Get_ColorSpace(const int chroma_format_idc,const int BitDepthY,const int BitDepthC);

/**************************************************
*	@brief	Get_Chroma Format Idc Form ColorSpace	
*	@param	const _int s			(i) color space
*	@return chroma idc
*	@author mojo					          *
***************************************************/
M4M3API int Get_ChromaFormatIdcFormColorSpace(const int s);

/**************************************************
*	@brief	Bit depth Y Form ColorSpace		
*	@param	const _int s			(i) color space
*	@return Bit depth Y 
*	@author mojo					          *
***************************************************/
M4M3API int Get_BitDepthYFormColorSpace(const int s);

#ifdef __cplusplus
}
#endif

#endif
