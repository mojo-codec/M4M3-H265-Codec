/****************************************************
	H.265 decoder software codec
    reference document ITU-T H.265
	**H.265 direct show filter**
	developed by mojo
	Copyright (c)2002-2015 mojo,all rights reserved
****************************************************/
//#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <streams.h>
#include <measure.h>
#include <initguid.h>
#include <olectl.h>
#if (1100 > _MSC_VER)
#include <olectlid.h>
#endif
#include <dvdmedia.h>

#include "../M4M3Lib/API/M4M3API.h"
#include "M4M3DS.h"

enum
{
	VideoInfo=1,
	VideoInfo2,
	MPEGVideo,
	MPEG2_VIDEO_MP4,
	MPEG2_VIDEO_NAL,

}DSVideoInfo_type;

const AMOVIESETUP_MEDIATYPE sudInputPinTypes[] =
{
	{&MEDIATYPE_Video, &CLSID_M4M3},
	{&MEDIATYPE_Video, &CLSID_M4M3_UC},
	{&MEDIATYPE_Video, &CLSID_x265},
	{&MEDIATYPE_Video, &CLSID_x265_UC},
	{&MEDIATYPE_Video, &CLSID_H265},
	{&MEDIATYPE_Video, &CLSID_H265_UC},
	{&MEDIATYPE_Video,&CLSID_HEVC},
	{&MEDIATYPE_Video,&CLSID_HEVC_UC},
/*	{&MEDIATYPE_Video,&CLSID_NAL1},
	{&MEDIATYPE_Video, &CLSID_DX50},
	{&MEDIATYPE_Video, &CLSID_DX50_UC}*/
};

const AMOVIESETUP_MEDIATYPE sudOutputPinTypes[] =
{
	{&MEDIATYPE_Video,&CLSID_MEDIASUBTYPE_IYUV},
	{&MEDIATYPE_Video,&MEDIASUBTYPE_YV12},
	
	{&MEDIATYPE_Video,&MEDIASUBTYPE_YUY2},
	{&MEDIATYPE_Video,&MEDIASUBTYPE_RGB32},
	{&MEDIATYPE_Video,&MEDIASUBTYPE_RGB24},
	{&MEDIATYPE_Video,&MEDIASUBTYPE_RGB565},
};

//const AMOVIESETUP_MEDIATYPE sudOutputPinTypes[] =
//{
//	{&MEDIATYPE_Video, &CLSID_MEDIASUBTYPE_IYUV},
//	{&MEDIATYPE_Video, &MEDIASUBTYPE_YV12},
//	{&MEDIATYPE_Video, &MEDIASUBTYPE_YUY2},
//	{&MEDIATYPE_Video,&MEDIASUBTYPE_RGB32},
//	{&MEDIATYPE_Video,&MEDIASUBTYPE_RGB24},
//	{&MEDIATYPE_Video,&MEDIASUBTYPE_RGB565},
//};

const AMOVIESETUP_PIN psudPins[] =
{
	{
		L"Input",           // String pin name
		FALSE,              // Is it rendered
		FALSE,              // Is it an output
		FALSE,              // Allowed none
		FALSE,              // Allowed many
		&CLSID_NULL,        // Connects to filter
		L"Output",          // Connects to pin
		sizeof(sudInputPinTypes) / sizeof(AMOVIESETUP_MEDIATYPE), // Number of types
		&sudInputPinTypes[0]	// The pin details
	},
	{ 
		L"Output",          // String pin name
		FALSE,              // Is it rendered
		TRUE,               // Is it an output
		FALSE,              // Allowed none
		FALSE,              // Allowed many
		&CLSID_NULL,        // Connects to filter
		L"Input",           // Connects to pin
		sizeof(sudOutputPinTypes) / sizeof(AMOVIESETUP_MEDIATYPE),	// Number of types
		&sudOutputPinTypes[0]	// The pin details
	}
};

const AMOVIESETUP_FILTER sudM4M3VDecoder =
{
	&CLSID_M4M3,			// Filter CLSID
	codec_name,	// Filter name
	MERIT_PREFERRED,		// Its merit
	sizeof(psudPins) / sizeof(AMOVIESETUP_PIN),	// Number of pins
	psudPins				// Pin details
};

CFactoryTemplate g_Templates[] =
{
	{ 
		codec_name,
		&CLSID_M4M3,
		CM4M3DS::CreateInstance,
		NULL,
		&sudM4M3VDecoder
	},
	{
		codec_name L"About",
		&CLSID_M4M3ABOUT,
		CM4M3DSAbout::CreateInstance
	}
};
int g_cTemplates = sizeof(g_Templates) / sizeof(CFactoryTemplate);

STDAPI DllRegisterServer()
{
	return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
	return AMovieDllRegisterServer2(FALSE);
}



CNALQueue::CNALQueue()
{
	m_pBuff=m_pBuffBeg_=m_pBuffEnd_=NULL;
	m_BuffSize=0;
	m_pNAL_=NULL;
	m_NALLen=0;
}
CNALQueue::~CNALQueue()
{
	if(m_pBuff)
		delete []m_pBuff;

}
/**************************************************
*	@brief push NAL packet to queue
*	@param unsigned char *pPacket		(i)NAL packet
*	@param int len						(i)NAL length
*	@author mojo					          *
***************************************************/
void CNALQueue::PushPacket(unsigned char *pPacket,int len)
{
	if(m_BuffSize-m_NALLen<len)
	{
		if(m_pBuff)
		{
			m_BuffSize+=len<<3;
			unsigned char *pbuff=new unsigned char[m_BuffSize+4+16];
			m_pBuffBeg_=((pbuff+16)-(((intptr_t)(pbuff+16))&0xf));

			memcpy(m_pBuffBeg_,m_pNAL_,m_NALLen*sizeof(unsigned char));

			delete []m_pBuff;
			m_pBuff=pbuff;
			m_pNAL_=m_pBuffBeg_;
		}
		else
		{
			m_BuffSize=len<<2;
			if(m_BuffSize<1024*1024)
				m_BuffSize=1024*1024;

			m_pBuff=new unsigned char[m_BuffSize+4+16];
			m_pBuffBeg_=((m_pBuff+16)-(((intptr_t)(m_pBuff+16))&0xf));

			m_pNAL_=m_pBuffBeg_;
			m_NALLen=0;
		}

		m_pBuffEnd_=m_pBuff+m_BuffSize;
	}
	unsigned char *pfree=m_pNAL_+m_NALLen;
	if(pfree+len>m_pBuffEnd_)
	{
		memmove(m_pBuff,m_pNAL_,m_NALLen*sizeof(unsigned char));
		m_pNAL_=m_pBuff;
		pfree=m_pNAL_+m_NALLen;
	}

	memcpy(pfree,pPacket,len*sizeof(unsigned char));
	m_NALLen+=len;
	pfree=m_pNAL_+m_NALLen;
	pfree[0]=0;
	pfree[1]=0;
	pfree[2]=1;
	pfree[3]=0;
}
void CNALQueue::AdvancedNal(int len)
{
	m_pNAL_+=len-4;
	m_NALLen-=len-4;
}

// create instance 
CUnknown * WINAPI CM4M3DS::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
	CM4M3DS *pNewObject = new CM4M3DS(punk, phr);
	if (pNewObject == NULL)
	{
		*phr = E_OUTOFMEMORY;
	}
	return pNewObject;
}

STDMETHODIMP CM4M3DS::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CheckPointer(ppv, E_POINTER);

	if (riid == IID_IM4M3decoder)
	{
		return GetInterface((IM4M3decoder *) this, ppv);
	} 
	if (riid == IID_ISpecifyPropertyPages)
	{
		return GetInterface((ISpecifyPropertyPages *) this, ppv); 
	} 
	return CVideoTransformFilter::NonDelegatingQueryInterface(riid, ppv);
}


CM4M3DS::CM4M3DS(LPUNKNOWN punk, HRESULT *phr) 
	:CVideoTransformFilter((TCHAR*) NULL/*NAME("m4m1ds")*/,(LPUNKNOWN) punk,(const IID)CLSID_M4M3),
	m_pM4M3Decoder(NULL),m_DSVideoInfo_type(0),m_pVideoOutPar(NULL),m_pSequenceHeader(NULL),m_pNALStream(NULL)
{
	m_pVideoOutPar=new VideoOutPar;

	memset(m_pVideoOutPar,0,sizeof(VideoOutPar));

	m_ForceColorSpace=0;
	m_SkipB=0;
	m_NalLengthSize=0;
	m_SequenceHeaderLen=0;
	m_NALStreamSize=0;

	m_pInMedia_=NULL;//2010/03/04
	m_AvgTimePerFrame=0;//2010/03/05

	m_CurTimeStamp=0;
	m_DecodedDelayTime=0;//2010/05/20
	m_EncodedDelayTime=0;//2010/06/14

	//2011/04/26
	CodecHanle pDecoderHanle;
	M4M3DecPar	DecPar;
	DecPar.m_AutoSkipDeblockingFlag=0;
	DecPar.m_CPUFlag=1;
	DecPar.m_ThreadNum=1;

	pDecoderHanle=CreateM4M3Decoder(&DecPar);
	memset(&m_DecoderInfo,0,sizeof(M4M3DecoderInfo));
	if(pDecoderHanle)
	{
		GetDecoderInfo(pDecoderHanle,&m_DecoderInfo);
		DestroyM4M3Decoder(pDecoderHanle);
	}
	m_M4M3ColorSpace=CS_YUV420_88;
}

CM4M3DS::~CM4M3DS()
{
	if(m_pVideoOutPar)
		delete m_pVideoOutPar;
	if(m_pSequenceHeader)
		delete []m_pSequenceHeader;
	if(m_pNALStream)
		delete []m_pNALStream;

	DestroyM4M3Decoder(m_pM4M3Decoder);
}

// check input type 
HRESULT CM4M3DS::CheckInputType(const CMediaType *pmt)
{
	DWORD	biCompression=-1;
	BITMAPINFOHEADER * hdr;

	m_NalLengthSize=0;
	m_M4M3ColorSpace=CS_YUV420_88;//2011/04/26

	if (*pmt->Type() != MEDIATYPE_Video)
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	if (*pmt->FormatType() == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER * vih = (VIDEOINFOHEADER *) pmt->Format();
		hdr = &vih->bmiHeader;
		m_DSVideoInfo_type=VideoInfo;

		m_AvgTimePerFrame=vih->AvgTimePerFrame;//2010/03/05

		//	width = vih->bmiHeader.biYPelsPerMeter*hdr->biWidth;
		//	height = vih->bmiHeader.biXPelsPerMeter*hdr->biHeight;

		//2010/03/05
		m_pVideoOutPar->m_VideoWidth = hdr->biWidth;
		m_pVideoOutPar->m_VideoHeight = hdr->biHeight;
	}
	else if (*pmt->FormatType() == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2 * vih2 = (VIDEOINFOHEADER2 *) pmt->Format();
		hdr = &vih2->bmiHeader;
		m_DSVideoInfo_type=VideoInfo2;

		m_AvgTimePerFrame=vih2->AvgTimePerFrame;//2010/03/05
		//	width = vih2->dwPictAspectRatioX;
		//	height = vih2->dwPictAspectRatioY;

		//2010/03/05
		m_pVideoOutPar->m_VideoWidth = vih2->rcTarget.right-vih2->rcTarget.left;
		m_pVideoOutPar->m_VideoHeight = vih2->rcTarget.bottom-vih2->rcTarget.top;
	}
	else if(*pmt->FormatType()==FORMAT_MPEGVideo)
	{
		MPEG1VIDEOINFO *pMPEG1VIDEOINFO=(MPEG1VIDEOINFO*)pmt->Format();
		VIDEOINFOHEADER *vih =&pMPEG1VIDEOINFO->hdr;

		hdr = &vih->bmiHeader;
		m_DSVideoInfo_type=MPEGVideo;

		m_AvgTimePerFrame=vih->AvgTimePerFrame;//2010/03/05

		//2010/03/05
		m_pVideoOutPar->m_VideoWidth = hdr->biWidth;
		m_pVideoOutPar->m_VideoHeight = hdr->biHeight;
	}
	else if(*pmt->FormatType()==FORMAT_MPEG2_VIDEO)
	{
		MPEG2VIDEOINFO *pMPEG2VIDEOINFO=(MPEG2VIDEOINFO*)pmt->Format();
		VIDEOINFOHEADER2* vih2 =&pMPEG2VIDEOINFO->hdr;

		hdr = &vih2->bmiHeader;
		m_DSVideoInfo_type=MPEG2_VIDEO_MP4;

		m_AvgTimePerFrame=vih2->AvgTimePerFrame;//2010/03/05

		//2010/03/05
		m_pVideoOutPar->m_VideoWidth = vih2->rcTarget.right-vih2->rcTarget.left;
		m_pVideoOutPar->m_VideoHeight = vih2->rcTarget.bottom-vih2->rcTarget.top;

		if(m_pVideoOutPar->m_VideoWidth==0||m_pVideoOutPar->m_VideoHeight==0)//2010/03/27
		{
			m_pVideoOutPar->m_VideoWidth = hdr->biWidth;
			m_pVideoOutPar->m_VideoHeight = hdr->biHeight;
		}
	}
	else
	{
		m_AvgTimePerFrame=0;//2010/03/05
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	if (hdr->biHeight < 0)
	{
	}

//	m_pVideoOutPar->m_VideoWidth = hdr->biWidth;	//mark 2010/03/05
//	m_pVideoOutPar->m_VideoHeight = hdr->biHeight;	//mark 2010/03/05

	if(hdr->biCompression==FOURCC_M4M3&&hdr->biClrImportant>CS_NotCovSpace)
		m_M4M3ColorSpace=static_cast<ColorSpaceType>( hdr->biClrImportant);//for M4M3 bitstream colorspace

	switch(hdr->biCompression)
	{
	case FOURCC_M4M3 :
	case FOURCC_X265 :
	case FOURCC_x265 :
	case FOURCC_H265 :
	case FOURCC_h265 :
	case FOURCC_HEVC:
	case FOURCC_hevc:
		{
			if(m_DSVideoInfo_type==MPEG2_VIDEO_MP4)//2009/06/23
			{
				MPEG2VIDEOINFO *pMPEG2VIDEOINFO=(MPEG2VIDEOINFO*)pmt->Format();

				unsigned char *pSequenceHeader=(unsigned char*)pMPEG2VIDEOINFO->dwSequenceHeader;
				m_NalLengthSize=pMPEG2VIDEOINFO->dwFlags;
				if(m_pSequenceHeader)
					delete []m_pSequenceHeader;
				m_SequenceHeaderLen=pMPEG2VIDEOINFO->cbSequenceHeader;
				m_pSequenceHeader=new unsigned char[m_SequenceHeaderLen+4];
				m_pSequenceHeader[m_SequenceHeaderLen]=
				m_pSequenceHeader[m_SequenceHeaderLen+1]=0;
				m_pSequenceHeader[m_SequenceHeaderLen+2]=1;
				m_pSequenceHeader[m_SequenceHeaderLen+3]=0;

				memcpy(m_pSequenceHeader,pMPEG2VIDEOINFO->dwSequenceHeader,m_SequenceHeaderLen*sizeof(unsigned char));
			}
		}
		break;
	case 0:
		{
			if(m_DSVideoInfo_type==MPEG2_VIDEO_MP4)//2009/06/23
			{
				if(CLSID_NAL1==*pmt->Subtype())
				{
					MPEG2VIDEOINFO *pMPEG2VIDEOINFO=(MPEG2VIDEOINFO*)pmt->Format();

					unsigned char *pSequenceHeader=(unsigned char*)pMPEG2VIDEOINFO->dwSequenceHeader;
					m_NalLengthSize=pMPEG2VIDEOINFO->dwFlags;
					if(m_pSequenceHeader)
						delete []m_pSequenceHeader;
					m_SequenceHeaderLen=pMPEG2VIDEOINFO->cbSequenceHeader;
					m_pSequenceHeader=new unsigned char[m_SequenceHeaderLen+4];
					m_pSequenceHeader[m_SequenceHeaderLen]=
						m_pSequenceHeader[m_SequenceHeaderLen+1]=0;
					m_pSequenceHeader[m_SequenceHeaderLen+2]=1;
					m_pSequenceHeader[m_SequenceHeaderLen+3]=0;

					memcpy(m_pSequenceHeader,pMPEG2VIDEOINFO->dwSequenceHeader,m_SequenceHeaderLen*sizeof(unsigned char));
					m_DSVideoInfo_type=MPEG2_VIDEO_NAL;

					return VFW_E_TYPE_NOT_ACCEPTED;//disable NAL raw stream and MPEG2 ts
					break;
				}
			}
		}
		
/*	case FOURCC_XVID :
		if(dec_other_mpeg4)
			break;*/
	default :
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	return S_OK;
}

HRESULT CM4M3DS::GetMediaType(int iPosition, CMediaType *mtOut)
{
	ColorSpaceType OutColorSpace=CS_UnSupportSpace;
	BITMAPINFOHEADER * bmih;
	if (m_pInput->IsConnected() == FALSE)
	{
		return E_UNEXPECTED;
	}

	if(m_DSVideoInfo_type==VideoInfo2||m_DSVideoInfo_type==MPEG2_VIDEO_MP4||m_DSVideoInfo_type==MPEG2_VIDEO_NAL)//2009/06/23
	{
		VIDEOINFOHEADER2 * vih = (VIDEOINFOHEADER2 *) mtOut->ReallocFormatBuffer(sizeof(VIDEOINFOHEADER2));
		if (vih == NULL)
		{
			return E_OUTOFMEMORY;
		}
		bmih = &(vih->bmiHeader);
		ZeroMemory(vih, sizeof (VIDEOINFOHEADER2));
		mtOut->SetFormatType(&FORMAT_VideoInfo2);
		vih->dwPictAspectRatioX = m_pVideoOutPar->m_VideoWidth;
		vih->dwPictAspectRatioY = m_pVideoOutPar->m_VideoHeight;
	}
	else if(m_DSVideoInfo_type==VideoInfo||m_DSVideoInfo_type==MPEGVideo)
	{
		VIDEOINFOHEADER * vih = (VIDEOINFOHEADER *) mtOut->ReallocFormatBuffer(sizeof(VIDEOINFOHEADER));
		if (vih == NULL) return E_OUTOFMEMORY;
		bmih = &(vih->bmiHeader);
		ZeroMemory(vih, sizeof (VIDEOINFOHEADER));
		mtOut->SetFormatType(&FORMAT_VideoInfo);
	}
	

	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth	= m_pVideoOutPar->m_VideoWidth;
	bmih->biHeight = m_pVideoOutPar->m_VideoHeight;
	bmih->biPlanes = 1;

	if (iPosition < 0)
	{
		return E_INVALIDARG;
	}
	//	if(force_color_space!=-1&&force_color_space!=iPosition)
	//		return VFW_S_NO_MORE_ITEMS;

	switch(iPosition)
	{
	case 0:
		if(m_ForceColorSpace==0||m_ForceColorSpace==CS_YUV420_88)
		{
			bmih->biCompression = CLSID_MEDIASUBTYPE_IYUV.Data1;
			bmih->biBitCount = 12;
			mtOut->SetSubtype(&CLSID_MEDIASUBTYPE_IYUV);
			OutColorSpace=CS_YUV420_88;//2011/04/26
			break;
		}
		else
			return VFW_S_NO_MORE_ITEMS;
	case 1:
		if(m_ForceColorSpace==0||m_ForceColorSpace==CS_YVU420_88)
		{
			bmih->biCompression = MEDIASUBTYPE_YV12.Data1;
			bmih->biBitCount = 12;
			mtOut->SetSubtype(&MEDIASUBTYPE_YV12);
			OutColorSpace=CS_YVU420_88;//2011/04/26
			break;
		}
		else
			return VFW_S_NO_MORE_ITEMS;
	case 2:
		if(m_ForceColorSpace==0||m_ForceColorSpace==CS_YUY2)
		{
			bmih->biCompression = MEDIASUBTYPE_YUY2.Data1;
			bmih->biBitCount = 16;
			mtOut->SetSubtype(&MEDIASUBTYPE_YUY2);
			OutColorSpace=CS_YUY2;//2011/04/26
			break;
		}
		else
			return VFW_S_NO_MORE_ITEMS;
	case 3:
		if(m_ForceColorSpace==0||m_ForceColorSpace==CS_BGR32)
		{
			bmih->biCompression = BI_RGB;
			bmih->biBitCount = 32;
			mtOut->SetSubtype(&MEDIASUBTYPE_RGB32);
			OutColorSpace=CS_BGR32;//2011/04/26
			break;
		}
		else
			return VFW_S_NO_MORE_ITEMS;
	case 4:
		if(m_ForceColorSpace==0||m_ForceColorSpace==CS_BGR24)
		{
			bmih->biCompression = BI_RGB;
			bmih->biBitCount = 24;	
			mtOut->SetSubtype(&MEDIASUBTYPE_RGB24);
			OutColorSpace=CS_BGR24;//2011/04/26
			break;
		}
		else
			return VFW_S_NO_MORE_ITEMS;
	case 5:
		if(m_ForceColorSpace==0||m_ForceColorSpace==CS_RGB565)
		{
			bmih->biCompression = BI_RGB;
			bmih->biBitCount = 16;	
			mtOut->SetSubtype(&MEDIASUBTYPE_RGB565);
			OutColorSpace=CS_RGB565;//2011/04/26
			break;
		}
		else
			return VFW_S_NO_MORE_ITEMS;

		/*	case 1 :
		if ( USE_YVYU )
		{
		vih->bmiHeader.biCompression = MEDIASUBTYPE_YVYU.Data1;
		vih->bmiHeader.biBitCount = 16;
		mtOut->SetSubtype(&MEDIASUBTYPE_YVYU);
		break;
		}
		case 2 :
		if ( USE_UYVY )
		{
		vih->bmiHeader.biCompression = MEDIASUBTYPE_UYVY.Data1;
		vih->bmiHeader.biBitCount = 16;
		mtOut->SetSubtype(&MEDIASUBTYPE_UYVY);
		break;
		}*/

		/*	
		case 7 :
		if ( USE_RG555 )
		{
		vih->bmiHeader.biCompression = BI_RGB;
		vih->bmiHeader.biBitCount = 16;	
		mtOut->SetSubtype(&MEDIASUBTYPE_RGB555);
		break;
		}
		case 8 :
		if ( USE_RG565 )
		{
		vih->bmiHeader.biCompression = BI_RGB;
		vih->bmiHeader.biBitCount = 16;	
		mtOut->SetSubtype(&MEDIASUBTYPE_RGB565);
		break;
		}	*/
	default :
		return VFW_S_NO_MORE_ITEMS;
	}

	bmih->biSizeImage = GetBitmapSize(bmih);

	mtOut->SetType(&MEDIATYPE_Video);
	//	mtOut->SetFormatType(&FORMAT_VideoInfo2);
	mtOut->SetTemporalCompression(FALSE);
	mtOut->SetSampleSize(bmih->biSizeImage);

	if(m_DecoderInfo.m_SupportColorSpaceConvert[m_M4M3ColorSpace][OutColorSpace])//2011/04/26
		return S_OK;
	else
		return S_FALSE;
}

HRESULT CM4M3DS::SetMediaType(PIN_DIRECTION direction, const CMediaType *pmt)
{
	if (direction == PINDIR_OUTPUT)
	{
		return ChangeColorSpace(*pmt->Subtype(), *pmt->FormatType(), pmt->Format());
	}
	return S_OK;
}
HRESULT CM4M3DS::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
	return S_OK;
}

HRESULT CM4M3DS::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *ppropInputRequest)
{
	HRESULT result;
	ALLOCATOR_PROPERTIES ppropActual;

	if (m_pInput->IsConnected() == FALSE)
	{
		return E_UNEXPECTED;
	}

	ppropInputRequest->cBuffers = 1;
	ppropInputRequest->cbBuffer = m_pVideoOutPar->m_VideoWidth * m_pVideoOutPar->m_VideoHeight * 4;
	// cbAlign causes problems with the resize filter */
	// ppropInputRequest->cbAlign = 16;	
	ppropInputRequest->cbPrefix = 0;
	result = pAlloc->SetProperties(ppropInputRequest, &ppropActual);
	if (result != S_OK)
	{
		return result;
	}
	if (ppropActual.cbBuffer < ppropInputRequest->cbBuffer)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CM4M3DS::ChangeColorSpace(GUID subtype, GUID formattype, void * format)
{
	if (formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER * vih = (VIDEOINFOHEADER * )format;
		m_pVideoOutPar->m_VideoWidthStride = vih->bmiHeader.biWidth;

		if(vih->bmiHeader.biHeight>0)
			m_pVideoOutPar->m_bflip=true;
		else
			m_pVideoOutPar->m_bflip=false;//2010/02/23
	}
	else if (formattype == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2 * vih2 = (VIDEOINFOHEADER2 * )format;
		m_pVideoOutPar->m_VideoWidthStride = vih2->bmiHeader.biWidth;

		if(vih2->bmiHeader.biHeight>0)
			m_pVideoOutPar->m_bflip=true;
		else
			m_pVideoOutPar->m_bflip=false;//2010/02/23
	}
	else
	{
		return S_FALSE;
	}

	
	if(subtype== CLSID_MEDIASUBTYPE_IYUV)
	{
//			m_pVideoOutPar->m_bflip = false;
			m_pVideoOutPar->m_ForceColorSpace=CS_YUV420_88;
			
			m_pVideoOutPar->m_VideoOutBufferSizeInbyte=(m_pVideoOutPar->m_VideoWidthStride*m_pVideoOutPar->m_VideoHeight*3)>>1;
			m_pVideoOutPar->m_ChromaWidthStride=m_pVideoOutPar->m_VideoWidthStride>>1;
	//		if(m_pVideoOutPar->m_pVideoOutBuffer!=NULL)
	//			delete []m_pVideoOutPar->m_pVideoOutBuffer;
	//		m_pVideoOutPar->m_pVideoOutBuffer=new unsigned char[m_pVideoOutPar->m_VideoOutBufferSizeInbyte];
	}
	else if(subtype== MEDIASUBTYPE_YV12)
	{
//			m_pVideoOutPar->m_bflip = false;
			m_pVideoOutPar->m_ForceColorSpace=CS_YVU420_88;

			m_pVideoOutPar->m_VideoOutBufferSizeInbyte=(m_pVideoOutPar->m_VideoWidthStride*m_pVideoOutPar->m_VideoHeight*3)>>1;
			m_pVideoOutPar->m_ChromaWidthStride=m_pVideoOutPar->m_VideoWidthStride>>1;
	//		if(m_pVideoOutPar->m_pVideoOutBuffer!=NULL)
	//			delete []m_pVideoOutPar->m_pVideoOutBuffer;
	//		m_pVideoOutPar->m_pVideoOutBuffer=new unsigned char[m_pVideoOutPar->m_VideoOutBufferSizeInbyte];

	}
	else if(subtype== MEDIASUBTYPE_YUY2)
	{
//			m_pVideoOutPar->m_bflip = false;
			m_pVideoOutPar->m_ForceColorSpace=CS_YUY2;

			m_pVideoOutPar->m_VideoOutBufferSizeInbyte=(m_pVideoOutPar->m_VideoWidthStride*m_pVideoOutPar->m_VideoHeight)<<1;
			m_pVideoOutPar->m_ChromaWidthStride=m_pVideoOutPar->m_VideoWidthStride>>1;
	//		if(m_pVideoOutPar->m_pVideoOutBuffer!=NULL)
	//			delete []m_pVideoOutPar->m_pVideoOutBuffer;
	//		m_pVideoOutPar->m_pVideoOutBuffer=new unsigned char[m_pVideoOutPar->m_VideoOutBufferSizeInbyte];

	}
	else if(subtype== MEDIASUBTYPE_RGB24)
	{
		//	m_pVideoOutPar->m_bflip = false;
			m_pVideoOutPar->m_ForceColorSpace=CS_BGR24;

			m_pVideoOutPar->m_VideoOutBufferSizeInbyte=(m_pVideoOutPar->m_VideoWidthStride*m_pVideoOutPar->m_VideoHeight*3);
			m_pVideoOutPar->m_ChromaWidthStride=m_pVideoOutPar->m_VideoWidthStride;
	//		if(m_pVideoOutPar->m_pVideoOutBuffer!=NULL)
	//			delete []m_pVideoOutPar->m_pVideoOutBuffer;
	//		m_pVideoOutPar->m_pVideoOutBuffer=new unsigned char[m_pVideoOutPar->m_VideoOutBufferSizeInbyte];

	}
	else if(subtype== MEDIASUBTYPE_RGB32)
	{
	//		m_pVideoOutPar->m_bflip = false;
			m_pVideoOutPar->m_ForceColorSpace=CS_BGR32;

			m_pVideoOutPar->m_VideoOutBufferSizeInbyte=(m_pVideoOutPar->m_VideoWidthStride*m_pVideoOutPar->m_VideoHeight)<<2;
			m_pVideoOutPar->m_ChromaWidthStride=m_pVideoOutPar->m_VideoWidthStride;
	//		if(m_pVideoOutPar->m_pVideoOutBuffer!=NULL)
	//			delete []m_pVideoOutPar->m_pVideoOutBuffer;
	//		m_pVideoOutPar->m_pVideoOutBuffer=new unsigned char[m_pVideoOutPar->m_VideoOutBufferSizeInbyte];

	}
	else if(subtype== MEDIASUBTYPE_RGB565)//2009/01/14
	{
//		m_pVideoOutPar->m_bflip = false;
		m_pVideoOutPar->m_ForceColorSpace=CS_RGB565;

		m_pVideoOutPar->m_VideoOutBufferSizeInbyte=(m_pVideoOutPar->m_VideoWidthStride*m_pVideoOutPar->m_VideoHeight)<<1;
		m_pVideoOutPar->m_ChromaWidthStride=m_pVideoOutPar->m_VideoWidthStride;
		//		if(m_pVideoOutPar->m_pVideoOutBuffer!=NULL)
		//			delete []m_pVideoOutPar->m_pVideoOutBuffer;
		//		m_pVideoOutPar->m_pVideoOutBuffer=new unsigned char[m_pVideoOutPar->m_VideoOutBufferSizeInbyte];

	}
	else
		return S_FALSE;

	if(m_DecoderInfo.m_SupportColorSpaceConvert[m_M4M3ColorSpace][m_pVideoOutPar->m_ForceColorSpace])//2011/04/26
		return S_OK;
	else
		return S_FALSE;
}

HRESULT CM4M3DS::Transform(IMediaSample *pInMedia, IMediaSample *pOutMedia)
{
	unsigned char *poutimage,*pinimage;

	if(m_pM4M3Decoder==NULL)
	{
	////////////// disable SIMD for simple Chinese
		#define SimplifiedChineseID	0x0804
		LANGID lang=GetSystemDefaultLangID();
		int CPUFlag=Getx86SIMDFlag()<<1;
		int Mask=lang^SimplifiedChineseID;
		Mask=(-Mask)>>31;
		CPUFlag&=Mask;
	////////////////////		

		M4M3DecPar DecPar;
		bool MT=true;
		SYSTEM_INFO inf;
		GetSystemInfo(&inf);

		DecPar.m_ThreadNum=inf.dwNumberOfProcessors;
		DecPar.m_CPUFlag=CPUFlag;

		m_M4M3DSConfig.LoadSetting();
		DecPar.m_AutoSkipDeblockingFlag=m_M4M3DSConfig.Get_AutoSkipDeblockingFlag();

		m_pM4M3Decoder=CreateM4M3Decoder(&DecPar);

		CM4M3DSAbout::m_pCurrM4M3Decoder=m_pM4M3Decoder;

		ParserSequenceHeader();

	//	m_DecodedDelayTime=m_AvgTimePerFrame*GetDecodedFrameDelay(m_pM4M3Decoder);//TODO
		m_DecodedDelayTime=0;
		m_EncodedDelayTime=0;
	}

	AM_MEDIA_TYPE * mtOut;
	pOutMedia->GetMediaType(&mtOut);
	if (mtOut != NULL)
	{
		HRESULT result;
		result = ChangeColorSpace(mtOut->subtype, mtOut->formattype, mtOut->pbFormat);
		DeleteMediaType(mtOut);
		if (result != S_OK)
		{
			return result;
		}
	}
	/*	int size=pOutMedia->GetSize();
	if(color_space==CS_BGR&&out_image_size!=pOutMedia->GetSize())
	{
	color_space=CS_BGRA;
	}*/
	/*040903
	decoder->stream->len = pInMedia->GetActualDataLength();
	if (pInMedia->GetPointer((BYTE**)&decoder->stream->pstream) != S_OK)
	{
	return S_FALSE;
	}
	decoder->stream->begstream1=decoder->stream->stream1=decoder->stream->pstream;
	decoder->stream->endstream1=decoder->stream->begstream1+decoder->stream->len;
	*/
	if (pInMedia->GetPointer((BYTE**)&pinimage) != S_OK)
	{
		return S_FALSE;
	}
//	decoder->SetStream(pinimage,pInMedia->GetActualDataLength());
	///2005.01.02	if input data len is 1 not deocde
/*	if(pInMedia->GetActualDataLength()==1)
	{
		//	out_image(poutimage);
		return S_OK;
	}*///mark 2010/06/14



	if (pOutMedia->GetPointer((BYTE**)&poutimage) != S_OK)
	{
		return S_FALSE; 
	}
//	int tmp=pOutMedia->GetActualDataLength();

	m_pVideoOutPar->m_VideoOutBufferSizeInbyte=pOutMedia->GetActualDataLength();
	m_pVideoOutPar->m_pVideoOutBuffer=poutimage;

	if(pInMedia->GetActualDataLength()==1&&pinimage[0]==0x00)
	{
	//	m_EncodedDelayTime+=m_AvgTimePerFrame;//2010/06/14

		REFERENCE_TIME TimeStart,TimeEnd;
		if(pInMedia->GetTime(&TimeStart,&TimeEnd)==NOERROR)
		{
			m_EncodedDelayTime=TimeEnd;
		}
	}

	if (pInMedia->IsPreroll() != S_OK)
	{
		REFERENCE_TIME TimeStart;
		REFERENCE_TIME TimeEnd;
		HRESULT	GetTimeRes=pInMedia->GetTime(&TimeStart,&TimeEnd);//2011/06/23

		if(NOERROR==GetTimeRes)
		{
			REFERENCE_TIME FrameTime=(TimeEnd-TimeStart);

			m_SkipB=0;
			if(m_itrLate >FrameTime)
				m_SkipB=1;

			//2010/05/20
		/*	if(pInMedia->GetActualDataLength()>1)
			{
				m_VideoTimeStamp.push_back(TimeStart);
				std::sort(m_VideoTimeStamp.begin(),m_VideoTimeStamp.end());
			}*/


		/*	if(m_CurTimeStamp==0)
			{
				m_CurTimeStamp=TimeStart;
			}*///mark 2010/06/05


		/*	if(pInMedia->GetActualDataLength()>1)
			{
				static FILE *pfile=NULL;
				if(pfile==NULL)
				{
					pfile=fopen("c:\\aa.txt","w");
				}
				char tmp[1024];
				sprintf(tmp,"%d\n",TimeStart);
				fwrite(tmp,strlen(tmp),1,pfile);
				fflush(pfile);
			}*/
			
		}

		unsigned char *pStream=pinimage;
		int StreamLen=pInMedia->GetActualDataLength();

		if(m_DSVideoInfo_type!=MPEG2_VIDEO_NAL)//2009/06/25
		{
			pStream=InsertH265StartCode(pStream,StreamLen);
			int res=M4M3Decode(m_pM4M3Decoder,pStream,StreamLen,m_pVideoOutPar,m_SkipB);//2009/06/23
			if(res<0)
			{
				return S_FALSE;
			}

			//2010/05/20
		/*	if(!m_VideoTimeStamp.empty())
			{
				TimeStart=m_VideoTimeStamp.front();
				m_VideoTimeStamp.pop_front();
				TimeEnd=TimeStart+m_AvgTimePerFrame;
				pOutMedia->SetTime(&TimeStart,&TimeEnd);
			}*/

			TimeStart-=m_DecodedDelayTime+m_EncodedDelayTime;
			TimeEnd-=m_DecodedDelayTime+m_EncodedDelayTime;
			pOutMedia->SetTime(&TimeStart,&TimeEnd);

		}
		else
		{
			m_pInMedia_=pInMedia;//2010/03/04
			int res=M4M3Decode(m_pM4M3Decoder,pStream,StreamLen,m_pVideoOutPar,m_SkipB);
			if(res<0 && (S_OK==GetTimeRes||StreamLen<=0) )//2011/06/23
				return S_FALSE;
			
			//2010/05/20
			TimeStart=m_CurTimeStamp;
			TimeEnd=m_CurTimeStamp+m_AvgTimePerFrame;
			pOutMedia->SetTime(&TimeStart,&TimeEnd);
			m_CurTimeStamp+=m_AvgTimePerFrame;


		/*	if(!m_VideoTimeStamp.empty())
			{
				TimeStart=m_VideoTimeStamp.front();
				m_VideoTimeStamp.pop_front();
				TimeEnd=TimeStart+m_AvgTimePerFrame;
				pOutMedia->SetTime(&TimeStart,&TimeEnd);
			}*/
		}
	}
	else//seek file
	{
		unsigned char *pStream=pinimage;
		int StreamLen=pInMedia->GetActualDataLength();
		pStream=InsertH265StartCode(pStream,StreamLen);

		int res=M4M3DecSeek(m_pM4M3Decoder,pStream,StreamLen,m_pVideoOutPar,1);//2009/06/24

		//2010/05/20
	/*	while (!m_VideoTimeStamp.empty())
		{	
			m_VideoTimeStamp.pop();
		}*/

		if(m_DSVideoInfo_type==MPEG2_VIDEO_NAL)//2010/06/15
		{
			if(res<0)	return S_OK;

			REFERENCE_TIME TimeStart,TimeEnd;
			TimeStart=m_CurTimeStamp;
			TimeEnd=m_CurTimeStamp+m_AvgTimePerFrame;
		//	pOutMedia->SetTime(&TimeStart,&TimeEnd);
			m_CurTimeStamp+=m_AvgTimePerFrame;
		}
	
	}

	return S_OK;
}

STDMETHODIMP CM4M3DS::GetPages(CAUUID *pPages)
{
	pPages->cElems = 1;
	pPages->pElems = (GUID *) CoTaskMemAlloc(sizeof(GUID));
	if (pPages->pElems == NULL) {
		return E_OUTOFMEMORY;
	}
	*(pPages->pElems) = CLSID_M4M3ABOUT;
	return S_OK;

} // GetPages

STDMETHODIMP CM4M3DS::FreePages(CAUUID * pPages)
{
	CoTaskMemFree(pPages->pElems);
	return S_OK;
}

HRESULT CM4M3DS::EndOfStream()
{
	if(m_DSVideoInfo_type!=MPEG2_VIDEO_NAL)
		return CTransformFilter::EndOfStream();
	else
	{
		HRESULT res=S_FALSE;
		REFERENCE_TIME DisplayTime=timeGetTime();
		REFERENCE_TIME frametime=m_AvgTimePerFrame/10000;

		do 
		{
			REFERENCE_TIME CurrTime=timeGetTime();
			if(CurrTime>=DisplayTime)
			{
				res=FlushBitStream(m_pInMedia_);
				DisplayTime+=frametime;
			}
			else
			{
				Sleep(DisplayTime-CurrTime);
			}
			
		} while (res==S_OK);

		//2010/05/20
	//	while (!m_VideoTimeStamp.empty())
	//	{	m_VideoTimeStamp.pop();	}
		m_CurTimeStamp=0;

		return CTransformFilter::EndOfStream();;
	}
}
HRESULT CM4M3DS::FlushBitStream(IMediaSample *pSample)
{

    AM_MEDIA_TYPE *pmtOut, *pmt;

    HRESULT hr;
    ASSERT(pSample);
    IMediaSample * pOutSample;

    // If no output pin to deliver to then no point sending us data
    ASSERT (m_pOutput != NULL) ;

#define rcS1 ((VIDEOINFOHEADER *)(pmt->pbFormat))->rcSource
#define rcT1 ((VIDEOINFOHEADER *)(pmt->pbFormat))->rcTarget

    pSample->GetMediaType(&pmt);
    if (pmt != NULL && pmt->pbFormat != NULL) 
	{
		return S_FALSE;
    }

    // Set up the output sample
    hr = InitializeOutputSample(pSample, &pOutSample);

    if (FAILED(hr)) 
	{
        return hr;
    }

    m_bSampleSkipped = FALSE;

    // The renderer may ask us to on-the-fly to start transforming to a
    // different format.  If we don't obey it, we'll draw garbage

#define rcS ((VIDEOINFOHEADER *)(pmtOut->pbFormat))->rcSource
#define rcT ((VIDEOINFOHEADER *)(pmtOut->pbFormat))->rcTarget

    pOutSample->GetMediaType(&pmtOut);
    if (pmtOut != NULL && pmtOut->pbFormat != NULL) 
	{
		return S_FALSE;
    }

    // After a discontinuity, we need to wait for the next key frame
    if (pSample->IsDiscontinuity() == S_OK) 
	{
        DbgLog((LOG_TRACE,3,TEXT("Non-key discontinuity - wait for keyframe")));
		m_nWaitForKey = 30;
    }

    // Start timing the transform (and log it if PERF is defined)

    if (SUCCEEDED(hr)) 
	{
        m_tDecodeStart = timeGetTime();
        MSR_START(m_idTransform);

		pSample->SetActualDataLength(0);
        // have the derived class transform the data
        hr = Transform(pSample, pOutSample);

        // Stop the clock (and log it if PERF is defined)
        MSR_STOP(m_idTransform);
        m_tDecodeStart = timeGetTime()-m_tDecodeStart;
        m_itrAvgDecode = m_tDecodeStart*(10000/16) + 15*(m_itrAvgDecode/16);

        // Maybe we're waiting for a keyframe still?
        if (m_nWaitForKey)
            m_nWaitForKey--;
        if (m_nWaitForKey && pSample->IsSyncPoint() == S_OK)
			m_nWaitForKey = FALSE;
    }

   
    // the Transform() function can return S_FALSE to indicate that the
    // sample should not be delivered; we only deliver the sample if it's
    // really S_OK (same as NOERROR, of course.)
    // Try not to return S_FALSE to a direct draw buffer (it's wasteful)
    // Try to take the decision earlier - before you get it.

    if (hr == NOERROR) 
	{
	    hr = m_pOutput->Deliver(pOutSample);
    } 
    // release the output buffer. If the connected pin still needs it,
    // it will have addrefed it itself.
    pOutSample->Release();

    return hr;
}


HRESULT CM4M3DS::StartStreaming()
{
//	m_VideoTimeStamp.clear();
	m_EncodedDelayTime=0;//2010/06/14
//	ClearSegmentNAL(m_pM4M3Decoder);

	m_CurTimeStamp=0;//2010/06/15
	return CVideoTransformFilter::StartStreaming();
}/*
HRESULT CM4M3DS::StopStreaming()
{
	return CVideoTransformFilter::StopStreaming();
}*/
HRESULT CM4M3DS::BeginFlush()
{
//	m_VideoTimeStamp.clear();
//	ClearSegmentNAL(m_pM4M3Decoder);
//	m_EncodedDelayTime=0;//2010/06/14

//	m_CurTimeStamp=0;//2010/06/15
	return CVideoTransformFilter::BeginFlush();
}
/*
HRESULT CM4M3DS::EndFlush()
{
	return CVideoTransformFilter::EndFlush();
}*/

HRESULT CM4M3DS::NewSegment(REFERENCE_TIME tStart,REFERENCE_TIME tStop,double dRate)
{
	m_CurTimeStamp=0;
	ClearSegmentNAL(m_pM4M3Decoder);
	return CVideoTransformFilter::NewSegment(tStart,tStop,dRate);
}

void CM4M3DS::ParserSequenceHeader()
{
	if((m_DSVideoInfo_type==MPEG2_VIDEO_MP4||m_DSVideoInfo_type==MPEG2_VIDEO_NAL)&&m_pM4M3Decoder)
	{
		unsigned char *pSeqHeader=m_pSequenceHeader;
		int len=m_SequenceHeaderLen;

		if(m_NalLengthSize>0)
		{
			int size=m_NalLengthSize;//for mp4 avc1
			m_NalLengthSize=2;
			pSeqHeader=InsertH265StartCode(pSeqHeader,len);//2009/06/24
			m_NalLengthSize=size;
		}
		
		ReadParameterSetNAL(m_pM4M3Decoder,pSeqHeader,len);
	}
}

unsigned char* CM4M3DS::InsertH265StartCode(unsigned char *pStream,int &Streamlen)
{
	unsigned char *pData=pStream;
//	if(m_DSVideoInfo_type==MPEG2_VIDEO)//2009/06/24
	{
		if(m_NalLengthSize==4)
		{
			int i;
			int len;
			for(i=0,len=0;len<Streamlen&&i<sizeof(m_H265NalGroupInfo.m_NalType)/sizeof(m_H265NalGroupInfo.m_NalType[0]);i++)//2011/05/16
			{
				int nalsize=(pData[0]<<24)|(pData[1]<<16)|(pData[2]<<8)|(pData[3]);

				nalsize+=4;
				m_H265NalGroupInfo.m_pNal[i]=pData;
				m_H265NalGroupInfo.m_NalLen[i]=nalsize;
				m_H265NalGroupInfo.m_NalType[i]=pData[4];//2010/06/02

				pData[0]=0;
				pData[1]=0;
				pData[2]=0;
				pData[3]=1;

				pData+=nalsize;
				len+=nalsize;
			}
			m_H265NalGroupInfo.m_NalCnt=i;
			pData=pStream;
		}
		else if(m_NalLengthSize==2)
		{
			if(m_NALStreamSize<(Streamlen+3*1024))//2009/06/24
			{
				m_NALStreamSize=(Streamlen+3*1024);
				if(m_NALStreamSize<1024*1024)
					m_NALStreamSize=1024*1024;

				if(m_pNALStream)
					delete []m_pNALStream;
				m_pNALStream=new unsigned char[m_NALStreamSize];
			}

			int i;
			int len,NewStreamLen=0;
			unsigned char *pNALStream=m_pNALStream;
			for(i=0,len=0;len<Streamlen&&i<sizeof(m_H265NalGroupInfo.m_NalType)/sizeof(m_H265NalGroupInfo.m_NalType[0]);i++)//2011/05/16
			{
				int nalsize=(pData[0]<<8)|(pData[1]);

				pNALStream[0]=0;
				pNALStream[1]=0;
				pNALStream[2]=1;

				memcpy(pNALStream+3,pData+2,nalsize*sizeof(unsigned char));

				m_H265NalGroupInfo.m_pNal[i]=pNALStream;
				m_H265NalGroupInfo.m_NalLen[i]=nalsize+3;
				m_H265NalGroupInfo.m_NalType[i]=pNALStream[3];//2010/06/02
			
				pData+=nalsize+2;
				len+=nalsize+2;
				pNALStream+=nalsize+3;
				NewStreamLen+=nalsize+3;
			}
			pNALStream[0]=0;
			pNALStream[1]=0;
			pNALStream[2]=1;
			pNALStream[3]=0x0c;
			pNALStream[4]=0;
			NewStreamLen+=4;//2010/06/02

			m_H265NalGroupInfo.m_NalCnt=i;
			pData=m_pNALStream;
			Streamlen=NewStreamLen;
		}
		else if(m_NalLengthSize==1)
		{
			if(m_NALStreamSize<(Streamlen+3*1024))
			{
				m_NALStreamSize=(Streamlen+3*1024);
				if(m_NALStreamSize<1024*1024)
					m_NALStreamSize=1024*1024;

				if(m_pNALStream)
					delete []m_pNALStream;
				m_pNALStream=new unsigned char[m_NALStreamSize];
			}

			int i;
			int len,NewStreamLen=0;
			unsigned char *pNALStream=m_pNALStream;
			for(i=0,len=0;len<Streamlen&&i<sizeof(m_H265NalGroupInfo.m_NalType)/sizeof(m_H265NalGroupInfo.m_NalType[0]);i++)//2011/05/16
			{
				int nalsize=pData[0];

				pNALStream[0]=0;
				pNALStream[1]=0;
				pNALStream[2]=1;

				memcpy(pNALStream+3,pData+1,nalsize*sizeof(unsigned char));

				m_H265NalGroupInfo.m_pNal[i]=pNALStream;
				m_H265NalGroupInfo.m_NalLen[i]=nalsize+3;
				m_H265NalGroupInfo.m_NalType[i]=pNALStream[3];//2010/06/02

				pData+=nalsize+1;
				len+=nalsize+1;
				pNALStream+=nalsize+3;
				NewStreamLen+=nalsize+3;
			}
			pNALStream[0]=0;
			pNALStream[1]=0;
			pNALStream[2]=1;
			pNALStream[3]=0x0c;
			pNALStream[4]=0;
			NewStreamLen+=4;//2010/06/02
			m_H265NalGroupInfo.m_NalCnt=i;
			pData=m_pNALStream;
			Streamlen=NewStreamLen;

		}
	}
/*	else
	{

	}*/

	return pData;

}

//2011/05/07
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);
extern "C" BOOL WINAPI _CRT_INIT(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);

BOOL WINAPI DllMain(HINSTANCE hDllHandle, DWORD dwReason, LPVOID lpReserved)
{
#ifdef __X86__
//	_CRT_INIT(hDllHandle, dwReason, lpReserved); // initialize CRT
#endif
	return DllEntryPoint(hDllHandle, dwReason, lpReserved); // initialize baseclasses
}