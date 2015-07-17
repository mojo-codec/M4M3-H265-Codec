/****************************************************
	M4M3 H.265 VFW software codec
	developed by mojo
	Copyright (c)2002-2015 mojo,all rights reserved
****************************************************/
#ifndef _M4M3VFW_h
#define _M4M3VFW_h
#include <stdio.h>

#include <commctrl.h>
#include <vfw.h>

#include "def.h"
#include "../M4M3Lib/API/M4M3API.h"

//#define BS_DUMP

class CVFWConfig;

class CM4M3VFWCodec
{
public:
	CM4M3VFWCodec();
	~CM4M3VFWCodec();

	HINSTANCE GetVFW_hInst(){	return m_hInst;};
	void SetVFW_hInst(HINSTANCE phInst)	{	m_hInst=phInst;};

	//ICMsg function
	static INT_PTR CALLBACK AboutProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK SetConfigProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK ConfigProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

	void InitAboutDialog(HWND hDlg, WPARAM wParam, LPARAM lParam);
	void InitSetConfigProc(HWND hDlg, WPARAM wParam, LPARAM lParam);
	BOOL NotifySetConfigProc(HWND hDlg, WPARAM wParam, LPARAM lParam);
	BOOL CommandSetConfigProc(HWND hDlg, WPARAM wParam, LPARAM lParam);
	void UpdateConfigTabItem(CVFWConfig *pVFWConfig);

	void DestroyCodec();

	//for decompress
	LRESULT DecompressQuery(BITMAPINFO *lpbiInput, BITMAPINFO *lpbiOutput);
	int		DecompressColorspace(BITMAPINFOHEADER * inhdr,BITMAPINFOHEADER * outhdr);
	LRESULT DecompressGetFormat(BITMAPINFO *lpbiInput, BITMAPINFO *lpbiOutput);
	LRESULT DecompressBegin(BITMAPINFO *lpbiInput, BITMAPINFO *lpbiOutput);
	LRESULT Decompress(ICDECOMPRESS * icd);
	LRESULT DecompressEnd();

	//for compress
	LRESULT CompressGetFormat(BITMAPINFO *lpbiInput, BITMAPINFO *lpbiOutput);
	int		CompressColorspace(BITMAPINFOHEADER * hdr);
	LRESULT CompressGetSize(BITMAPINFO *lpbiOutput);
	LRESULT CompressQuery(BITMAPINFO *lpbiInput, BITMAPINFO *lpbiOutput);
	LRESULT CompressFramesInfo(ICCOMPRESSFRAMES * icf);

	LRESULT CompressBegin(BITMAPINFO *lpbiInput);
	LRESULT CompressEnd();
	LRESULT Compress(ICCOMPRESS * icc);
protected:
	int			m_FourCC;
	M4M3EncPar	m_M4M3EncPar;
	M4M3DecPar	m_M4M3DecPar;

	CodecHanle	m_pEncoderHanle,
				m_pDecoderHanle;

	CVFWConfig	*m_pVFWConfig,
				*m_pTmpVFWConfig;

	HINSTANCE	m_hInst;//DLL instace
	HWND		m_hConfigDlg;//configure dialog handle


	M4M3EncoderInfo m_EncoderInfo;
	M4M3DecoderInfo m_DecoderInfo;
	VideoOutPar		m_VideoOutPar;
#ifdef BS_DUMP
	FILE *m_outfile;
#endif

protected:
	int  Get_FourCC()const	{ return m_FourCC; }
	bool bSupportFourCC(int FourCC);
	int Get_ColorSpace(CVFWConfig	*pVFWConfig);
	int Get_PixelBitCount(CVFWConfig	*pVFWConfig);

	//ICMsg function
	void GetVerString(wchar_t TextBuffer[256]);
	void EnableConfigTabItem(CVFWConfig *pVFWConfig);
	void LoadConfigFromConfigSetting(CVFWConfig *pVFWConfig);

	static inline int Clip3(int x, int min, int max) { return min<x ? (x<max ? x : max) : min; }
};

#endif
