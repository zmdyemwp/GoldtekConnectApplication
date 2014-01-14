// stdafx.cpp : source file that includes just the standard includes
// w10_pc_user.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

void dmsg(TCHAR * msg, DWORD x)
{
	dmsg(L"", msg, x);
}

void dmsg(TCHAR * tag, TCHAR * msg, DWORD x) {
	if(0 == x)
		return;

	TCHAR buffer[1200+1] = {0};
	swprintf(buffer, 1200, L"[Cosmos::%s]    %s\r\n", tag, msg);
	
	OutputDebugString(buffer);



	/*
	HWND hDlg = FindWindow(NULL, L"DebugMsg");
	if(IsWindow(hDlg))
	{
		SendDlgItemMessage(hDlg, 10101, EM_REPLACESEL, 0, (LPARAM)buffer);
	}
	else {
		//MessageBox(NULL, L"Window NOT Found", NULL, 0);
	}
	*/


}

