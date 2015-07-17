/****************************************************
	M4M3 H.265 VFW codec register
	developed by mojo
	Copyright (c)2002-2015 mojo,all rights reserved
****************************************************/
//#include "stdafx.h"

#include<Windows.h>

void RegM4M3VFW()
{
	HKEY Key;
	DWORD pdwDisposition;
	///for nt
	if(RegCreateKeyEx(
		HKEY_LOCAL_MACHINE,
		TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\drivers.desc"),
		0,
		/*"M4M3VFW.dll"*/NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		0, 
		&Key, 
		&pdwDisposition)==ERROR_SUCCESS)
	{
		RegSetValueEx(Key, TEXT("M4M3VFW.dll"), 0, 
			REG_SZ, (LPBYTE)&TEXT("M4M3 H265 Codec"), sizeof(TEXT("M4M3 H265 Codec")));
	}
	if(RegCreateKeyEx(
		HKEY_LOCAL_MACHINE,
		TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\drivers32"),
		0,
		/*"M4M3VFW.dll"*/NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		0, 
		&Key, 
		&pdwDisposition)==ERROR_SUCCESS)
	{
		RegSetValueEx(Key, TEXT("vidc.M4M3"), 0, 
			REG_SZ, (LPBYTE)&TEXT("M4M3VFW.dll"), sizeof(TEXT("M4M3VFW.dll")));
	}
	/// for 9x
	if(RegCreateKeyEx(
		HKEY_LOCAL_MACHINE,
		TEXT("SYSTEM\\CurrentControlSet\\Control\\MediaResources\\icm\\vidc.M4M3"),
		0,
		/*"vidc.M4M3"*/NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		0, 
		&Key, 
		&pdwDisposition)==ERROR_SUCCESS)
	{
		RegSetValueEx(Key, TEXT("Description"), 0, 
			REG_SZ, (LPBYTE)&TEXT("M4M3 H265 Codec"), sizeof(TEXT("M4M3 H265 Codec")));
		RegSetValueEx(Key, TEXT("Driver"), 0, 
			REG_SZ, (LPBYTE)&TEXT("M4M3VFW.dll"), sizeof(TEXT("M4M3VFW.dll")));
		RegSetValueEx(Key, TEXT("FriendlyName"), 0, 
			REG_SZ, (LPBYTE)&TEXT("M4M3"), sizeof(TEXT("M4M3")));
	}

}

void UnRegM4M3VFW()
{
	HKEY Key;
	DWORD pdwDisposition;
	//nt
	if(RegCreateKeyEx(
		HKEY_LOCAL_MACHINE,
		TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\drivers.desc"),
		0,
		/*"M4M3VFW.dll"*/NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		0, 
		&Key, 
		&pdwDisposition)==ERROR_SUCCESS)
	{
		RegDeleteValue(Key, TEXT("M4M3VFW.dll"));
	}
	if(RegCreateKeyEx(
		HKEY_LOCAL_MACHINE,
		TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\drivers32"),
		0,
		/*"M4M3VFW.dll"*/NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		0, 
		&Key, 
		&pdwDisposition)==ERROR_SUCCESS)
	{
		RegDeleteValue(Key, TEXT("vidc.M4M3"));
	}

	/// for 9x
	if(RegCreateKeyEx(
		HKEY_LOCAL_MACHINE,
		TEXT("SYSTEM\\CurrentControlSet\\Control\\MediaResources\\icm"),
		0,
		/*"vidc.M4M3"*/NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		0, 
		&Key, 
		&pdwDisposition)==ERROR_SUCCESS)
	{
		RegDeleteKey(Key, TEXT("vidc.M4M3"));
	}
}

/*
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, 
				   INT nCmdShow)
{

	if(memcmp(pCmdLine,TEXT("/r"),sizeof(TEXT("/r")))==0)
		RegM4M3VFW();
	else if(memcmp(pCmdLine,TEXT("/u"),sizeof(TEXT("/u")))==0)
		UnRegM4M3VFW();
	return TRUE;
}*/

