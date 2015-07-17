/******************************************************************
	H.265  software codec
    reference document ITU-T H.265
	**H.265 Directshow language class**
	developed by mojo
	Copyright (c)2002-2015 mojo,all rights reserved
*******************************************************************/
#ifndef _M4M3DS_Lang_h
#define _M4M3DS_Lang_h


class CM4M3DS_EngLang
{
public:
	static const wchar_t	*Get_AutoSkipDeblockingTxt(int index);

protected:
	static const wchar_t m_AutoSkipDeblockingTxt[4][32];
};


#endif