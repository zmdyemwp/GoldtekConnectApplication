#include "stdafx.h"

#include "ComIO.h"

CRITICAL_SECTION ComIO::cs;

TCHAR ComIO::szEvent[][64] = {
	L"__SZ_EVENT_ComIO_ReadThread_Terminator__",
	L"",
};

ComIO::~ComIO() {
	EnterCriticalSection(&cs);
	DeinitComIO();
	Sleep(200);
	LeaveCriticalSection(&cs);
	DeleteCriticalSection(&cs);
}

void ComIO::InitCS() {
	InitializeCriticalSection(&cs);
	proc = NULL;
	objproc = NULL;
	memset(ack, 0, sizeof(ack));
}

ComIO::ComIO(DWORD com) {
	InitCS();
	swprintf(szDevName, 256, L"COM%d:", com);
}

ComIO::ComIO(TCHAR* com) {
	InitCS();
	swprintf(szDevName, 256, L"%s", com);
}

BOOL ComIO::InitComIO(BOOL bSingle) {
	BOOL result = TRUE;
	//	Open comm port and initialize it to 8-n-1
	hCom = CreateFile(szDevName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == hCom) {
		swprintf(msg, 1024, L"InitComIO()::INVALID_HANDLE_VALUE(%d)", GetLastError());
		dmsg(msg);
		CloseHandle(hCom);
		hCom = NULL;
		return FALSE;
	}
	DCB dcb;
	memset(&dcb, 0, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	GetCommState(hCom, &dcb);
	dcb.ByteSize = 8;
	dcb.fParity = FALSE;
	dcb.StopBits = ONESTOPBIT;
	SetCommState(hCom, &dcb);
	//	set comm mask
	SetCommMask(hCom, EV_BREAK | EV_ERR | EV_RXCHAR);
	//	set comm timeout
	COMMTIMEOUTS timeout;
	timeout.ReadIntervalTimeout = 5;
	timeout.ReadTotalTimeoutConstant = 100;
	timeout.ReadTotalTimeoutMultiplier = 0;
	SetCommTimeouts(hCom, &timeout);
	//	Initialize the reading thread
	if( ! bSingle) {
		hReadThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ComIO::ReadThread, this, 0, NULL);
	}
	return result;
}

BOOL ComIO::DeinitComIO(void) {
	BOOL result = TRUE;
	//	Terminate the reading thread
	//		Do nothing! Since there is some comm event will be sent when the comm is closed.
	//	Close comm port
	CloseHandle(hCom);
	hCom = NULL;
	return result;
}

DWORD ComIO::ReadCom(char* buffer, DWORD size) {
	//	Read data from comm port to buffer
	DWORD read = 0;
	EnterCriticalSection(&cs);
	if(NULL != hCom) {
		ReadFile(hCom, buffer, size, &read, NULL);
	}
	LeaveCriticalSection(&cs);
	return read;
}

DWORD ComIO::WriteCom(char* buffer, DWORD size) {
	//	Write data to comm port
	DWORD write = 0;
	EnterCriticalSection(&cs);
	if(NULL != hCom) {
		if( ! WriteFile(hCom, buffer, size, &write, NULL)) {
			swprintf(msg, 1024, L"Error(%d)\r\n", GetLastError());
			dmsg(msg);
		}
	} else {
		dmsg(L"ComIO::WriteCom()::NULL == hCom");
	}
	LeaveCriticalSection(&cs);
	return write;
}

void ComIO::cmdEngine(TCHAR* cmd, DWORD len) {
}

void ComIO::SendAck() {
	return;
	if(0 < strlen((char*)ack)) {
		WriteCom((char*)ack, strlen((char*)ack));
	}
}

void readThreadBackup();
void ComIO::ReadThread(ComIO* pobj) {
	ComIO & obj = *pobj;
	//	reading thread
	DWORD events = 0;
	const DWORD buflen = 1024;
	DWORD read = 0;
	unsigned char buf[buflen+1] = {0};
#if 1
	while(obj.getHandle()) {
		read = obj.ReadCom((char*)buf, buflen);
		if(read) {
			//obj.SendAck();
			if(NULL != obj.proc) {
				obj.proc((char*)buf, read);
			} else if(NULL != obj.objproc) {
				obj.objproc->proc((char*)buf, read);
			} else {
				DWORD len = 0;
				for(DWORD i = 0; i < read; i++) {
					len = wcslen(msg);
					swprintf(msg + len, 1024 - len, L"%02x ", buf[i]);
				}
				len = wcslen(msg);
				swprintf(msg + len, 1024 - len, L"(%d)", read);
				dmsg(msg,1);
			}
		}
		Sleep(50);
	}
#else
	//	read with comm event, but NOT work!
	readThreadBackup();
#endif
	dmsg(L"End of ComIO::ReadThread", 1);
}









/*************************************************************************************
*		this does not work!
*			keep as sample code
*			or future use with proper solution...
*/

void readThreadBackup() {/*
	while(WaitCommEvent(obj.getHandle(), &events, NULL)) {
		if (EV_BREAK == (EV_BREAK & events)) {
			//	Do nothing
		} else if (EV_ERR == (EV_ERR & events)) {
			break;
		} else if (EV_RXCHAR == (EV_RXCHAR & events)) {
			//	Read data from the device
			//if( ! ReadFile(obj.getHandle(), buf, buflen, &read, NULL)) {
			if(0 == (read = obj.ReadCom(buf, buflen))) {
				//break;
				Sleep(50);
				continue;
			} else {
				if(NULL != obj.proc) {
					//obj.proc(buf, read);
					DWORD len = 0;
					TCHAR buffer[4096] = {0};
					for(int i = 0; i < read; i++) {
						len = wcslen(len);
						swprintf(buffer + len, 4096 - len, L"%02x", buffer[i]);
					}
					dmsg(buffer, 1);
				} else {
					//	NO customized routine is registered
					//	Define some default routine in this case
				}
			}
		} else {
			//	Unknown event
		}
	}*/
}




