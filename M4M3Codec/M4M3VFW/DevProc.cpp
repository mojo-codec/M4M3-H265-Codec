//#include "stdafx.h"

#include <windows.h>
#include <stdio.h>
#include <vfw.h>


#include "resource.h"

#include "DevProc.h"
#include "M4M3VFW.h"


CM4M3VFWCodec  *g_M4M3VFWCodec=nullptr;
CM4M3VFWCodec* Get_gM4M3VFWCodec()	{ return g_M4M3VFWCodec; }

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		if(nullptr==g_M4M3VFWCodec)
		{
			g_M4M3VFWCodec=new CM4M3VFWCodec();
			Get_gM4M3VFWCodec()->SetVFW_hInst((HINSTANCE)hModule);
		}
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		delete g_M4M3VFWCodec;
		g_M4M3VFWCodec=nullptr;
		break;
	}
	return TRUE;
}


DLLExport LRESULT WINAPI DriverProc(
									DWORD dwDriverId, 
									HDRVR hDriver, 
									UINT Msg, 
									LPARAM lParam1, 
									LPARAM lParam2) 
{

	HINSTANCE hInst = Get_gM4M3VFWCodec()->GetVFW_hInst();
	CM4M3VFWCodec *pM4M3VFWCodec = Get_gM4M3VFWCodec();
	switch(Msg)
	{
		//codec state message
	case ICM_ABOUT:
		DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_ABOUT), (HWND)lParam1, CM4M3VFWCodec::AboutProc,NULL);
		return ICERR_OK;

	case ICM_CONFIGURE:
		DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_CONFIG), (HWND)lParam1, CM4M3VFWCodec::SetConfigProc,NULL);
		return ICERR_OK;

	case ICM_GETSTATE:
	case ICM_SETSTATE:
		return ICERR_OK;

	case ICM_GETINFO:
		{
			ICINFO *icinfo = (ICINFO *)lParam1;

			icinfo->fccType = ICTYPE_VIDEO;
			icinfo->fccHandler = FOURCC_HEVC;
			icinfo->dwFlags =
				VIDCF_FASTTEMPORALC |
				VIDCF_FASTTEMPORALD |
				VIDCF_COMPRESSFRAMES;

			icinfo->dwVersion = 0;
			icinfo->dwVersionICM = ICVERSION;
			wcscpy(icinfo->szName, M4M3_NAME); 
			wcscpy(icinfo->szDescription, M4M3_Description);
			return lParam2;
		}
	case ICM_GET:
	case ICM_SET:
		return ICERR_OK;

	case ICM_GETQUALITY:
	case ICM_SETQUALITY:
	case ICM_GETDEFAULTQUALITY:
	case ICM_GETDEFAULTKEYFRAMERATE:
		return ICERR_UNSUPPORTED;

		//drive message
	case DRV_LOAD:
	case DRV_FREE:
		return DRV_OK;

	case DRV_OPEN:
		{
			ICOPEN *icopen=(ICOPEN*)lParam2;
			if(icopen!=NULL && icopen->fccType!=ICTYPE_VIDEO)
				return DRV_CANCEL;
			
			if (icopen != NULL)
			{
				icopen->dwError = ICERR_OK;
			}

			return (LRESULT)Get_gM4M3VFWCodec();
		}
	case DRV_CLOSE:
		{
			Get_gM4M3VFWCodec()->DestroyCodec();
		}
		return DRV_OK;

	case DRV_DISABLE:
	case DRV_ENABLE:
	case DRV_INSTALL:
	case DRV_REMOVE:
		return DRV_OK;

	case DRV_QUERYCONFIGURE:
		//return DRV_CANCEL;
		return 1;
	case DRV_CONFIGURE:
		DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_ABOUT), (HWND)lParam1, CM4M3VFWCodec::AboutProc,NULL);
//		DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_CONFIG), (HWND)lParam1, CM4M3VFWCodec::SetConfigProc,NULL);
		return ICERR_OK;
//decompress message
	case ICM_DECOMPRESS_GET_PALETTE:
	case ICM_DECOMPRESS_SET_PALETTE:
	case ICM_DECOMPRESSEX_QUERY:
	case ICM_DECOMPRESSEX_BEGIN:
	case ICM_DECOMPRESSEX_END:
	case ICM_DECOMPRESSEX:
		return ICERR_UNSUPPORTED;
	case ICM_DECOMPRESS_QUERY:
		return pM4M3VFWCodec->DecompressQuery((BITMAPINFO*)lParam1, (BITMAPINFO*)lParam2);
	case ICM_DECOMPRESS_GET_FORMAT:
		return pM4M3VFWCodec->DecompressGetFormat((BITMAPINFO*)lParam1, (BITMAPINFO*)lParam2);
	case ICM_DECOMPRESS_BEGIN:	
		return pM4M3VFWCodec->DecompressBegin((BITMAPINFO*)lParam1, (BITMAPINFO*)lParam2);
	case ICM_DECOMPRESS:
		return pM4M3VFWCodec->Decompress((ICDECOMPRESS*)lParam1);
	case ICM_DECOMPRESS_END:	
		return pM4M3VFWCodec->DecompressEnd();
//compress message
	case ICM_COMPRESS_GET_FORMAT:
		return pM4M3VFWCodec->CompressGetFormat((BITMAPINFO *)lParam1, (BITMAPINFO *)lParam2);
	case ICM_COMPRESS_QUERY:
		return pM4M3VFWCodec->CompressQuery((BITMAPINFO *)lParam1, (BITMAPINFO *)lParam2);
	case ICM_COMPRESS_GET_SIZE:
		return pM4M3VFWCodec->CompressGetSize((BITMAPINFO *)lParam2);
	case ICM_COMPRESS_FRAMES_INFO:
		return pM4M3VFWCodec->CompressFramesInfo((ICCOMPRESSFRAMES *)lParam1);


	case ICM_COMPRESS_BEGIN:
		return pM4M3VFWCodec->CompressBegin((BITMAPINFO *)lParam1);
	case ICM_COMPRESS:
		return pM4M3VFWCodec->Compress((ICCOMPRESS *)lParam1);
	case ICM_COMPRESS_END:
		return pM4M3VFWCodec->CompressEnd();
		


	default:
		return DefDriverProc(dwDriverId, hDriver, Msg, lParam1, lParam2);
	}

	return DefDriverProc(dwDriverId, hDriver, Msg, lParam1, lParam2);
}
