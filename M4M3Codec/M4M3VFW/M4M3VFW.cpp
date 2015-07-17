/****************************************************
	M4M3 H.265 VFW software codec
	developed by mojo
	Copyright (c)2002-2015 mojo,all rights reserved
****************************************************/
#include <thread>
#include <windows.h>
#include "M4M3VFW.h"
#include "config.h"

#include "DevProc.h"
#include "language.h"
#include "resource.h"


CM4M3VFWCodec::CM4M3VFWCodec()
{

	m_FourCC = FOURCC_HEVC;
	m_pEncoderHanle=nullptr;
	m_pDecoderHanle=nullptr;
	m_hInst = NULL;
	m_hConfigDlg = NULL;

	m_pVFWConfig=new CVFWConfig;
	m_pTmpVFWConfig=new CVFWConfig;
	
	CodecHanle	pEncoderHanle;
	M4M3EncPar	EncPar;
	SetM4M3EncPar(	&EncPar,720,480);
	EncPar.m_ProcessNum=1;
	pEncoderHanle=CreateM4M3Encoder(&EncPar);
	memset(&m_EncoderInfo,0,sizeof(M4M3EncoderInfo));
	if(pEncoderHanle)
	{
		GetEncoderInfo(pEncoderHanle,&m_EncoderInfo);
		DestroyM4M3Encoder(pEncoderHanle);
	}
	CodecHanle	pDecoderHanle;
	M4M3DecPar	DecPar;
	DecPar.m_AutoSkipDeblockingFlag=0;
	DecPar.m_CPUFlag=0;
	DecPar.m_ThreadNum=1;
	pDecoderHanle=CreateM4M3Decoder(&DecPar);
	memset(&m_DecoderInfo,0,sizeof(M4M3DecoderInfo));
	if(pDecoderHanle)
	{
		GetDecoderInfo(pDecoderHanle,&m_DecoderInfo);
		DestroyM4M3Decoder(pDecoderHanle);
	}

	memset(&m_VideoOutPar,0,sizeof(m_VideoOutPar));

#ifdef BS_DUMP
	m_outfile=fopen("c:\\Temp\\dump.265","wb");
#endif
}
CM4M3VFWCodec::~CM4M3VFWCodec()
{
#ifdef BS_DUMP
	if(m_outfile)
		fclose(m_outfile);
	m_outfile=NULL;
#endif
	delete m_pVFWConfig;
	delete m_pTmpVFWConfig;

//	delete []m_VideoOutPar.m_pVideoOutBuffer;

	DestroyCodec();
}


bool CM4M3VFWCodec::bSupportFourCC(int FourCC)
{
	return	FourCC==FOURCC_M4M3||
			FourCC==FOURCC_X265||
			FourCC==FOURCC_x265||
			FourCC==FOURCC_H265||
			FourCC==FOURCC_h265||
			FourCC==FOURCC_HEVC||
			FourCC==FOURCC_hevc;
}
/*!*************************************************
*	@brief get compressed bitstream color space
*	@param CVFWConfig	*pVFWConfig (i) config
*	@author mojo					           
****************************************************/
int CM4M3VFWCodec::Get_ColorSpace(CVFWConfig	*pVFWConfig)
{
	static const int colorspace[5]={CS_YUV400_88,CS_YUV420_88,CS_YUV422_88,CS_YUV444_88,CS_YUV444_88};
	int coutcolorspace=CS_YUV420_88;
//	int coutcolorspace=colorspace[m_pVFWConfig->m_ColorSpace.m_SelectValue];
//	coutcolorspace+=(m_pVFWConfig->m_BitDepth.m_Value[m_pVFWConfig->m_BitDepth.m_SelectValue]-8)>>1;
	return coutcolorspace;
}
/*!*************************************************
*	@brief get compressed bitstream bit count of one pixel
*	@param CVFWConfig	*pVFWConfig (i) config
*	@author mojo					           
****************************************************/
int CM4M3VFWCodec::Get_PixelBitCount(CVFWConfig	*pVFWConfig)
{
	int biBitCount=8;
/*	int biBitCount=m_pVFWConfig->m_BitDepth.m_Value[m_pVFWConfig->m_BitDepth.m_SelectValue];
	switch(m_pVFWConfig->m_ColorSpace.m_SelectValue)
	{
	case 0:
		break;
	case 1:
		biBitCount=biBitCount*3>>1;
		break;
	case 2:
		biBitCount<<=1;
		break;
	case 3:
	case 4:
		biBitCount*=3;
		break;
	}
	*/
	return biBitCount;
	
}
/*!*************************************************
*	@brief Decompress Query 

*	@author mojo					     
***************************************************/
LRESULT CM4M3VFWCodec::DecompressQuery(BITMAPINFO *lpbiInput, BITMAPINFO *lpbiOutput)
{
	BITMAPINFOHEADER * inBMPhdr = &lpbiInput->bmiHeader;
	BITMAPINFOHEADER * outBMPhdr = &lpbiOutput->bmiHeader;
	if (lpbiInput == NULL) 
		return ICERR_ERROR;


	if(	!bSupportFourCC(inBMPhdr->biCompression)
		||inBMPhdr->biWidth%2
		||inBMPhdr->biHeight%2)
		return ICERR_BADFORMAT;


	if (lpbiOutput == NULL) 
		return ICERR_OK;

	//	if (inBMPhdr->biWidth != outBMPhdr->biWidth ||
	//		inBMPhdr->biHeight != outBMPhdr->biHeight ||
	//		DecompressColorspace(outBMPhdr) == NULL_M4M_CS) 

	if (DecompressColorspace(inBMPhdr,outBMPhdr) == NULL_M4M_CS)
		return ICERR_BADFORMAT;

	return ICERR_OK;
}

/*!*************************************************
*	@brief check decompress color space 
	@return >=0 support, <0 not support
*	@author mojo					     
***************************************************/
int	CM4M3VFWCodec::DecompressColorspace(BITMAPINFOHEADER * inhdr,BITMAPINFOHEADER * outhdr)
{
	int BitstrreamColorSpace=CS_YUV420_88;
	if (bSupportFourCC(inhdr->biCompression) && inhdr->biClrImportant>CS_NotCovSpace)
		BitstrreamColorSpace=inhdr->biClrImportant;

	int res=NULL_M4M_CS;
	switch(outhdr->biCompression)
	{
	case BI_RGB:
		{
			if (outhdr->biBitCount == 24) 
			{
				m_VideoOutPar.m_ForceColorSpace=CS_BGR24;
				if(m_DecoderInfo.m_SupportColorSpaceConvert[BitstrreamColorSpace][m_VideoOutPar.m_ForceColorSpace])
					res=1;
			}	
			else if(outhdr->biBitCount == 32)
			{
				m_VideoOutPar.m_ForceColorSpace=CS_BGR32;
				if(m_DecoderInfo.m_SupportColorSpaceConvert[BitstrreamColorSpace][m_VideoOutPar.m_ForceColorSpace])
					res=1;
			}
		/*	else if(hdr->biBitCount == 16)
			{
				m_VideoOutPar.m_ForceColorSpace=CS_RGB565;
				return 1;
			}*/
			else
			{
				m_VideoOutPar.m_ForceColorSpace=CS_NotCovSpace;
				return NULL_M4M_CS;
			}

		}
		break;
	case FOURCC_I420 :
	case FOURCC_IYUV :
		m_VideoOutPar.m_ForceColorSpace=CS_YUV420_88;
		if(m_DecoderInfo.m_SupportColorSpaceConvert[BitstrreamColorSpace][m_VideoOutPar.m_ForceColorSpace])
			res=1;
		break;
	case FOURCC_YUYV:
	case FOURCC_YUY2:
		m_VideoOutPar.m_ForceColorSpace=CS_YUY2;
		if(m_DecoderInfo.m_SupportColorSpaceConvert[BitstrreamColorSpace][m_VideoOutPar.m_ForceColorSpace])
			res=1;
		break;
	case FOURCC_YVYU:
	case FOURCC_UYVY:
		m_VideoOutPar.m_ForceColorSpace=CS_NotCovSpace;
		return NULL_M4M_CS;
		break;
	case FOURCC_YV12:
		m_VideoOutPar.m_ForceColorSpace=CS_YVU420_88;
		if(m_DecoderInfo.m_SupportColorSpaceConvert[BitstrreamColorSpace][m_VideoOutPar.m_ForceColorSpace])
			res=1;
		break;
	case FOURCC_YV16:
		m_VideoOutPar.m_ForceColorSpace=CS_YVU422_88;
		if(m_DecoderInfo.m_SupportColorSpaceConvert[BitstrreamColorSpace][m_VideoOutPar.m_ForceColorSpace])
			res=1;
		break;
	case FOURCC_YV24:
		m_VideoOutPar.m_ForceColorSpace=CS_YVU444_88;
		if(m_DecoderInfo.m_SupportColorSpaceConvert[BitstrreamColorSpace][m_VideoOutPar.m_ForceColorSpace])
			res=1;
		break;
	case BI_BITFIELDS :
		{
			BITMAPV4HEADER * hdr4 = (BITMAPV4HEADER *)outhdr;

			if (hdr4->bV4BitCount == 16 &&
				hdr4->bV4RedMask == 0x7c00 &&
				hdr4->bV4GreenMask == 0x3e0 &&
				hdr4->bV4BlueMask == 0x1f)
			{
				m_VideoOutPar.m_ForceColorSpace=CS_RGB555;
				if(m_DecoderInfo.m_SupportColorSpaceConvert[BitstrreamColorSpace][m_VideoOutPar.m_ForceColorSpace])
					res=1;
			}
			else if(hdr4->bV4BitCount == 16 &&
				hdr4->bV4RedMask == 0xf800 &&
				hdr4->bV4GreenMask == 0x7e0 &&
				hdr4->bV4BlueMask == 0x1f)
			{
			//	m_VideoOutPar.m_ForceColorSpace=CS_RGB555;
				m_VideoOutPar.m_ForceColorSpace=CS_RGB565;
				if(m_DecoderInfo.m_SupportColorSpaceConvert[BitstrreamColorSpace][m_VideoOutPar.m_ForceColorSpace])
					res=1;
			}

		//	return NULL_M4M_CS;
		}
		break;

	default:
		return NULL_M4M_CS;
	}
	return res;
}

/*!*************************************************
*	@brief Decompress format 

*	@author  mojo					     
***************************************************/
LRESULT CM4M3VFWCodec::DecompressGetFormat(BITMAPINFO *lpbiInput, BITMAPINFO *lpbiOutput)
{
	BITMAPINFOHEADER * inBMPhdr = &lpbiInput->bmiHeader;
	BITMAPINFOHEADER * outBMPhdr = &lpbiOutput->bmiHeader;

	if (lpbiOutput == NULL) 
		return sizeof(BITMAPINFOHEADER);
	else if(DecompressColorspace(inBMPhdr,outBMPhdr)!=NULL_M4M_CS)
	{
		memcpy(outBMPhdr,inBMPhdr,sizeof(BITMAPINFOHEADER));
		return ICERR_OK;
	}
	else if(!bSupportFourCC(inBMPhdr->biCompression)
		||inBMPhdr->biWidth%2
		||inBMPhdr->biHeight%2)
	{
		return ICERR_BADFORMAT;
	}
	else
	{
	//	if(!bSupportFourCC(inBMPhdr->biCompression))
	//	{
	//		return ICERR_BADFORMAT;
	//	}
		outBMPhdr->biSize = sizeof(BITMAPINFOHEADER);
		outBMPhdr->biWidth = inBMPhdr->biWidth;
		outBMPhdr->biHeight = inBMPhdr->biHeight;
		outBMPhdr->biPlanes = 1;
		outBMPhdr->biBitCount = 24;
		outBMPhdr->biCompression = BI_RGB;	
		outBMPhdr->biSizeImage = outBMPhdr->biWidth * outBMPhdr->biHeight * outBMPhdr->biBitCount;
		outBMPhdr->biXPelsPerMeter = 0;
		outBMPhdr->biYPelsPerMeter = 0;
		outBMPhdr->biClrUsed = 0;
		outBMPhdr->biClrImportant = 0;
		return ICERR_OK;
	}
}
/*!*************************************************
*	@brief Decompress begin

*	@author mojo					     
***************************************************/
LRESULT CM4M3VFWCodec::DecompressBegin(BITMAPINFO *lpbiInput, BITMAPINFO *lpbiOutput)
{
	if(DecompressColorspace(&lpbiInput->bmiHeader,&lpbiOutput->bmiHeader)==NULL_M4M_CS)
		return ICERR_BADFORMAT;

	if(m_pDecoderHanle==NULL)
	{
		//	DestroyM4M3Decoder(m_pDecoderHanle);
		m_M4M3DecPar.m_AutoSkipDeblockingFlag=0;
		m_M4M3DecPar.m_CPUFlag=0;
		m_M4M3DecPar.m_ThreadNum=std::thread::hardware_concurrency();
		m_pDecoderHanle=CreateM4M3Decoder(&m_M4M3DecPar);

		m_VideoOutPar.m_VideoWidth=lpbiInput->bmiHeader.biWidth;
		m_VideoOutPar.m_VideoWidthStride=lpbiInput->bmiHeader.biWidth;
		m_VideoOutPar.m_VideoHeight=lpbiInput->bmiHeader.biHeight;
		m_VideoOutPar.m_ChromaWidthStride=m_VideoOutPar.m_VideoWidth>>1;
	}
	else
	{
		m_VideoOutPar.m_VideoWidth=lpbiInput->bmiHeader.biWidth;
		m_VideoOutPar.m_VideoWidthStride=lpbiInput->bmiHeader.biWidth;
		m_VideoOutPar.m_VideoHeight=lpbiInput->bmiHeader.biHeight;
		m_VideoOutPar.m_ChromaWidthStride=m_VideoOutPar.m_VideoWidth>>1;

		ClearSegmentNAL(m_pDecoderHanle);
	}

	return ICERR_OK;
}

/*!*************************************************
*	@brief Decompress frame

*	@author mojo					     
***************************************************/
LRESULT CM4M3VFWCodec::Decompress(ICDECOMPRESS * icd)
{
	if(icd->lpbiInput->biSizeImage==0)
	{
		memset(icd->lpOutput,0,icd->lpbiOutput->biSizeImage);
		return ICERR_OK;
	}

	if(icd->lpbiInput->biSizeImage==1)
		return ICERR_OK;
	m_VideoOutPar.m_bflip=icd->lpbiOutput->biHeight>0;
	m_VideoOutPar.m_pVideoOutBuffer=(unsigned char*)icd->lpOutput;
	m_VideoOutPar.m_VideoOutBufferSizeInbyte=icd->lpbiOutput->biSizeImage;

	int res=M4M3Decode(m_pDecoderHanle,(unsigned char*)icd->lpInput,icd->lpbiInput->biSizeImage,&m_VideoOutPar,0);
	if(res<0)
		memset(icd->lpOutput,0,icd->lpbiOutput->biSizeImage);
	return ICERR_OK;
}

/*!*************************************************
*	@brief Decompress frame

*	@author mojo					     
***************************************************/
LRESULT CM4M3VFWCodec::DecompressEnd()
{
	return ICERR_OK;
}


/*!*************************************************
*	@brief get compress format

*	@author implement by mojo					     
***************************************************/
LRESULT CM4M3VFWCodec::CompressGetFormat(BITMAPINFO *lpbiInput, BITMAPINFO *lpbiOutput)
{
	BITMAPINFOHEADER * inBMPhdr = &lpbiInput->bmiHeader;
	BITMAPINFOHEADER * outBMPhdr = &lpbiOutput->bmiHeader;

	if(CompressColorspace(inBMPhdr)==NULL_M4M_CS)
	{
		return ICERR_BADFORMAT;
	}
	if((inBMPhdr->biHeight%2)||(inBMPhdr->biWidth%2))
		return ICERR_BADFORMAT;
	if (lpbiOutput == NULL) 
		return sizeof(BITMAPINFOHEADER);

	memcpy(outBMPhdr,inBMPhdr,sizeof(BITMAPINFOHEADER));
	outBMPhdr->biSize=sizeof(BITMAPINFOHEADER);
	outBMPhdr->biCompression = Get_FourCC();
	outBMPhdr->biBitCount=Get_PixelBitCount(m_pVFWConfig);
	outBMPhdr->biSizeImage=CompressGetSize(lpbiOutput);
	outBMPhdr->biClrImportant=Get_ColorSpace(m_pVFWConfig);

	return ICERR_OK;
}

/*!*************************************************
*	@brief get compress color space

*	@author mojo					     
***************************************************/
int CM4M3VFWCodec::CompressColorspace(BITMAPINFOHEADER * hdr)
{
	int coutcolorspace=Get_ColorSpace(m_pVFWConfig);
	int res=NULL_M4M_CS;
	switch(hdr->biCompression)
	{

	case BI_RGB:
		if (hdr->biBitCount == 24 && m_EncoderInfo.m_SupportColorSpaceConvert[CS_BGR24][coutcolorspace]) 
		{
			m_M4M3EncPar.m_SrcVideoColorSpace=CS_BGR24;
			res=1;
		}
		else if(hdr->biBitCount ==32 && m_EncoderInfo.m_SupportColorSpaceConvert[CS_BGR32][coutcolorspace])
		{
			m_M4M3EncPar.m_SrcVideoColorSpace=CS_BGR32;
			res=1;
		}
		break;
		
	case FOURCC_I420 :
	case FOURCC_IYUV :

		if(m_EncoderInfo.m_SupportColorSpaceConvert[CS_YUV420_88][coutcolorspace])
		{
			m_M4M3EncPar.m_SrcVideoColorSpace=CS_YUV420_88;
			res=1;
		}
		break;
	case FOURCC_YV12 :
		if(m_EncoderInfo.m_SupportColorSpaceConvert[CS_YVU420_88][coutcolorspace])
		{
			m_M4M3EncPar.m_SrcVideoColorSpace=CS_YVU420_88;
			res=1;
		}
		break;
	case FOURCC_YV16 :
		if(m_EncoderInfo.m_SupportColorSpaceConvert[CS_YVU422_88][coutcolorspace])
		{
			m_M4M3EncPar.m_SrcVideoColorSpace=CS_YVU422_88;
			res=1;
		}
		break;
	case FOURCC_YV24 :
		if(m_EncoderInfo.m_SupportColorSpaceConvert[CS_YVU444_88][coutcolorspace])
		{
			m_M4M3EncPar.m_SrcVideoColorSpace=CS_YVU444_88;
			res=1;
		}
		break;

	case FOURCC_YUY2:
		if(m_EncoderInfo.m_SupportColorSpaceConvert[CS_YUY2][coutcolorspace])
		{
			m_M4M3EncPar.m_SrcVideoColorSpace=CS_YUY2;
			res=1;
		}
		break;
	default:

		return NULL_M4M_CS;
	}

	return res;
}

LRESULT CM4M3VFWCodec::CompressGetSize(BITMAPINFO *lpbiOutput)	
{	
	return (lpbiOutput->bmiHeader.biWidth*lpbiOutput->bmiHeader.biHeight*lpbiOutput->bmiHeader.biBitCount)>>3;
}
LRESULT CM4M3VFWCodec::CompressQuery(BITMAPINFO *lpbiInput, BITMAPINFO *lpbiOutput)
{
	BITMAPINFOHEADER * inBMPhdr = &lpbiInput->bmiHeader;
	BITMAPINFOHEADER * outBMPhdr = &lpbiOutput->bmiHeader;

	if(CompressColorspace(inBMPhdr)==NULL_M4M_CS)
	{
		return ICERR_BADFORMAT;
	}
	if((inBMPhdr->biHeight%2)||(inBMPhdr->biWidth%2))
		return ICERR_BADFORMAT;

	if(lpbiOutput==NULL)
		return ICERR_OK;
	if(inBMPhdr->biWidth!=outBMPhdr->biWidth||inBMPhdr->biHeight!=outBMPhdr->biHeight||
		!bSupportFourCC(outBMPhdr->biCompression))
		return ICERR_BADFORMAT;

	return ICERR_OK;
}
LRESULT CM4M3VFWCodec::CompressFramesInfo(ICCOMPRESSFRAMES * icf)
{
	m_M4M3EncPar.m_FrameRate=icf->dwRate;
	m_M4M3EncPar.m_FrameRate/=icf->dwScale;

	return ICERR_OK;
}

LRESULT CM4M3VFWCodec::CompressBegin(BITMAPINFO *lpbiInput)
{
	if(m_pEncoderHanle)
	{
		DestroyM4M3Encoder(m_pEncoderHanle);
		m_pEncoderHanle=NULL;
	}
	if(CompressColorspace(&lpbiInput->bmiHeader)==NULL_M4M_CS)
		return ICERR_BADFORMAT;

	const int CPUFlag=0;
	const int Width=lpbiInput->bmiHeader.biWidth;
	bool bflip=false;
	int Height=lpbiInput->bmiHeader.biHeight;
	if(Height < 0 )
	{
		Height=-Height;
	}
	if(m_M4M3EncPar.m_SrcVideoColorSpace==CS_BGR24||m_M4M3EncPar.m_SrcVideoColorSpace==CS_BGR32)
	{
		if(lpbiInput->bmiHeader.biHeight>0)
			bflip=true;
	}

	float framerate=m_M4M3EncPar.m_FrameRate;
	const int CPU_Flag = 0;
	const int Performance = m_pVFWConfig->m_Performance.m_Value[m_pVFWConfig->m_Performance.m_SelectValue];
	const int SrcVideoColorSpace=m_M4M3EncPar.m_SrcVideoColorSpace;
	const int RateControl=m_pVFWConfig->m_RateControl.m_Value[m_pVFWConfig->m_RateControl.m_SelectValue];
	const int RateControlPar=m_pVFWConfig->m_RateControlPar[RateControl].m_SelectValue;
	const int Threads=m_pVFWConfig->m_Threads.m_SelectValue>m_EncoderInfo.m_MaxThread?m_EncoderInfo.m_MaxThread:m_pVFWConfig->m_Threads.m_SelectValue;
	const int IDR=m_pVFWConfig->m_IDR.m_SelectValue;
	const int GOP=(m_pVFWConfig->m_GOP.m_Value[m_pVFWConfig->m_GOP.m_SelectValue])&0x7f;
	const int InterlacedVideo=m_pVFWConfig->m_InterlacedVideo.m_SelectValue;
	const int QuantizaMatrix=m_pVFWConfig->m_QuantizaMatrix.m_SelectValue;
	const int DeblockingFilter=m_pVFWConfig->m_DeblockingFilter.m_SelectValue;
	const int SAO = m_pVFWConfig->m_SAO.m_SelectValue;
	const int	chroma_format_idc=m_pVFWConfig->m_ColorSpace.m_SelectValue<4?m_pVFWConfig->m_ColorSpace.m_SelectValue:3,
				BitDepthY=m_pVFWConfig->m_BitDepth.m_Value[m_pVFWConfig->m_BitDepth.m_SelectValue],
				BitDepthC=m_pVFWConfig->m_BitDepth.m_Value[m_pVFWConfig->m_BitDepth.m_SelectValue];
	

	SetM4M3EncPar(&m_M4M3EncPar, Width, Height,SrcVideoColorSpace, Performance, chroma_format_idc, BitDepthY, GOP, RateControl, RateControlPar, framerate, DeblockingFilter, SAO, QuantizaMatrix, InterlacedVideo != 0,bflip, Threads, CPU_Flag);
	m_pEncoderHanle=CreateM4M3Encoder(&m_M4M3EncPar);

	if(m_pEncoderHanle==NULL)
		return ICERR_ERROR;

	return ICERR_OK;
}

LRESULT CM4M3VFWCodec::Compress(ICCOMPRESS * icc)
{
	bool bKeyFrame;
	int size=0x7fffffff;

	LRESULT status=ICERR_OK;
	
	int res=M4M3Encode(m_pEncoderHanle,(unsigned char*)icc->lpInput,m_M4M3EncPar.m_SrcVideoWidth,(unsigned char*)icc->lpOutput,size,bKeyFrame,NULL);

	*icc->lpdwFlags=0;
	if(res>=0)
	{
		if (size > 0)
		{
			icc->lpbiOutput->biSizeImage = size;
			icc->lpbiOutput->biCompression = Get_FourCC();
			if (bKeyFrame)
				*icc->lpdwFlags = AVIIF_KEYFRAME;
#ifdef BS_DUMP
			if(m_outfile)
				fwrite((unsigned char*)icc->lpOutput,1,size,m_outfile);
#endif
		}
		else
		{
			*icc->lpdwFlags = 0;
			((char *)icc->lpOutput)[0] = 0x7f;
			icc->lpbiOutput->biSizeImage = 1;
			icc->lpbiOutput->biCompression = mmioFOURCC('X','V','I','D');
		}
	}
	else
	{
		icc->lpbiOutput->biSizeImage=1;
		((char*)icc->lpOutput)[0] = 0x00;	//2010/06/14 padding to get media time 
	}

	return status;
}
LRESULT CM4M3VFWCodec::CompressEnd()
{
	return ICERR_OK;
}




//ICMsg function
INT_PTR CALLBACK CM4M3VFWCodec::AboutProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		{
			Get_gM4M3VFWCodec()->InitAboutDialog(hDlg, wParam, lParam);
		}
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
		}
		break;
	default:
		return 0;
	}
	return TRUE;
}
INT_PTR CALLBACK CM4M3VFWCodec::SetConfigProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		{
			Get_gM4M3VFWCodec()->InitSetConfigProc(hDlg, wParam, lParam);
		}
		break;
	case WM_NOTIFY:
		{
			INT_PTR res = Get_gM4M3VFWCodec()->ConfigProc(hDlg, msg, wParam, lParam);
			if (FALSE == res)
				res=Get_gM4M3VFWCodec()->NotifySetConfigProc(hDlg, wParam, lParam);
			return res;
		}
	case WM_COMMAND:
		{
			INT_PTR res = Get_gM4M3VFWCodec()->ConfigProc(hDlg, msg, wParam, lParam);
			if (FALSE==res)
				res=Get_gM4M3VFWCodec()->CommandSetConfigProc(hDlg, wParam, lParam);
			return res;
		}
	default:
		return 0;
	}
	return TRUE;
}
INT_PTR CALLBACK CM4M3VFWCodec::ConfigProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		{
			return TRUE;
		}
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_COMBO_PERFORMANCE:
				{
					int selectidx = (int)SendDlgItemMessage(Get_gM4M3VFWCodec()->m_hConfigDlg, IDC_COMBO_PERFORMANCE, CB_GETCURSEL, 0, 0);
					if (selectidx != Get_gM4M3VFWCodec()->m_pTmpVFWConfig->m_Performance.m_SelectValue)
					{
						Get_gM4M3VFWCodec()->m_pTmpVFWConfig->m_Performance.m_SelectValue = selectidx;
						Get_gM4M3VFWCodec()->EnableConfigTabItem(Get_gM4M3VFWCodec()->m_pTmpVFWConfig);
						Get_gM4M3VFWCodec()->LoadConfigFromConfigSetting(Get_gM4M3VFWCodec()->m_pTmpVFWConfig);
						Get_gM4M3VFWCodec()->UpdateConfigTabItem(Get_gM4M3VFWCodec()->m_pTmpVFWConfig);
					}
				}
				break;
			case IDC_COMBO_COLORSPACE:
				{
					int selectidx = (int)SendDlgItemMessage(Get_gM4M3VFWCodec()->m_hConfigDlg, IDC_COMBO_COLORSPACE, CB_GETCURSEL, 0, 0);
					if (selectidx != Get_gM4M3VFWCodec()->m_pTmpVFWConfig->m_ColorSpace.m_SelectValue)
					{
						Get_gM4M3VFWCodec()->m_pTmpVFWConfig->m_ColorSpace.m_SelectValue = selectidx;
						Get_gM4M3VFWCodec()->EnableConfigTabItem(Get_gM4M3VFWCodec()->m_pTmpVFWConfig);
						Get_gM4M3VFWCodec()->LoadConfigFromConfigSetting(Get_gM4M3VFWCodec()->m_pTmpVFWConfig);
						Get_gM4M3VFWCodec()->UpdateConfigTabItem(Get_gM4M3VFWCodec()->m_pTmpVFWConfig);
					}
				}
				break;
			case IDC_COMBO_BITDEPTH:
				{
					int selectidx = (int)SendDlgItemMessage(Get_gM4M3VFWCodec()->m_hConfigDlg, IDC_COMBO_BITDEPTH, CB_GETCURSEL, 0, 0);
					if (selectidx != Get_gM4M3VFWCodec()->m_pTmpVFWConfig->m_BitDepth.m_SelectValue)
					{
						Get_gM4M3VFWCodec()->m_pTmpVFWConfig->m_BitDepth.m_SelectValue = selectidx;
						Get_gM4M3VFWCodec()->EnableConfigTabItem(Get_gM4M3VFWCodec()->m_pTmpVFWConfig);
						Get_gM4M3VFWCodec()->LoadConfigFromConfigSetting(Get_gM4M3VFWCodec()->m_pTmpVFWConfig);
						Get_gM4M3VFWCodec()->UpdateConfigTabItem(Get_gM4M3VFWCodec()->m_pTmpVFWConfig);
					}
				}
				break;
			case IDC_COMBO_GOP:
				{
					int selectidx = (int)SendDlgItemMessage(Get_gM4M3VFWCodec()->m_hConfigDlg, IDC_COMBO_GOP, CB_GETCURSEL, 0, 0);
					if (selectidx != Get_gM4M3VFWCodec()->m_pTmpVFWConfig->m_GOP.m_SelectValue)
					{
						Get_gM4M3VFWCodec()->m_pTmpVFWConfig->m_GOP.m_SelectValue = selectidx;
						Get_gM4M3VFWCodec()->EnableConfigTabItem(Get_gM4M3VFWCodec()->m_pTmpVFWConfig);
						Get_gM4M3VFWCodec()->LoadConfigFromConfigSetting(Get_gM4M3VFWCodec()->m_pTmpVFWConfig);
						Get_gM4M3VFWCodec()->UpdateConfigTabItem(Get_gM4M3VFWCodec()->m_pTmpVFWConfig);
					}
				}
				break;
			case IDC_COMBO_RATECONTROL:
				{
					int selectidx = (int)SendDlgItemMessage(Get_gM4M3VFWCodec()->m_hConfigDlg, IDC_COMBO_RATECONTROL, CB_GETCURSEL, 0, 0);
					if (selectidx != Get_gM4M3VFWCodec()->m_pTmpVFWConfig->m_RateControl.m_SelectValue)
					{
						Get_gM4M3VFWCodec()->LoadConfigFromConfigSetting(Get_gM4M3VFWCodec()->m_pTmpVFWConfig);
					}
				}
				break;
				//RC PAR


			case IDC_CHECK_DEBLOCKING_FILTER:
				{
					ConfigItem *pConfigItem = &Get_gM4M3VFWCodec()->m_pTmpVFWConfig->m_DeblockingFilter;
					pConfigItem->m_SelectValue = (int)SendDlgItemMessage(Get_gM4M3VFWCodec()->m_hConfigDlg, IDC_CHECK_DEBLOCKING_FILTER, BM_GETCHECK, 0, 0);
					Get_gM4M3VFWCodec()->EnableConfigTabItem(Get_gM4M3VFWCodec()->m_pTmpVFWConfig);
				}
				break;
			case IDC_CHECK_SAO:
				{
					ConfigItem *pConfigItem = &Get_gM4M3VFWCodec()->m_pTmpVFWConfig->m_SAO;
					pConfigItem->m_SelectValue = (int)SendDlgItemMessage(Get_gM4M3VFWCodec()->m_hConfigDlg, IDC_CHECK_SAO, BM_GETCHECK, 0, 0);
					Get_gM4M3VFWCodec()->EnableConfigTabItem(Get_gM4M3VFWCodec()->m_pTmpVFWConfig);
				}
				break;
			case IDC_CHECK_INTERLACE_VIDEO:
				{
					ConfigItem *pConfigItem = &Get_gM4M3VFWCodec()->m_pTmpVFWConfig->m_InterlacedVideo;
					pConfigItem->m_SelectValue = (int)SendDlgItemMessage(Get_gM4M3VFWCodec()->m_hConfigDlg, IDC_CHECK_INTERLACE_VIDEO, BM_GETCHECK, 0, 0);
					Get_gM4M3VFWCodec()->EnableConfigTabItem(Get_gM4M3VFWCodec()->m_pTmpVFWConfig);
				}
				break;
			
			default:
				return FALSE;
			}
		}
		break;
	case WM_NOTIFY:
		{
			if (lParam == 0)
				return FALSE;

			LPNMUPDOWN pMUPDOWN = (LPNMUPDOWN)lParam;

			if (pMUPDOWN->hdr.code != UDN_DELTAPOS)
				return FALSE;
			switch (pMUPDOWN->hdr.idFrom)
			{
			case IDC_SPIN_IDR:
				{
					int Value = GetDlgItemInt(Get_gM4M3VFWCodec()->m_hConfigDlg, IDC_EDIT_IDR, NULL, FALSE);
					Value -= pMUPDOWN->iDelta;
					ConfigItem *pConfigItem = &Get_gM4M3VFWCodec()->m_pTmpVFWConfig->m_IDR;
					Value = CM4M3VFWCodec::Clip3(Value, pConfigItem->m_Value[0], pConfigItem->m_Value[1]);
					pConfigItem->m_SelectValue = Value;
					SetDlgItemInt(Get_gM4M3VFWCodec()->m_hConfigDlg, IDC_EDIT_IDR, Value, FALSE);
				}
				break;
			case IDC_SPIN_RCPAR:
			{
				int Value = GetDlgItemInt(Get_gM4M3VFWCodec()->m_hConfigDlg, IDC_EDIT_RCPAR, NULL, FALSE);
				Value -= pMUPDOWN->iDelta;
				ConfigItem *pConfigItem = &Get_gM4M3VFWCodec()->m_pTmpVFWConfig->m_RateControlPar[Get_gM4M3VFWCodec()->m_pTmpVFWConfig->m_RateControl.m_SelectValue];
				Value = CM4M3VFWCodec::Clip3(Value, pConfigItem->m_Value[0], pConfigItem->m_Value[1]);
				pConfigItem->m_SelectValue = Value;
				SetDlgItemInt(Get_gM4M3VFWCodec()->m_hConfigDlg, IDC_EDIT_RCPAR, Value, FALSE);
			}
			break;
			case IDC_SPIN_THREAD:
				{
					int Value = GetDlgItemInt(Get_gM4M3VFWCodec()->m_hConfigDlg, IDC_EDIT_THREAD, NULL, FALSE);
					Value -= pMUPDOWN->iDelta;
					ConfigItem *pConfigItem = &Get_gM4M3VFWCodec()->m_pTmpVFWConfig->m_Threads;
					Value = CM4M3VFWCodec::Clip3(Value, pConfigItem->m_Value[0], pConfigItem->m_Value[1]);
					pConfigItem->m_SelectValue = Value;
					SetDlgItemInt(Get_gM4M3VFWCodec()->m_hConfigDlg, IDC_EDIT_THREAD, Value, FALSE);
				}
				break;
			}
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}



/*!*************************************************
*	@brief init about dialog  		           *
*	@param HWND hDlg			(i) dialog handle
*	@param WPARAM wParam		(i)
*	@param LPARAM lParam		(i)
*	@author mojo					           *
****************************************************/
void CM4M3VFWCodec::InitAboutDialog(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	wchar_t TextBuffer[256];
	GetVerString(TextBuffer);
	SetDlgItemText(hDlg, IDC_ABOUT_VER, TextBuffer);
}
/*!*************************************************
*	@brief init configure dialog
*	@param HWND hDlg			(i) dialog handle
*	@param WPARAM wParam		(i)
*	@param LPARAM lParam		(i)
*	@author mojo
****************************************************/
void CM4M3VFWCodec::InitSetConfigProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	m_hConfigDlg=hDlg;
	UpdateConfigTabItem(m_pVFWConfig);

	*m_pTmpVFWConfig = *m_pVFWConfig;
}
/*!*************************************************
*	@brief nitify configure dialog
*	@param HWND hDlg			(i) dialog handle
*	@param WPARAM wParam		(i)
*	@param LPARAM lParam		(i)
*	@author mojo
****************************************************/
BOOL CM4M3VFWCodec::NotifySetConfigProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	NMHDR *pPar = (NMHDR*)lParam;
	return TRUE;
}
/*!*************************************************
*	@brief command configure dialog
*	@param HWND hDlg			(i) dialog handle
*	@param WPARAM wParam		(i)
*	@param LPARAM lParam		(i)
*	@author mojo
****************************************************/
BOOL CM4M3VFWCodec::CommandSetConfigProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDOK:
		*m_pVFWConfig = *m_pTmpVFWConfig;
		LoadConfigFromConfigSetting(m_pVFWConfig);
		m_pVFWConfig->SaveConfig();
		EndDialog(hDlg, LOWORD(wParam));
		break;
	case IDCANCEL:
		EndDialog(hDlg, LOWORD(wParam));
		break;

	default:
		return FALSE;
	}
	return TRUE;
}
/*!*************************************************
*	@brief update configure Tabs item
*
*	@author mojo
****************************************************/
void CM4M3VFWCodec::UpdateConfigTabItem(CVFWConfig *pVFWConfig)
{
	int i;
	static txt	Num[16][4] =
	{
		__TEXT("1"), __TEXT("2"), __TEXT("3"), __TEXT("4"), __TEXT("5"), __TEXT("6"), __TEXT("7"), __TEXT("8"),
		__TEXT("9"), __TEXT("10"), __TEXT("11"), __TEXT("12"), __TEXT("13"), __TEXT("14"), __TEXT("15"), __TEXT("16"),
	};
	static txt	GOP[3][16] =
	{
		 __TEXT("IPP..."), __TEXT("IBPBP..."), __TEXT("IBBPBBP...")
	};
	//ver
	wchar_t TextBuffer[256];
	GetVerString(TextBuffer);
	SetDlgItemText(m_hConfigDlg, IDC_CONFIG_VER, TextBuffer);

	//Performance
	SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_PERFORMANCE, CB_RESETCONTENT, 0, 0);
	for (i = 0; i <= pVFWConfig->m_Performance.m_MaxValue; i++)
		SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_PERFORMANCE, CB_ADDSTRING, 0, (LPARAM)CLanguage::GetLanguage(pVFWConfig->m_Performance.m_ValueNameIdx[i]));
	SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_PERFORMANCE, CB_SETCURSEL, (LPARAM)pVFWConfig->m_Performance.m_SelectValue, 0);

	//ColorSpace
	SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_COLORSPACE, CB_RESETCONTENT, 0, 0);
	for (i = 0; i <= pVFWConfig->m_ColorSpace.m_MaxValue; i++)
		SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_COLORSPACE, CB_ADDSTRING, 0, (LPARAM)CLanguage::GetLanguage(pVFWConfig->m_ColorSpace.m_ValueNameIdx[i]));
	SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_COLORSPACE, CB_SETCURSEL, (LPARAM)pVFWConfig->m_ColorSpace.m_SelectValue, 0);

	//BitDepth
	SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_BITDEPTH, CB_RESETCONTENT, 0, 0);
	for (i = 0; i <= pVFWConfig->m_BitDepth.m_MaxValue; i++)
		SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_BITDEPTH, CB_ADDSTRING, 0, (LPARAM)Num[pVFWConfig->m_BitDepth.m_Value[i] - 1]);
	SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_BITDEPTH, CB_SETCURSEL, (LPARAM)pVFWConfig->m_BitDepth.m_SelectValue, 0);

	//IDR
	SetDlgItemInt(m_hConfigDlg, IDC_EDIT_IDR, pVFWConfig->m_IDR.m_SelectValue, FALSE);

	////GOP
	SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_GOP, CB_RESETCONTENT, 0, 0);
	for (i = 0; i <= pVFWConfig->m_GOP.m_MaxValue; i++)
		SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_GOP, CB_ADDSTRING, 0, (LPARAM)GOP[i]);
	SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_GOP, CB_SETCURSEL, pVFWConfig->m_GOP.m_SelectValue, 0);

	//RateControl
	SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_RATECONTROL, CB_RESETCONTENT, 0, 0);
	for (i = 0; i <= pVFWConfig->m_RateControl.m_MaxValue; i++)
		SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_RATECONTROL, CB_ADDSTRING, 0, (LPARAM)CLanguage::GetLanguage(pVFWConfig->m_RateControl.m_ValueNameIdx[i]));
	SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_RATECONTROL, CB_SETCURSEL, (LPARAM)pVFWConfig->m_RateControl.m_SelectValue, 0);
	SetDlgItemText(m_hConfigDlg, IDC_RC_PAR, CLanguage::GetLanguage(pVFWConfig->m_RateControlPar[pVFWConfig->m_RateControl.m_SelectValue].m_ItemNameIdx));
	SetDlgItemInt(m_hConfigDlg, IDC_EDIT_RCPAR, pVFWConfig->m_RateControlPar[pVFWConfig->m_RateControl.m_SelectValue].m_SelectValue, FALSE);

	////DeblockingFilter
//	SetDlgItemText(m_hConfigDlg, IDC_CHECK_DEBLOCKING_FILTER, Get_Language()->GetLanguage(pVFWConfig->m_DeblockingFilter.m_ItemNameIdx));
	SendDlgItemMessage(m_hConfigDlg, IDC_CHECK_DEBLOCKING_FILTER, BM_SETCHECK, pVFWConfig->m_DeblockingFilter.m_SelectValue ? BST_CHECKED : BST_UNCHECKED, 0);

	//SAO
	SendDlgItemMessage(m_hConfigDlg, IDC_CHECK_SAO, BM_SETCHECK, pVFWConfig->m_SAO.m_SelectValue ? BST_CHECKED : BST_UNCHECKED, 0);

	//InterlacedVideo
	SendDlgItemMessage(m_hConfigDlg, IDC_CHECK_INTERLACE_VIDEO, BM_SETCHECK, pVFWConfig->m_InterlacedVideo.m_SelectValue ? BST_CHECKED : BST_UNCHECKED, 0);

	//QuantizaMatrix
	SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_QUANT_MATRIX, CB_RESETCONTENT, 0, 0);
	for (i = 0; i <= pVFWConfig->m_QuantizaMatrix.m_MaxValue; i++)
		SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_QUANT_MATRIX, CB_ADDSTRING, 0, (LPARAM)CLanguage::GetLanguage(pVFWConfig->m_QuantizaMatrix.m_ValueNameIdx[i]));
	SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_QUANT_MATRIX, CB_SETCURSEL, (LPARAM)pVFWConfig->m_QuantizaMatrix.m_SelectValue, 0);

	//Threads
	SetDlgItemInt(m_hConfigDlg, IDC_EDIT_THREAD, pVFWConfig->m_Threads.m_SelectValue, FALSE);
}

/*!*************************************************
*	@brief destroy codec

*	@author mojo
***************************************************/
void CM4M3VFWCodec::DestroyCodec()
{
	if (m_pEncoderHanle)
		DestroyM4M3Encoder(m_pEncoderHanle);
	m_pEncoderHanle = NULL;

	if (m_pDecoderHanle)
		DestroyM4M3Decoder(m_pDecoderHanle);
	m_pDecoderHanle = NULL;
}
/*!*************************************************
*	@brief enable or disable configure Tabs item
*
*	@author mojo
****************************************************/
void CM4M3VFWCodec::EnableConfigTabItem(CVFWConfig *pVFWConfig)
{
//	pVFWConfig->ProfileCheck(&m_EncoderInfo);
	////basic setting


	////advance setting
	EnableWindow(GetDlgItem(m_hConfigDlg, IDC_COMBO_COLORSPACE), pVFWConfig->m_ColorSpace.m_bEnable);
	EnableWindow(GetDlgItem(m_hConfigDlg, IDC_COMBO_BITDEPTH), pVFWConfig->m_BitDepth.m_bEnable);

}

/*!*************************************************
*	@brief load configure from configure setting dialog
*	@param CVFWConfig *pVFWConfig	(o)	config
*	@author mojo
****************************************************/
void CM4M3VFWCodec::LoadConfigFromConfigSetting(CVFWConfig *pVFWConfig)
{
	bool bUpdate = false;
	LRESULT SelectIdx;

	//RateControl
	SelectIdx = (int)SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_RATECONTROL, CB_GETCURSEL, 0, 0);
	if (SelectIdx != pVFWConfig->m_RateControl.m_SelectValue)
	{
		bUpdate = true;
		pVFWConfig->m_RateControl.m_SelectValue = (int)SelectIdx;
	}
	else
	{
		ConfigItem *pConfigItem = &pVFWConfig->m_RateControlPar[pVFWConfig->m_RateControl.m_SelectValue];
		pConfigItem->m_SelectValue = GetDlgItemInt(m_hConfigDlg, IDC_EDIT_RCPAR, NULL, FALSE);
		pConfigItem->m_SelectValue = Clip3(pConfigItem->m_SelectValue, pConfigItem->m_Value[0], pConfigItem->m_Value[1]);
	}

	//Threads
	pVFWConfig->m_Threads.m_SelectValue = GetDlgItemInt(m_hConfigDlg, IDC_EDIT_THREAD, NULL, FALSE);
	if (pVFWConfig->m_Threads.m_SelectValue<pVFWConfig->m_Threads.m_Value[0])
		pVFWConfig->m_Threads.m_SelectValue = pVFWConfig->m_Threads.m_Value[0];
	else if (pVFWConfig->m_Threads.m_SelectValue>pVFWConfig->m_Threads.m_Value[1])
		pVFWConfig->m_Threads.m_SelectValue = pVFWConfig->m_Threads.m_Value[1];

	//IDR
	pVFWConfig->m_IDR.m_SelectValue = GetDlgItemInt(m_hConfigDlg, IDC_EDIT_IDR, NULL, FALSE);

	////GOP
	pVFWConfig->m_GOP.m_SelectValue = (int)SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_GOP, CB_GETCURSEL, 0, 0);

	//InterlacedVideo
	pVFWConfig->m_InterlacedVideo.m_SelectValue = (int)SendDlgItemMessage(m_hConfigDlg, IDC_CHECK_INTERLACE_VIDEO, BM_GETCHECK, 0, 0);
	//QuantizaMatrix
	pVFWConfig->m_QuantizaMatrix.m_SelectValue = (int)SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_QUANT_MATRIX, CB_GETCURSEL, 0, 0);
	////DeblockingFilter
	pVFWConfig->m_DeblockingFilter.m_SelectValue = (int)SendDlgItemMessage(m_hConfigDlg, IDC_CHECK_DEBLOCKING_FILTER, BM_GETCHECK, 0, 0);

	//color space
	pVFWConfig->m_ColorSpace.m_SelectValue = (int)SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_COLORSPACE, CB_GETCURSEL, 0, 0);
	//Bit depth
	SelectIdx = (int)SendDlgItemMessage(m_hConfigDlg, IDC_COMBO_BITDEPTH, CB_GETCURSEL, 0, 0);
	if (SelectIdx != pVFWConfig->m_BitDepth.m_SelectValue)
	{
		bUpdate = true;
		pVFWConfig->m_BitDepth.m_SelectValue = (int)SelectIdx;
	}

	if (bUpdate)
	{
		UpdateConfigTabItem(pVFWConfig);
	}
}
/*!*************************************************
*	@brief get ver string
*	@param wchar_t TextBuffer[256] 		(o) ver string
*	@author mojo
****************************************************/
void CM4M3VFWCodec::GetVerString(wchar_t TextBuffer[256])
{
	static const wchar_t Profile[3][32] =
	{
		{ L"MainProfile " },
		{ L"Main10Profile " },
		{ L"MainStillPictureProfile " },
	};
	wchar_t *pText;
	memset(TextBuffer, 0, sizeof(wchar_t) * 256);

	wchar_t TextTmp[256];
	memset(TextTmp, 0, sizeof(TextTmp));
	for (size_t i = 0; i<strlen(m_EncoderInfo.m_Ver); i++)
	{
		TextTmp[i] = m_EncoderInfo.m_Ver[i];
	}
	wsprintf(TextBuffer, __TEXT("Core Ver:%s-P%d\n")
			__TEXT("Profile:")
			, TextTmp, m_EncoderInfo.m_MaxThread);
	
	pText = TextBuffer + wcslen(TextBuffer);
	for (int i = 0; i<3; i++)
	{
		if (m_EncoderInfo.m_bSupportProfile[i])
		{
			memcpy(pText, Profile[i], wcslen(Profile[i])*sizeof(wchar_t));
			pText += wcslen(Profile[i]);
		}
	}
}
