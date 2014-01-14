#include "stdafx.h"
#include "MainProcFunc.h"
#include "stdio.h"

#include "EnvSettings.h"
#include "Storage.h"

#ifdef		__DIRECTLY_SQL__
#include "zlb_mysql.h"
#pragma comment(lib, "libmysql.lib")
void writeSQL(char * sql) {
	zlb_mysql::DataBase zlb;
	if(-1 == zlb.Connect("192.168.1.189", "root","henry", "watch", 0, 0)) {
		dmsg(L"Connect to DB FAIL!", 1);
		return;
	}// zlb_mysql::RecordSet rs(zlb.GetMysql());
	zlb.ExecQuery(sql);
}
#endif		//__DIRECTLY_SQL__

#ifdef		__W10_OLD_DATA_FORMAT__
	void oldBodyReader(DWORD i, char mode, HANDLE hfile, HANDLE hdestfile, unsigned char* buffer, char* wbuffer);
#else
	void newBodyReader(DWORD i, char mode, HANDLE hfile, HANDLE hdestfile, unsigned char* buffer, char* wbuffer);
#endif		//__W10_OLD_DATA_FORMAT__


BOOL getDistTick(HANDLE hfile, unsigned int & TotalTick, unsigned int & Distance);

void doErrProc(DWORD i) {
	TCHAR szName[512] = {0};
	GetCurrentDirectory(512, szName);
	wcscat_s(szName, 512, L"\\cosmos_spp_temp");
	TCHAR * pszName = szName+wcslen(szName);
	swprintf(pszName, 512, L"\\%06d.tmp", i);
	DeleteFile(szName);
}
void doParseFile(DWORD i) {
	swprintf(msg, 1024, L"\tdoParseFile(%d)", i);
	dmsg(msg);
	unsigned char buffer[512];
	char wbuffer[2048+1] = {0};
	char mode;
	TCHAR szName[512] = {0};
	GetCurrentDirectory(512, szName);
	wcscat_s(szName, 512, L"\\cosmos_spp_temp");
	TCHAR * pszName = szName+wcslen(szName);
	HANDLE hfile = NULL;
	HANDLE hdestfile = NULL;
	DWORD w = 0;
	
	swprintf(pszName, 512, L"\\%06d.tmp", i);
	hfile = CreateFile(szName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_HIDDEN, NULL);
	if(INVALID_HANDLE_VALUE == hfile) {
		CloseHandle(hfile);
		doErrProc(i);
		return;
	}


#ifndef		__W10_OLD_DATA_FORMAT__
	//		Get Total Tick & Dist.
	unsigned int TotalTick = 0;
	unsigned int Distance = 0;
	if( ! getDistTick(hfile, TotalTick, Distance)) {
		CloseHandle(hfile);
		doErrProc(i);
		return;
	}
#endif		//__W10_OLD_DATA_FORMAT__


	//		Read Head
	if(ReadFile(hfile, buffer, sizeof(STORAGE_HEAD), &w, NULL)) {
		if(w < sizeof(STORAGE_HEAD)) {
			CloseHandle(hfile);
			doErrProc(i);
			return;
		}
		STORAGE_HEAD & data = *(STORAGE_HEAD*)buffer;
		//	Create dest file
		swprintf(pszName, 512, L"\\%d_%d.cosmos", (data.Year*10000+data.Month*100+data.Day), i);
		hdestfile = CreateFile(szName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(INVALID_HANDLE_VALUE == hdestfile) {
			CloseHandle(hfile);
			return;
		}
		//	SQL insert summary
		if(1 == data.Mode) mode = 'R';
		else if(2 == data.Mode) mode = 'B';
		else if(3 == data.Mode) mode = 'S';
		else if(4 == data.Mode) mode = 'P';
		else mode = 'O';
		//	select record to check data existance
		sprintf_s(wbuffer, 2048, "sn = '%d' and date = '%d' and mode = '%c' and time = '%d' and distance = '%f'\r\n",
#ifdef		__W10_OLD_DATA_FORMAT__
					i, (data.Year*10000+data.Month*100+data.Day), mode, (data.TotalTick/1000), data.Distance);
#else
					i, (data.Year*10000+data.Month*100+data.Day), mode, (TotalTick/1000), Distance);
#endif
		WriteFile(hdestfile, wbuffer, strlen(wbuffer), &w, NULL);
		//	insert summary
		sprintf_s(wbuffer, 2048, "'%d','%d','%c','%d','%d'\r\n",
#ifdef		__W10_OLD_DATA_FORMAT__
			i, (data.Year*10000+data.Month*100+data.Day), mode, (data.TotalTick/1000), data.Distance);
#else
			i, (data.Year*10000+data.Month*100+data.Day), mode, (TotalTick/1000), Distance);
#endif		//__W10_OLD_DATA_FORMAT__
		WriteFile(hdestfile, wbuffer, strlen(wbuffer), &w, NULL);


		#ifdef		__DIRECTLY_SQL__
		//	insert summary
		sprintf_s(wbuffer, 2048, "insert into summary values('min','%d','%d','%c','%d','%d','100')\0",
				i, (data.Year*10000+data.Month*100+data.Day), mode, (data.TotalTick/1000), data.Distance);
		writeSQL(wbuffer);
		#endif		//__DIRECTLY_SQL__
		
	} else {
		CloseHandle(hfile);
		CloseHandle(hdestfile);
		doErrProc(i);
		return;
	}
	//		Read Body
#ifdef		__W10_OLD_DATA_FORMAT__
	oldBodyReader(i, mode, hfile, hdestfile, buffer, wbuffer);
#else
	newBodyReader(i, mode, hfile, hdestfile, buffer, wbuffer);
#endif		//__W10_OLD_DATA_FORMAT__
	CloseHandle(hfile);
	CloseHandle(hdestfile);
	swprintf(msg, 1024, L"doParseFile(%d)::End of Thread", i);
	dmsg(msg);
}



/****************************************************************
*			Data Parser get Dist. & Tick
*					for NEW Data Format
*					Retrieve Total Dist. & Duration for Summary
*/
BOOL getDistTick(HANDLE hfile, unsigned int & TotalTick, unsigned int & Distance) {
	DWORD w = 0;
	char buffer[512+1];
	int back = -1 * (int)sizeof(STORAGE_DATA);
	if(0xFFFFFFFF == SetFilePointer(hfile, back, NULL, FILE_END)) {
		swprintf(msg, 1024, L"SetFilePointer()::Fail...(%d)", GetLastError());
		dmsg(L"doParseFile()", msg);
		return FALSE;
	}
	if( ! ReadFile(hfile, buffer, sizeof(STORAGE_DATA), &w, NULL)) {
		swprintf(msg, 1024, L"ReadFile()::Fail...(%d)", GetLastError());
		dmsg(L"doParseFile()", msg);
		return FALSE;
	}
	SetFilePointer(hfile, 0, NULL, FILE_BEGIN);
	STORAGE_DATA & final = *(STORAGE_DATA*)buffer;
	TotalTick = final.Tick;
	Distance = (int)final.Distance;
	return TRUE;;
}


/****************************************************************
*
*/
void newBodyReader(DWORD i, char mode, HANDLE hfile, HANDLE hdestfile, unsigned char* buffer, char* wbuffer) {
	DWORD w = 0;
	while(ReadFile(hfile, buffer, sizeof(STORAGE_DATA), &w, NULL) && w == sizeof(STORAGE_DATA)) {
		STORAGE_DATA & data = *(STORAGE_DATA*)buffer;
		if(0 != data.Lat || 0 != data.Long) {
			sprintf_s(wbuffer, 2048, "'%d','%c','%f','%f'\r\n", i, mode, data.Lat, data.Long);
			WriteFile(hdestfile, wbuffer, strlen(wbuffer), &w, NULL);
		}
		swprintf(msg, 1024, L"ReadFile(%d)", w);
		dmsg(L"newBodyReader", msg);
	}
	swprintf(msg, 1024, L"ReadFile(%d)", w);
	dmsg(L"newBodyReader", msg);
}

/****************************************************************
*			Data Parser for OLD Data Format
*					dynamic struct size
*/

void oldBodyReader(DWORD i, char mode, HANDLE hfile, HANDLE hdestfile, unsigned char* buffer, char* wbuffer) {
	DWORD w = 0;
	while(ReadFile(hfile, buffer, 1, &w, NULL) && w > 0) {
		{/*********************************************************************/
			TCHAR body[12+1] = {0};
			swprintf(body, 12, L"body:%02x", buffer[0]);
			dmsg(body);
		}/*********************************************************************/
		switch(buffer[0]) {
			case 0x10:
				ReadFile(hfile, buffer, sizeof(STORAGE_GPS), &w, NULL);
				//	GPS
				if(w < sizeof(STORAGE_HEARTBEAT)) {
					w = 0;
				} else {
					STORAGE_GPS& data = *(STORAGE_GPS*)buffer;
					if(0 != data.Lat || 0 != data.Long) {
						sprintf_s(wbuffer, 2048, "'%d','%c','%f','%f'\r\n",
							i, mode, data.Lat, data.Long);
						WriteFile(hdestfile, wbuffer, strlen(wbuffer), &w, NULL);
						#ifdef		__DIRECTLY_SQL__
						//	insert GPS
						sprintf_s(wbuffer, 1024, "insert into gps values('min','%d','%c','%f','%f')",
								i, mode, data.Lat, data.Long);
						writeSQL(wbuffer);
						#endif		//__DIRECTLY_SQL__
					}
				}
				break;
			case 0x20:
				ReadFile(hfile, buffer, sizeof(STORAGE_HEARTBEAT), &w, NULL);
				if(w < sizeof(STORAGE_HEARTBEAT))
					w = 0;
				break;
			case 0x21:
				ReadFile(hfile, buffer, sizeof(STORAGE_PEDOMETER), &w, NULL);
				if(w < sizeof(STORAGE_PEDOMETER))
					w = 0;
				break;
			case 0x22:
				ReadFile(hfile, buffer, sizeof(STORAGE_BIKE), &w, NULL);
				if(w < sizeof(STORAGE_BIKE))
					w = 0;
				break;
			case 0x30:
				ReadFile(hfile, buffer, sizeof(STORAGE_BAROMETER), &w, NULL);
				if(w < sizeof(STORAGE_BAROMETER))
					w = 0;
				break;
			case 0x31:
				ReadFile(hfile, buffer, sizeof(STORAGE_TEMPERATURE), &w, NULL);
				if(w < sizeof(STORAGE_TEMPERATURE))
					w = 0;
				break;
			case 0x82:
				dmsg(L"lap");
				ReadFile(hfile, buffer, sizeof(STORAGE_LAP), &w, NULL);
				/*
				if(w < sizeof(STORAGE_LAP))
					w = 0;
				{
					char sql[128+1] = {0};
					sprintf_s(sql, 128, "insert into member values('min','min','%d','X','25.123456','128.654321')", i); 
					writeMember(sql);
				}
				*/
				break;
			default:
				break;
			}
		if(0 == w)
			break;
	}
}

