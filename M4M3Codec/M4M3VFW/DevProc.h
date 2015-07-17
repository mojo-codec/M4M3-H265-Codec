#ifndef _DevProc_h
#define _DevProc_h
#include <windows.h>
#include <stdio.h>
#include <vfw.h>

#include "def.h"

class CM4M3VFWCodec;

CM4M3VFWCodec* Get_gM4M3VFWCodec();

DLLExport LRESULT WINAPI DriverProc(
									DWORD dwDriverId, 
									HDRVR hDriver, 
									UINT Msg, 
									LPARAM lParam1, 
									LPARAM lParam2);


#endif
