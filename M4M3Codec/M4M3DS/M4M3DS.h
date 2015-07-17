/****************************************************
	H.265 decoder software codec
    reference document ITU-T H.265 
	**H.265 direct show filter**
	developed by mojo
	Copyright (c)2002-2015 mojo,all rights reserved
****************************************************/
#ifndef _M4M3DS_h
#define _M4M3DS_h


#include <queue>

#include"M4M3DSAbout.h"

struct _VideoOutPar;

#ifdef __X64__
	#define codec_name L"M4M3 H265 Decoder (64bit)"
#else
	#define codec_name L"M4M3 H265 Decoder"
#endif

#define FOURCC_M4M3 mmioFOURCC('M','4','M','3')

#define FOURCC_X265	mmioFOURCC('X','2','6','5')
#define FOURCC_x265	mmioFOURCC('x','2','6','5')
#define FOURCC_H265	mmioFOURCC('H','2','6','5')
#define FOURCC_h265	mmioFOURCC('h','2','6','5')
#define FOURCC_HEVC	mmioFOURCC('H','E','V','C')
#define FOURCC_hevc	mmioFOURCC('h','e','v','c')



DEFINE_GUID(CLSID_M4M3,		mmioFOURCC('M','4','M','3'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_M4M3_UC,	mmioFOURCC('m','4','m','3'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_x265,		mmioFOURCC('X','2','6','5'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_x265_UC,	mmioFOURCC('x','2','6','5'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_H265,		mmioFOURCC('h','2','6','5'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_H265_UC,	mmioFOURCC('H','2','6','5'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_HEVC,		mmioFOURCC('h','e','v','c'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_HEVC_UC,	mmioFOURCC('H','E','V','C'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

//DEFINE_GUID(CLSID_NAL1,		0x8D2D71CB, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_NAL1 ,0x8d2d71cb, 0x243f, 0x45e3, 0xb2, 0xd8, 0x5f, 0xd7, 0x96, 0x7e, 0xc0, 0x9b);

/*DEFINE_GUID(CLSID_DIVX,		mmioFOURCC('d','i','v','x'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_DIVX_UC,	mmioFOURCC('D','I','V','X'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_DX50,		mmioFOURCC('d','x','5','0'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(CLSID_DX50_UC,	mmioFOURCC('D','X','5','0'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);*/

/* MEDIATYPE_IYUV is not always defined in the directx headers */
DEFINE_GUID(CLSID_MEDIASUBTYPE_IYUV, 0x56555949, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);


class CH265NalGroupInfo
{
public:
	int				m_NalCnt;
	unsigned char	*m_pNal[256];
	unsigned int	m_NalLen[256];
	char			m_NalType[256];
};

class CNALQueue
{
public:
	CNALQueue();
	~CNALQueue();

	void PushPacket(unsigned char *pPacket,int len);
	void AdvancedNal(int len);
	unsigned char* GetNal()	{	return m_pNAL_;}
	int	GetNalLen()			{	return m_NALLen;}
	
protected:
	unsigned char	*m_pBuff,*m_pBuffBeg_,*m_pBuffEnd_;
	int				m_BuffSize;
	unsigned char	*m_pNAL_;
	int				m_NALLen;
};


class CM4M3DS :public CVideoTransformFilter,
	public IM4M3decoder,
	public ISpecifyPropertyPages
{
public:
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
	DECLARE_IUNKNOWN;

	CM4M3DS(LPUNKNOWN punk, HRESULT *phr);
	~CM4M3DS();


	HRESULT CheckInputType(const CMediaType *MediaTypeIn);
	HRESULT GetMediaType(int iPos, CMediaType * pmt);
	HRESULT SetMediaType(PIN_DIRECTION direction, const CMediaType *pmt);
	HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
	HRESULT DecideBufferSize(IMemAllocator * pima, ALLOCATOR_PROPERTIES * pProperties);
	HRESULT ChangeColorSpace(GUID subtype, GUID formattype, void * format);
//	HRESULT out_image(unsigned char*poutimage);
	HRESULT Transform(IMediaSample *pInMedia, IMediaSample *pOutMedia);

	STDMETHODIMP GetPages(CAUUID * pPages);
	STDMETHODIMP FreePages(CAUUID * pPages);


	virtual HRESULT EndOfStream();//2010/03/04
	HRESULT FlushBitStream(IMediaSample *pSample);//2010/03/04
//	virtual HRESULT EndFlush();//2010/03/04

	// you can also override these if you want to know about streaming
	virtual HRESULT StartStreaming();
//	virtual HRESULT StopStreaming();
	virtual HRESULT BeginFlush();
//	virtual HRESULT EndFlush();
	virtual HRESULT NewSegment(REFERENCE_TIME tStart,REFERENCE_TIME tStop,double dRate);

	void ParserSequenceHeader();
	unsigned char* InsertH265StartCode(unsigned char *pStream,int &len);
protected:
	CodecHanle	m_pM4M3Decoder;
	unsigned char *m_pSequenceHeader;//2009/06/23

	int m_DSVideoInfo_type,
		m_ForceColorSpace;

	int	m_SequenceHeaderLen;
	int m_NalLengthSize;		//0:has start code 0x000001;1,2,4 is NalLen byte,not has start code
	int	m_SkipB;
	int	m_NALStreamSize;

	unsigned char *m_pNALStream;//2009/06/23
	
	CH265NalGroupInfo	m_H265NalGroupInfo;
//	CNALQueue			m_NALQueue;//2009/06/25
	_VideoOutPar *m_pVideoOutPar;
	CM4M3DSConfig m_M4M3DSConfig;
	IMediaSample *m_pInMedia_;//2010/03/04
	REFERENCE_TIME  m_AvgTimePerFrame;//2010/03/05

	REFERENCE_TIME m_DecodedDelayTime;//2010/05/20
	REFERENCE_TIME m_EncodedDelayTime;//2010/06/14

	REFERENCE_TIME m_CurTimeStamp;//2010/05/20
//	std::deque<REFERENCE_TIME> m_VideoTimeStamp;//2010/05/20

	M4M3DecoderInfo m_DecoderInfo;//2011/04/26 for colorspace support
	ColorSpaceType	m_M4M3ColorSpace;//2011/04/26
};



#endif
