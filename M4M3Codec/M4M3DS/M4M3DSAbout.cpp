/****************************************************
	H.265 decoder software codec
    reference document ITU-T H.265
	**H.265 direct show filter about**
	developed by mojo
	Copyright (c)2002-2015 mojo,all rights reserved
****************************************************/
#include <streams.h>
#include <commctrl.h>
#include <olectl.h>
#include <memory.h>
#include "resource.h"

#include"M4M3DSAbout.h"
#include "M4M3DS_Lang.h"


extern HINSTANCE g_hInst;

//static CM4M3DSAbout *g_pM4M3DSAbout;

CodecHanle	CM4M3DSAbout::m_pCurrM4M3Decoder=NULL;

// CreateInstance
CUnknown * WINAPI CM4M3DSAbout::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
	CUnknown *punk = new CM4M3DSAbout(lpunk, phr);
	if (punk == NULL) {
		*phr = E_OUTOFMEMORY;
	}
	return punk;
}

CM4M3DSAbout::CM4M3DSAbout(LPUNKNOWN pUnk, HRESULT *phr) 
	:CBasePropertyPage(NAME("M4M3DSAbout"),pUnk,IDD_M4M3ABOUT,IDS_M4M3ABOUT)
{

}

CM4M3DSAbout::~CM4M3DSAbout()
{
	m_M4M3DSConfig.SaveSetting();
}

BOOL CALLBACK Dialog_Proc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
//	HKEY Key;
//	DWORD size=sizeof(out_color);
//	DWORD pdwDisposition;
//	bool dec_other_mpeg4;


	CM4M3DSAbout *pM4M3DSAbout=(CM4M3DSAbout *)lParam;
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
				/*
			RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\MPEG4\\M4M1"
				, 0, KEY_READ, &Key);
			if(RegQueryValueEx(Key, "color_space", 0, 0, (LPBYTE)&out_color, &size)!= ERROR_SUCCESS)
				out_color=-1;
			if(RegQueryValueEx(Key, "Dec_other_MPEG4", 0, 0, (LPBYTE)&dec_other_mpeg4, &size)!= ERROR_SUCCESS)
				dec_other_mpeg4=false;
			RegCloseKey(Key);
			*/
			//	SetDlgItemText(hDlg,IDC_VER,codec_ver);
			//	SetDlgItemText(hDlg,IDC_BUILD,build_num);
			//	SetDlgItemText(hDlg,IDC_DATE,__DATE__);

			M4M3DecPar DecPar;
			M4M3DecoderInfo DecoderInfo;
			DecPar.m_ThreadNum=1;
			DecPar.m_CPUFlag=0;

			//M4M3 core ver
			CodecHanle pM4M3Decoder=CreateM4M3Decoder(&DecPar);
			if(pM4M3Decoder)
			{
				static const wchar_t Profile[3][20]=
				{
					{L"Main "},
					{L"Main10 "},
					{L"MainStillPicture "}
				};

				wchar_t TextBuffer[256];
				wchar_t *pText;
				memset(TextBuffer,0,sizeof(TextBuffer));

				GetDecoderInfo(pM4M3Decoder,&DecoderInfo);

			/*	if(DecoderInfo.m_bUnicode )
				{
					wsprintf(TextBuffer,__TEXT("Core Ver:%s-P%d\n")
										__TEXT("Profile:")
						,					DecoderInfo.m_Ver,DecoderInfo.m_MaxThread);
				}
				else*/
				{
					wchar_t TextTmp[256];
					memset(TextTmp,0,sizeof(TextTmp));
					for(size_t i=0;i<strlen(DecoderInfo.m_Ver);i++)
					{
						TextTmp[i]=DecoderInfo.m_Ver[i];
					}
					wsprintf(TextBuffer,__TEXT("Core Ver:%s-P%d\n")
										__TEXT("Profile:")
						,					TextTmp,DecoderInfo.m_MaxThread);

				}

				pText=TextBuffer+wcslen(TextBuffer);
				for(int i=0;i<3;i++)
				{
					if(DecoderInfo.m_bSupportProfile[i])
					{
						memcpy(pText,Profile[i],wcslen(Profile[i])*sizeof(wchar_t));
						pText+=wcslen(Profile[i]);
					}
				}
				SetDlgItemText(hDlg,IDC_M4M3VER,TextBuffer);

				DestroyM4M3Decoder(pM4M3Decoder);
			}
			SetDlgItemText(hDlg,IDC_DSVER,M4M3DirectShowVer);//2008/07/20

			pM4M3DSAbout->SetConfigTxt(hDlg);//2008/07/14
	/*
			CheckDlgButton(hDlg, IDC_CSYUY2,  BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CSYV12,  BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CSYUV420,BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CSRGB24, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CSRGB32,   BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CSAUTO,  BST_UNCHECKED);
			if(dec_other_mpeg4)
				CheckDlgButton(hDlg, IDC_DEC_OTHER_MPEG4,  BST_CHECKED);
			else
				CheckDlgButton(hDlg, IDC_DEC_OTHER_MPEG4,  BST_UNCHECKED);
			switch(out_color)
			{
			case -1:
				CheckDlgButton(hDlg, IDC_CSAUTO,  BST_CHECKED);
				break;
			case 0:
				CheckDlgButton(hDlg, IDC_CSYUY2,  BST_CHECKED);
				break;
			case 1:
				CheckDlgButton(hDlg, IDC_CSYV12,  BST_CHECKED);
				break;
			case 2:
				CheckDlgButton(hDlg, IDC_CSYUV420,BST_CHECKED);
				break;
			case 3:
				CheckDlgButton(hDlg, IDC_CSRGB32,   BST_CHECKED);			
				break;
			case 4:
				CheckDlgButton(hDlg, IDC_CSRGB24, BST_CHECKED);			
				break;
			}*/
		}
		return 1;

	case WM_COMMAND:
		{
			switch(LOWORD(wParam) )
			{
			case IDC_COMBO_AUTOSKIPDEBLOCKING:
				{
					pM4M3DSAbout->GetNewAutoSkipDeblockingFlag(hDlg);
				}
				
				break;
			}
		}
		return 1;
	case WM_DESTROY:
		//	HKEY Key;
		//	DWORD pdwDisposition;
		/*		if(IsDlgButtonChecked(hDlg, IDC_CSAUTO) == BST_CHECKED)
		out_color=-1;
		else if(IsDlgButtonChecked(hDlg, IDC_CSYUY2) == BST_CHECKED)
		out_color=0;
		else if(IsDlgButtonChecked(hDlg, IDC_CSYV12) == BST_CHECKED)
		out_color=1;
		else if(IsDlgButtonChecked(hDlg, IDC_CSYUV420) == BST_CHECKED)
		out_color=2;
		else if(IsDlgButtonChecked(hDlg, IDC_CSRGB32) == BST_CHECKED)
		out_color=3;
		else if(IsDlgButtonChecked(hDlg, IDC_CSRGB24) == BST_CHECKED)
		out_color=4;

		//save reg
		if(RegCreateKeyEx(
		HKEY_CURRENT_USER,
		"Software\\MPEG4\\M4M1",
		0,
		"DS config",
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		0, 
		&Key, 
		&pdwDisposition)==ERROR_SUCCESS)
		{
		RegSetValueEx(Key, "color_space", 0, 
		REG_DWORD, (LPBYTE)&out_color, sizeof(int));
		RegCloseKey(Key);
		}*/
		return 1;
	}
	return 0;
}

INT_PTR CM4M3DSAbout::OnReceiveMessage(HWND hDlg,
								   UINT uMsg,
								   WPARAM wParam,
								   LPARAM lParam)
{
	if(Dialog_Proc(hDlg,uMsg,wParam,(LPARAM)this)==1)
		return 1;
	else
		return CBasePropertyPage::OnReceiveMessage(hDlg,uMsg,wParam,lParam);
}

/**********************************************************************
*	@brief set  setting config text
*	@author mojo					          *
***********************************************************************/
void CM4M3DSAbout::SetConfigTxt(HWND hDlg)
{
	m_M4M3DSConfig.LoadSetting();

//auto skip deblocking 
	SetDlgItemText(hDlg,IDC_STATIC_AUTOSKIPDEBLOCKING,CM4M3DS_EngLang::Get_AutoSkipDeblockingTxt(0));
	SendDlgItemMessage(hDlg, IDC_COMBO_AUTOSKIPDEBLOCKING, CB_ADDSTRING, 0, (LPARAM)CM4M3DS_EngLang::Get_AutoSkipDeblockingTxt(1));
	SendDlgItemMessage(hDlg, IDC_COMBO_AUTOSKIPDEBLOCKING, CB_ADDSTRING, 1, (LPARAM)CM4M3DS_EngLang::Get_AutoSkipDeblockingTxt(2));
	SendDlgItemMessage(hDlg, IDC_COMBO_AUTOSKIPDEBLOCKING, CB_ADDSTRING, 2, (LPARAM)CM4M3DS_EngLang::Get_AutoSkipDeblockingTxt(3));

	SendDlgItemMessage(hDlg, IDC_COMBO_AUTOSKIPDEBLOCKING, CB_SETCURSEL, m_M4M3DSConfig.Get_AutoSkipDeblockingFlag(), 0);
}
/**********************************************************************
*	@brief save  setting config 
*	implement by mojo					          *
***********************************************************************/
void CM4M3DSAbout::SaveConfigTxt(HWND hDlg)
{
	GetNewAutoSkipDeblockingFlag(hDlg);
}
/**********************************************************************
*	@brief save  setting config 
*	@author mojo					          *
***********************************************************************/
_inline void CM4M3DSAbout::GetNewAutoSkipDeblockingFlag(HWND hDlg)
{
	LRESULT iIndex;
	iIndex = SendDlgItemMessage(hDlg, IDC_COMBO_AUTOSKIPDEBLOCKING, CB_GETCURSEL, 0, 0);

	if(iIndex!=LB_ERR&&iIndex!=m_M4M3DSConfig.Get_AutoSkipDeblockingFlag())
	{
		m_M4M3DSConfig.Set_AutoSkipDeblockingFlag(iIndex);

		SetAutoSkipInloopFilterFlag(m_pCurrM4M3Decoder,iIndex);
	}
}



/**********************************************************************
*	@brief load M4M3 directshow setting config
*	@author mojo					          *
***********************************************************************/
void CM4M3DSConfig::LoadSetting()
{
	HKEY Key;
	DWORD size;
	LONG Res=ERROR_SUCCESS;
	if(RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\H265\\M4M3", 0, KEY_READ, &Key)==ERROR_SUCCESS)
	{
		//load AutoSkipDeblockingFlag
		size=sizeof(m_AutoSkipDeblockingFlag);
		if(RegQueryValueEx(Key, L"AutoSkipDeblocking", 0, 0, (LPBYTE)&m_AutoSkipDeblockingFlag, &size)!= ERROR_SUCCESS)
		{
			m_AutoSkipDeblockingFlag=1;
			Res=1;
		}

		RegCloseKey(Key);

		if(Res)
			SaveSetting();
	}
	else
	{
		SaveSetting();
	}
}
/**********************************************************************
*	@brief load M4M3 directshow setting config
*	@author mojo					          *
***********************************************************************/
void CM4M3DSConfig::SaveSetting()
{
	HKEY Key;
	DWORD pdwDisposition;

	if(RegCreateKeyEx(
		HKEY_CURRENT_USER,
		L"Software\\H265\\M4M3",
		0,
		L"DS config",
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		0, 
		&Key, 
		&pdwDisposition)==ERROR_SUCCESS)
	{
		//save AutoSkipDeblockingFlag
		RegSetValueEx(Key, L"AutoSkipDeblocking", 0, 
			REG_DWORD, (LPBYTE)&m_AutoSkipDeblockingFlag, sizeof(m_AutoSkipDeblockingFlag));

		RegCloseKey(Key);
	}
}
