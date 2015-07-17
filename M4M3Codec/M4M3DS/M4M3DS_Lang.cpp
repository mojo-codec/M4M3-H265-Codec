/******************************************************************
	H.265  software codec
    reference document ITU-T H.265
	**H.265 Directshow language class**
	developed by mojo
	Copyright (c)2002-2015 mojo,all rights reserved
*******************************************************************/
#include "M4M3DS_Lang.h"

const wchar_t CM4M3DS_EngLang::m_AutoSkipDeblockingTxt[4][32]=
{
	L"disable deblocking filter",
	L"Never",
	L"Safe",
	L"Always",
};

const wchar_t	*CM4M3DS_EngLang::Get_AutoSkipDeblockingTxt(int index)
{
	return m_AutoSkipDeblockingTxt[index];
}
