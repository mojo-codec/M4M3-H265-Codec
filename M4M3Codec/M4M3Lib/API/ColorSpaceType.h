/*!*****************************************************************
@file
	H.264/H.265  software codec
	**H.264/H.265 color space type define**
	developed by mojo
@author
	Copyright (c)2002-2015 mojo,all rights reserved
*******************************************************************/
#ifndef _ColorSpaceType_h
#define _ColorSpaceType_h

//!  color space type
typedef enum _ColorSpaceType
{
	CS_UnSupportSpace=0,
	CS_NotCovSpace,

	CS_YUV420_88,		//!<YUV 4:2:0 Luma 8bit chroma 8bit
	CS_YUV420_1010,		//!<YUV 4:2:0 Luma 10bit chroma 10bit
	CS_YUV420_1212,		//!<YUV 4:2:0 Luma 12bit chroma 12bit
	CS_YUV420_1414,		//!<YUV 4:2:0 Luma 12bit chroma 14bit

	CS_YVU420_88,		//!<YVU 4:2:0 Luma 8bit chroma 8bit
	CS_YVU420_1010,		//!<YVU 4:2:0 Luma 10bit chroma 10bit
	CS_YVU420_1212,		//!<YVU 4:2:0 Luma 12bit chroma 12bit
	CS_YVU420_1414,		//!<YVU 4:2:0 Luma 12bit chroma 14bit

	CS_YUV422_88,		//!<YUV 4:2:2 Luma 8bit chroma 8bit
	CS_YUV422_1010,		//!<YUV 4:2:2 Luma 10bit chroma 10bit
	CS_YUV422_1212,		//!<YUV 4:2:2 Luma 12bit chroma 12bit
	CS_YUV422_1414,		//!<YUV 4:2:2 Luma 12bit chroma 14bit

	CS_YVU422_88,		//!<YUV 4:2:2 Luma 8bit chroma 8bit			//2011/05/04
	CS_YVU422_1010,		//!<YUV 4:2:2 Luma 10bit chroma 10bit
	CS_YVU422_1212,		//!<YUV 4:2:2 Luma 12bit chroma 12bit
	CS_YVU422_1414,		//!<YUV 4:2:2 Luma 12bit chroma 14bit

	CS_YUV444_88,		//!<YUV 4:4:4 Luma 8bit chroma 8bit
	CS_YUV444_1010,		//!<YUV 4:4:4 Luma 10bit chroma 10bit
	CS_YUV444_1212,		//!<YUV 4:4:4 Luma 12bit chroma 12bit
	CS_YUV444_1414,		//!<YUV 4:4:4 Luma 12bit chroma 14bit

	CS_YVU444_88,		//!<YUV 4:4:4 Luma 8bit chroma 8bit		//2011/05/04
	CS_YVU444_1010,		//!<YUV 4:4:4 Luma 10bit chroma 10bit
	CS_YVU444_1212,		//!<YUV 4:4:4 Luma 12bit chroma 12bit
	CS_YVU444_1414,		//!<YUV 4:4:4 Luma 12bit chroma 14bit

	CS_YUV400_88,		//!<Luma only 8bit 
	CS_YUV400_1010,		//!<Luma only 10bit 
	CS_YUV400_1212,		//!<Luma only 12bit 
	CS_YUV400_1414,		//!<Luma only 14bit 

	CS_YUY2,			//!<Y0U0Y1V0 Y2U1Y3V1


	CS_BGR24,			//!<BGR 8bit,8bit,8bit
	CS_BGR32,			//!<BGRA 8bit,8bit,8bit,8 bit
	CS_RGB565,			//!<RGB 5bit,6bit,5bit
	CS_RGB555,			//!<RGB 5bit,5bit,5bit

	CS_GBR_88,			//!<GBR 4:4:4 all 8bit   for VUI matrix_coefficients is 0
	CS_GBR_1010,		//!<GBR 4:4:4 all 10bit  for VUI matrix_coefficients is 0
	CS_GBR_1212,		//!<GBR 4:4:4 all 12bit  for VUI matrix_coefficients is 0
	CS_GBR_1414,		//!<GBR 4:4:4 all 14bit  for VUI matrix_coefficients is 0

	CS_NV12,

	TotalColorSpace
}ColorSpaceType;

//!  video out parameter
typedef struct _VideoOutPar
{
	bool m_bflip;								//!<video frame flip or not

	unsigned char	*m_pVideoOutBuffer;			//!<video out frame buffer
	int				m_VideoOutBufferSizeInbyte;	//!<video out frame buffer size
	int				m_VideoWidthStride;			//!<video width stride
	int				m_ChromaWidthStride;		//!<chroma width stride
	int				m_VideoWidth;				//!<video width
	int				m_VideoHeight;				//!<video height

	ColorSpaceType	m_ForceColorSpace;			//!<out color space

}VideoOutPar;


#endif
