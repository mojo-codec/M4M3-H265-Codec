/****************************************************
	H.265 decoder software codec
    reference document ITU-T H.265
	**H.265 direct show filter about**
	developed by mojo
	Copyright (c)2002-2015 mojo,all rights reserved
****************************************************/
#ifndef _M4M3DSAbout_h
#define _M4M3DSAbout_h

#include "../M4M3Lib/API/M4M3API.h"

#define M4M3DirectShowVer	L"M4M3 Direct show filter ver 1.0.5.7"

DEFINE_GUID(CLSID_M4M3ABOUT,mmioFOURCC('M','4','3','A'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

#ifdef __cplusplus
extern "C" 
{
#endif
	DEFINE_GUID(IID_IM4M3decoder,mmioFOURCC('I','M','4','3'), 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

	DECLARE_INTERFACE_(IM4M3decoder, IUnknown)
	{
	};

#ifdef __cplusplus
}
#endif

BOOL CALLBACK Dialog_Proc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
int PropertDialog(HWND hwndOwner);


class CM4M3DSConfig
{
public:
	CM4M3DSConfig()
		:m_AutoSkipDeblockingFlag(1)
	{	}
	

	void LoadSetting();
	void SaveSetting();
	
	int Get_AutoSkipDeblockingFlag()	{	return m_AutoSkipDeblockingFlag;}
	int Set_AutoSkipDeblockingFlag(int flag)	{	return m_AutoSkipDeblockingFlag=flag;}
protected:
	int m_AutoSkipDeblockingFlag;
private:
};




class CM4M3DSAbout:public CBasePropertyPage
{
public:
	CM4M3DSAbout(LPUNKNOWN lpunk, HRESULT *phr);
	CM4M3DSAbout()
		:CBasePropertyPage(NAME("M4M3DSAbout"),0,0,0)
	{

	}
	~CM4M3DSAbout();
	
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);
	INT_PTR OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	//	int PropertDialog(HWND hwndOwner);
	//	BOOL CALLBACK Dialog_Proc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	//	int out_color;

/////////
	void SetConfigTxt(HWND hDlg);
	void SaveConfigTxt(HWND hDlg);

	void GetNewAutoSkipDeblockingFlag(HWND hDlg);


	const CM4M3DSConfig *Get_M4M3DSConfig(){	return &m_M4M3DSConfig;}
	
public:
	static CodecHanle	m_pCurrM4M3Decoder;
protected:

	
	CM4M3DSConfig	m_M4M3DSConfig;
};

#endif