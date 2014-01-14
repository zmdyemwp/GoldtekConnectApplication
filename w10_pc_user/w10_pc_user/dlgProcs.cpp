#include "stdafx.h"
#include "dlgProcs.h"
#include "w10Param.h"
#include "resource.h"
#include "math.h"

static W10PARAM * obj;

INT_PTR CALLBACK sampleProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
		case WM_INITDIALOG:
			//		Title
			SetWindowText(hDlg, w10_LoadString(obj->title));
			swprintf(msg, 1024, L"%s(%d)", w10_LoadString(obj->title), obj->title);
			dmsg(msg);
			SetDlgItemText(hDlg, IDC_APPLY, w10_LoadString(IDS_WWE_APPLY));
			//		Options
			return TRUE;
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDC_APPLY:
					//		TODO:	revise value here.
					EndDialog(hDlg, LOWORD(wParam));
					break;
				case IDOK:
				case IDCANCEL:
					EndDialog(hDlg, LOWORD(wParam));
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
	return FALSE;
}

/*********************************************************************
*		COMBOBOX
*/
INT_PTR CALLBACK ComboListProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	if(WM_INITDIALOG == message) {
		obj = (W10PARAM *)lParam;
		//		Options
		for(DWORD i = 0; i < obj->count; i++) {
			SendDlgItemMessage(hDlg, IDC_OPTIONS, CB_INSERTSTRING,
				-1, (LPARAM)w10_LoadString(obj->options[i]));
		}
		SendDlgItemMessage(hDlg, IDC_OPTIONS, CB_SETCURSEL, obj->index, 0);
	} else if(WM_COMMAND == message && IDC_APPLY == LOWORD(wParam)) {
		//		TODO:	revise value here.
		obj->index = SendDlgItemMessage(hDlg, IDC_OPTIONS, CB_GETCURSEL, 0, 0);
	}
	return sampleProc(hDlg,message,wParam,lParam);
}

/*********************************************************************
*		EDITTEXT
*/
INT_PTR CALLBACK EditProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	if(WM_INITDIALOG == message) {
		obj = (W10PARAM *)lParam;
		//		Options
		SetDlgItemInt(hDlg, IDC_EDIT, obj->index, FALSE);
		SetDlgItemText(hDlg, IDC_UNIT_TEXT, w10_LoadString(obj->options[0]));
	} else if(WM_COMMAND == message && IDC_APPLY == LOWORD(wParam)) {
		//		TODO:	revise value here.
		BOOL result;
		obj->index = GetDlgItemInt(hDlg, IDC_EDIT, &result, FALSE);
	}
	return sampleProc(hDlg,message,wParam,lParam);
}

/*********************************************************************
*		TIMEZONE (COMBOBOX * 2)
*/
INT_PTR CALLBACK TimeZoneProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	if(WM_INITDIALOG == message) {
		obj = (W10PARAM *)lParam;
		DWORD hour = obj->index >> 16;
		hour = (hour > 20)? hour+12-32 : hour+12;
		DWORD minute = obj->index & 0xff;
		TCHAR temp[8];
		for(int i = 0; i <= 25; i++) {
			swprintf(temp, 8, L"%s%2d", (12 > i)? "-":"+", i-12);
			SendDlgItemMessage(hDlg, IDC_HOUR, CB_INSERTSTRING, -1, (LPARAM)temp);
		}
		SendDlgItemMessage(hDlg, IDC_HOUR, CB_SETCURSEL, hour, 0);
		for(int i = 0; i < 60; i++) {
			swprintf(temp, 8, L"%02d", i);
			SendDlgItemMessage(hDlg, IDC_MINUTE, CB_INSERTSTRING, -1, (LPARAM)temp);
		}
		SendDlgItemMessage(hDlg, IDC_MINUTE, CB_SETCURSEL, minute, 0);
	} else if(WM_COMMAND == message && IDC_APPLY == LOWORD(wParam)) {
		//		TODO:	revise value here.
		DWORD hour = SendDlgItemMessage(hDlg, IDC_HOUR, CB_GETCURSEL, 0, 0);
		hour = (12 > hour)? 32+hour-12:hour-12;
		DWORD minute = SendDlgItemMessage(hDlg, IDC_MINUTE, CB_GETCURSEL, 0, 0);
		obj->index = hour << 16 | minute;
	}
	return sampleProc(hDlg,message,wParam,lParam);
}

/*********************************************************************
*		COMBOBOX + EDITTEXT
*/
void ComboEditRefresh(HWND, W10PARAM*);
INT_PTR CALLBACK ComboEditProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	static DWORD sel = 0;
	if(WM_INITDIALOG == message) {
		obj = (W10PARAM *)lParam;
		//		Options
		for(DWORD i = 0; i < obj->count; i++) {
			SendDlgItemMessage(hDlg, IDC_OPTIONS, CB_INSERTSTRING,
				-1, (LPARAM)w10_LoadString(obj->options[i]));
		}
		sel = obj->index;
		SendDlgItemMessage(hDlg, IDC_OPTIONS, CB_SETCURSEL, sel, 0);
		//		Options cont.
		ComboEditRefresh(hDlg, obj);

	} else if(WM_COMMAND == message) {
		if(IDC_OPTIONS == LOWORD(wParam) && CBN_SELCHANGE == HIWORD(wParam)) {
			//		revise values immediately
			obj->index = SendDlgItemMessage(hDlg, IDC_OPTIONS, CB_GETCURSEL, 0, 0);
			if(sel != obj->index && MAXDWORD > obj->valueList[sel]) {
				obj->valueList[sel] = GetDlgItemInt(hDlg, IDC_EDIT, NULL, FALSE);
			}
			sel = obj->index;
			ComboEditRefresh(hDlg, obj);
		} else if (IDC_APPLY == LOWORD(wParam)) {
			//		TODO:	revise value here.
			obj->index = SendDlgItemMessage(hDlg, IDC_OPTIONS, CB_GETCURSEL, 0, 0);
			obj->valueList[obj->index] = GetDlgItemInt(hDlg, IDC_EDIT, NULL, FALSE);
		}
	}
	return sampleProc(hDlg,message,wParam,lParam);
}
void ComboEditRefresh(HWND hDlg, W10PARAM * obj) {
	if(MAXDWORD == obj->valueList[obj->index]) {
		SetDlgItemText(hDlg, IDC_EDIT, L"N/A");
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT), FALSE);
		SetDlgItemText(hDlg, IDC_UNIT_TEXT, L"");
	} else {
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT), TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT, obj->valueList[obj->index], FALSE);
		SetDlgItemText(hDlg, IDC_UNIT_TEXT, w10_LoadString(obj->options[obj->count + obj->index]));
	}
}

/*********************************************************************
*		STEP (EDITTEXT * 3)
*/
INT_PTR CALLBACK StepProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	if(WM_INITDIALOG == message) {
		obj = (W10PARAM *)lParam;
		//		Options
		SetDlgItemInt(hDlg, IDC_EDIT1, obj->valueList[0], FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT2, obj->valueList[1], FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT3, obj->valueList[2], FALSE);
		SetDlgItemText(hDlg, IDC_RUN, w10_LoadString(obj->options[0]));
		SetDlgItemText(hDlg, IDC_SWIM, w10_LoadString(obj->options[1]));
		SetDlgItemText(hDlg, IDC_OTHER, w10_LoadString(obj->options[2]));
		TCHAR temp[16+1] = {0};
		swprintf(temp, 16, L"(%s)", w10_LoadString(obj->options[3]));
		SetDlgItemText(hDlg, IDC_UNIT_TEXT, temp);
	} else if(WM_COMMAND == message && IDC_APPLY == LOWORD(wParam)) {
		//		TODO:	revise value here.
		obj->valueList[0] = GetDlgItemInt(hDlg, IDC_EDIT1, NULL, FALSE);
		obj->valueList[1] = GetDlgItemInt(hDlg, IDC_EDIT2, NULL, FALSE);
		obj->valueList[2] = GetDlgItemInt(hDlg, IDC_EDIT3, NULL, FALSE);
	}
	return sampleProc(hDlg,message,wParam,lParam);
}

/*********************************************************************
*		Alarm * 4
*/
void ShowLabels(HWND);
void Refresh(HWND);
void EnableAlarm(HWND);
void ReviseAlarmSetting(HWND);
INT_PTR CALLBACK AlarmProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	if(WM_INITDIALOG == message) {
		obj = (W10PARAM *)lParam;
		//		Labels
		ShowLabels(hDlg);
		//		Options
		Refresh(hDlg);
	} else if(WM_COMMAND == message) {
		//		TODO:	revise value here.
		if(IDC_APPLY == LOWORD(wParam)) {
			ReviseAlarmSetting(hDlg);
		} else if(
			IDC_ALARM1 == LOWORD(wParam) ||
			IDC_ALARM2 == LOWORD(wParam) ||
			IDC_ALARM3 == LOWORD(wParam) ||
			IDC_ALARM4 == LOWORD(wParam)) {
				EnableAlarm(hDlg);
		}
	}
	return sampleProc(hDlg,message,wParam,lParam);
}
void EnableAlarm(HWND hDlg) {
	EnableWindow(GetDlgItem(hDlg, IDC_HOUR1), IsDlgButtonChecked(hDlg, IDC_ALARM1));
	EnableWindow(GetDlgItem(hDlg, IDC_MIN1), IsDlgButtonChecked(hDlg, IDC_ALARM1));
	EnableWindow(GetDlgItem(hDlg, IDC_SUN1), IsDlgButtonChecked(hDlg, IDC_ALARM1));
	EnableWindow(GetDlgItem(hDlg, IDC_MON1), IsDlgButtonChecked(hDlg, IDC_ALARM1));
	EnableWindow(GetDlgItem(hDlg, IDC_TUE1), IsDlgButtonChecked(hDlg, IDC_ALARM1));
	EnableWindow(GetDlgItem(hDlg, IDC_WED1), IsDlgButtonChecked(hDlg, IDC_ALARM1));
	EnableWindow(GetDlgItem(hDlg, IDC_THU1), IsDlgButtonChecked(hDlg, IDC_ALARM1));
	EnableWindow(GetDlgItem(hDlg, IDC_FRI1), IsDlgButtonChecked(hDlg, IDC_ALARM1));
	EnableWindow(GetDlgItem(hDlg, IDC_SAT1), IsDlgButtonChecked(hDlg, IDC_ALARM1));

	EnableWindow(GetDlgItem(hDlg, IDC_HOUR2), IsDlgButtonChecked(hDlg, IDC_ALARM2));
	EnableWindow(GetDlgItem(hDlg, IDC_MIN2), IsDlgButtonChecked(hDlg, IDC_ALARM2));
	EnableWindow(GetDlgItem(hDlg, IDC_SUN2), IsDlgButtonChecked(hDlg, IDC_ALARM2));
	EnableWindow(GetDlgItem(hDlg, IDC_MON2), IsDlgButtonChecked(hDlg, IDC_ALARM2));
	EnableWindow(GetDlgItem(hDlg, IDC_TUE2), IsDlgButtonChecked(hDlg, IDC_ALARM2));
	EnableWindow(GetDlgItem(hDlg, IDC_WED2), IsDlgButtonChecked(hDlg, IDC_ALARM2));
	EnableWindow(GetDlgItem(hDlg, IDC_THU2), IsDlgButtonChecked(hDlg, IDC_ALARM2));
	EnableWindow(GetDlgItem(hDlg, IDC_FRI2), IsDlgButtonChecked(hDlg, IDC_ALARM2));
	EnableWindow(GetDlgItem(hDlg, IDC_SAT2), IsDlgButtonChecked(hDlg, IDC_ALARM2));

	EnableWindow(GetDlgItem(hDlg, IDC_HOUR3), IsDlgButtonChecked(hDlg, IDC_ALARM3));
	EnableWindow(GetDlgItem(hDlg, IDC_MIN3), IsDlgButtonChecked(hDlg, IDC_ALARM3));
	EnableWindow(GetDlgItem(hDlg, IDC_SUN3), IsDlgButtonChecked(hDlg, IDC_ALARM3));
	EnableWindow(GetDlgItem(hDlg, IDC_MON3), IsDlgButtonChecked(hDlg, IDC_ALARM3));
	EnableWindow(GetDlgItem(hDlg, IDC_TUE3), IsDlgButtonChecked(hDlg, IDC_ALARM3));
	EnableWindow(GetDlgItem(hDlg, IDC_WED3), IsDlgButtonChecked(hDlg, IDC_ALARM3));
	EnableWindow(GetDlgItem(hDlg, IDC_THU3), IsDlgButtonChecked(hDlg, IDC_ALARM3));
	EnableWindow(GetDlgItem(hDlg, IDC_FRI3), IsDlgButtonChecked(hDlg, IDC_ALARM3));
	EnableWindow(GetDlgItem(hDlg, IDC_SAT3), IsDlgButtonChecked(hDlg, IDC_ALARM3));

	EnableWindow(GetDlgItem(hDlg, IDC_HOUR4), IsDlgButtonChecked(hDlg, IDC_ALARM4));
	EnableWindow(GetDlgItem(hDlg, IDC_MIN4), IsDlgButtonChecked(hDlg, IDC_ALARM4));
	EnableWindow(GetDlgItem(hDlg, IDC_SUN4), IsDlgButtonChecked(hDlg, IDC_ALARM4));
	EnableWindow(GetDlgItem(hDlg, IDC_MON4), IsDlgButtonChecked(hDlg, IDC_ALARM4));
	EnableWindow(GetDlgItem(hDlg, IDC_TUE4), IsDlgButtonChecked(hDlg, IDC_ALARM4));
	EnableWindow(GetDlgItem(hDlg, IDC_WED4), IsDlgButtonChecked(hDlg, IDC_ALARM4));
	EnableWindow(GetDlgItem(hDlg, IDC_THU4), IsDlgButtonChecked(hDlg, IDC_ALARM4));
	EnableWindow(GetDlgItem(hDlg, IDC_FRI4), IsDlgButtonChecked(hDlg, IDC_ALARM4));
	EnableWindow(GetDlgItem(hDlg, IDC_SAT4), IsDlgButtonChecked(hDlg, IDC_ALARM4));
}
void Refresh(HWND hDlg) {

	CheckDlgButton(hDlg, IDC_ALARM1, 1==obj->options[0]);
	CheckDlgButton(hDlg, IDC_ALARM2, 1==obj->options[4]);
	CheckDlgButton(hDlg, IDC_ALARM3, 1==obj->options[8]);
	CheckDlgButton(hDlg, IDC_ALARM4, 1==obj->options[12]);
	
	swprintf(msg, 1024, L"Refresh()::obj->options[1]::%d", obj->options[1]);
	dmsg(msg);
	CheckDlgButton(hDlg, IDC_SUN1, 1<<0 & obj->options[1]);
	CheckDlgButton(hDlg, IDC_SUN2, 1<<0 & obj->options[5]);
	CheckDlgButton(hDlg, IDC_SUN3, 1<<0 & obj->options[9]);
	CheckDlgButton(hDlg, IDC_SUN4, 1<<0 & obj->options[13]);

	CheckDlgButton(hDlg, IDC_MON1, 1<<1 & obj->options[1]);
	CheckDlgButton(hDlg, IDC_MON2, 1<<1 & obj->options[5]);
	CheckDlgButton(hDlg, IDC_MON3, 1<<1 & obj->options[9]);
	CheckDlgButton(hDlg, IDC_MON4, 1<<1 & obj->options[13]);

	CheckDlgButton(hDlg, IDC_TUE1, 1<<2 & obj->options[1]);
	CheckDlgButton(hDlg, IDC_TUE2, 1<<2 & obj->options[5]);
	CheckDlgButton(hDlg, IDC_TUE3, 1<<2 & obj->options[9]);
	CheckDlgButton(hDlg, IDC_TUE4, 1<<2 & obj->options[13]);

	CheckDlgButton(hDlg, IDC_WED1, 1<<3 & obj->options[1]);
	CheckDlgButton(hDlg, IDC_WED2, 1<<3 & obj->options[5]);
	CheckDlgButton(hDlg, IDC_WED3, 1<<3 & obj->options[9]);
	CheckDlgButton(hDlg, IDC_WED4, 1<<3 & obj->options[13]);

	CheckDlgButton(hDlg, IDC_THU1, 1<<4 & obj->options[1]);
	CheckDlgButton(hDlg, IDC_THU2, 1<<4 & obj->options[5]);
	CheckDlgButton(hDlg, IDC_THU3, 1<<4 & obj->options[9]);
	CheckDlgButton(hDlg, IDC_THU4, 1<<4 & obj->options[13]);

	CheckDlgButton(hDlg, IDC_FRI1, 1<<5 & obj->options[1]);
	CheckDlgButton(hDlg, IDC_FRI2, 1<<5 & obj->options[5]);
	CheckDlgButton(hDlg, IDC_FRI3, 1<<5 & obj->options[9]);
	CheckDlgButton(hDlg, IDC_FRI4, 1<<5 & obj->options[13]);

	CheckDlgButton(hDlg, IDC_SAT1, 1<<6 & obj->options[1]);
	CheckDlgButton(hDlg, IDC_SAT2, 1<<6 & obj->options[5]);
	CheckDlgButton(hDlg, IDC_SAT3, 1<<6 & obj->options[9]);
	CheckDlgButton(hDlg, IDC_SAT4, 1<<6 & obj->options[13]);

	SendDlgItemMessage(hDlg, IDC_HOUR1, CB_SETCURSEL, obj->options[2], 0);
	SendDlgItemMessage(hDlg, IDC_HOUR2, CB_SETCURSEL, obj->options[6], 0);
	SendDlgItemMessage(hDlg, IDC_HOUR3, CB_SETCURSEL, obj->options[10], 0);
	SendDlgItemMessage(hDlg, IDC_HOUR4, CB_SETCURSEL, obj->options[14], 0);

	SendDlgItemMessage(hDlg, IDC_MIN1, CB_SETCURSEL, obj->options[3], 0);
	SendDlgItemMessage(hDlg, IDC_MIN2, CB_SETCURSEL, obj->options[7], 0);
	SendDlgItemMessage(hDlg, IDC_MIN3, CB_SETCURSEL, obj->options[11], 0);
	SendDlgItemMessage(hDlg, IDC_MIN4, CB_SETCURSEL, obj->options[15], 0);


	EnableAlarm(hDlg);
}
void ShowLabels(HWND hDlg) {
	DWORD len = 0;
	TCHAR temp[32+1];

	for(DWORD i = 0; i < 24; i++) {
		swprintf(temp, 32, L"%d", i);
		SendDlgItemMessage(hDlg, IDC_HOUR1, CB_INSERTSTRING, -1, (LPARAM)temp);
		SendDlgItemMessage(hDlg, IDC_HOUR2, CB_INSERTSTRING, -1, (LPARAM)temp);
		SendDlgItemMessage(hDlg, IDC_HOUR3, CB_INSERTSTRING, -1, (LPARAM)temp);
		SendDlgItemMessage(hDlg, IDC_HOUR4, CB_INSERTSTRING, -1, (LPARAM)temp);
	}
	SendDlgItemMessage(hDlg, IDC_HOUR1, CB_SETCURSEL, 0, 0);
	SendDlgItemMessage(hDlg, IDC_HOUR2, CB_SETCURSEL, 0, 0);
	SendDlgItemMessage(hDlg, IDC_HOUR3, CB_SETCURSEL, 0, 0);
	SendDlgItemMessage(hDlg, IDC_HOUR4, CB_SETCURSEL, 0, 0);

	for(DWORD i = 0; i < 60; i++) {
		swprintf(temp, 32, L"%d", i);
		SendDlgItemMessage(hDlg, IDC_MIN1, CB_INSERTSTRING, -1, (LPARAM)temp);
		SendDlgItemMessage(hDlg, IDC_MIN2, CB_INSERTSTRING, -1, (LPARAM)temp);
		SendDlgItemMessage(hDlg, IDC_MIN3, CB_INSERTSTRING, -1, (LPARAM)temp);
		SendDlgItemMessage(hDlg, IDC_MIN4, CB_INSERTSTRING, -1, (LPARAM)temp);
	}
	SendDlgItemMessage(hDlg, IDC_MIN1, CB_SETCURSEL, 0, 0);
	SendDlgItemMessage(hDlg, IDC_MIN2, CB_SETCURSEL, 0, 0);
	SendDlgItemMessage(hDlg, IDC_MIN3, CB_SETCURSEL, 0, 0);
	SendDlgItemMessage(hDlg, IDC_MIN4, CB_SETCURSEL, 0, 0);

	swprintf(temp, 32, w10_LoadString(IDS_WWE_ALARM_ID_STATE), 1);
	SetDlgItemText(hDlg, IDC_ALARM1, temp);
	swprintf(temp, 32, w10_LoadString(IDS_WWE_ALARM_ID_STATE), 2);
	SetDlgItemText(hDlg, IDC_ALARM2, temp);
	swprintf(temp, 32, w10_LoadString(IDS_WWE_ALARM_ID_STATE), 3);
	SetDlgItemText(hDlg, IDC_ALARM3, temp);
	swprintf(temp, 32, w10_LoadString(IDS_WWE_ALARM_ID_STATE), 4);
	SetDlgItemText(hDlg, IDC_ALARM4, temp);

	SetDlgItemText(hDlg, IDC_SUN1, w10_LoadString(IDS_WWE_SUN));
	SetDlgItemText(hDlg, IDC_SUN2, w10_LoadString(IDS_WWE_SUN));
	SetDlgItemText(hDlg, IDC_SUN3, w10_LoadString(IDS_WWE_SUN));
	SetDlgItemText(hDlg, IDC_SUN4, w10_LoadString(IDS_WWE_SUN));

	SetDlgItemText(hDlg, IDC_MON1, w10_LoadString(IDS_WWE_MON));
	SetDlgItemText(hDlg, IDC_MON2, w10_LoadString(IDS_WWE_MON));
	SetDlgItemText(hDlg, IDC_MON3, w10_LoadString(IDS_WWE_MON));
	SetDlgItemText(hDlg, IDC_MON4, w10_LoadString(IDS_WWE_MON));

	SetDlgItemText(hDlg, IDC_TUE1, w10_LoadString(IDS_WWE_TUE));
	SetDlgItemText(hDlg, IDC_TUE2, w10_LoadString(IDS_WWE_TUE));
	SetDlgItemText(hDlg, IDC_TUE3, w10_LoadString(IDS_WWE_TUE));
	SetDlgItemText(hDlg, IDC_TUE4, w10_LoadString(IDS_WWE_TUE));

	SetDlgItemText(hDlg, IDC_WED1, w10_LoadString(IDS_WWE_WED));
	SetDlgItemText(hDlg, IDC_WED2, w10_LoadString(IDS_WWE_WED));
	SetDlgItemText(hDlg, IDC_WED3, w10_LoadString(IDS_WWE_WED));
	SetDlgItemText(hDlg, IDC_WED4, w10_LoadString(IDS_WWE_WED));

	SetDlgItemText(hDlg, IDC_THU1, w10_LoadString(IDS_WWE_THU));
	SetDlgItemText(hDlg, IDC_THU2, w10_LoadString(IDS_WWE_THU));
	SetDlgItemText(hDlg, IDC_THU3, w10_LoadString(IDS_WWE_THU));
	SetDlgItemText(hDlg, IDC_THU4, w10_LoadString(IDS_WWE_THU));

	SetDlgItemText(hDlg, IDC_FRI1, w10_LoadString(IDS_WWE_FRI));
	SetDlgItemText(hDlg, IDC_FRI2, w10_LoadString(IDS_WWE_FRI));
	SetDlgItemText(hDlg, IDC_FRI3, w10_LoadString(IDS_WWE_FRI));
	SetDlgItemText(hDlg, IDC_FRI4, w10_LoadString(IDS_WWE_FRI));
	
	SetDlgItemText(hDlg, IDC_SAT1, w10_LoadString(IDS_WWE_SAT));
	SetDlgItemText(hDlg, IDC_SAT2, w10_LoadString(IDS_WWE_SAT));
	SetDlgItemText(hDlg, IDC_SAT3, w10_LoadString(IDS_WWE_SAT));
	SetDlgItemText(hDlg, IDC_SAT4, w10_LoadString(IDS_WWE_SAT));
}
void ReviseAlarmSetting(HWND hDlg) {
	obj->options[0] = (IsDlgButtonChecked(hDlg, IDC_ALARM1))? 1:0;
	if(IsDlgButtonChecked(hDlg, IDC_SUN1))		obj->options[1] |= 1<<0;
	if(IsDlgButtonChecked(hDlg, IDC_MON1))		obj->options[1] |= 1<<1;
	if(IsDlgButtonChecked(hDlg, IDC_TUE1))		obj->options[1] |= 1<<2;
	if(IsDlgButtonChecked(hDlg, IDC_WED1))		obj->options[1] |= 1<<3;
	if(IsDlgButtonChecked(hDlg, IDC_THU1))		obj->options[1] |= 1<<4;
	if(IsDlgButtonChecked(hDlg, IDC_FRI1))		obj->options[1] |= 1<<5;
	if(IsDlgButtonChecked(hDlg, IDC_SAT1))		obj->options[1] |= 1<<6;
	swprintf(msg, 1024, L"%d", obj->options[1]);
	dmsg(msg);
	obj->options[2] = SendDlgItemMessage(hDlg, IDC_HOUR1, CB_GETCURSEL, 0, 0);
	obj->options[3] = SendDlgItemMessage(hDlg, IDC_MIN1, CB_GETCURSEL, 0, 0);

	obj->options[4] = (IsDlgButtonChecked(hDlg, IDC_ALARM2))? 1:0;
	if(IsDlgButtonChecked(hDlg, IDC_SUN2))		obj->options[5] |= 1<<0;
	if(IsDlgButtonChecked(hDlg, IDC_MON2))		obj->options[5] |= 1<<1;
	if(IsDlgButtonChecked(hDlg, IDC_TUE2))		obj->options[5] |= 1<<2;
	if(IsDlgButtonChecked(hDlg, IDC_WED2))		obj->options[5] |= 1<<3;
	if(IsDlgButtonChecked(hDlg, IDC_THU2))		obj->options[5] |= 1<<4;
	if(IsDlgButtonChecked(hDlg, IDC_FRI2))		obj->options[5] |= 1<<5;
	if(IsDlgButtonChecked(hDlg, IDC_SAT2))		obj->options[5] |= 1<<6;
	obj->options[6] = SendDlgItemMessage(hDlg, IDC_HOUR2, CB_GETCURSEL, 0, 0);
	obj->options[7] = SendDlgItemMessage(hDlg, IDC_MIN2, CB_GETCURSEL, 0, 0);

	obj->options[8] = (IsDlgButtonChecked(hDlg, IDC_ALARM3))? 1:0;
	if(IsDlgButtonChecked(hDlg, IDC_SUN3))		obj->options[9] |= 1<<0;
	if(IsDlgButtonChecked(hDlg, IDC_MON3))		obj->options[9] |= 1<<1;
	if(IsDlgButtonChecked(hDlg, IDC_TUE3))		obj->options[9] |= 1<<2;
	if(IsDlgButtonChecked(hDlg, IDC_WED3))		obj->options[9] |= 1<<3;
	if(IsDlgButtonChecked(hDlg, IDC_THU3))		obj->options[9] |= 1<<4;
	if(IsDlgButtonChecked(hDlg, IDC_FRI3))		obj->options[9] |= 1<<5;
	if(IsDlgButtonChecked(hDlg, IDC_SAT3))		obj->options[9] |= 1<<6;
	obj->options[10] = SendDlgItemMessage(hDlg, IDC_HOUR3, CB_GETCURSEL, 0, 0);
	obj->options[11] = SendDlgItemMessage(hDlg, IDC_MIN3, CB_GETCURSEL, 0, 0);

	obj->options[12] = (IsDlgButtonChecked(hDlg, IDC_ALARM4))? 1:0;
	if(IsDlgButtonChecked(hDlg, IDC_SUN4))		obj->options[13] |= 1<<0;
	if(IsDlgButtonChecked(hDlg, IDC_MON4))		obj->options[13] |= 1<<1;
	if(IsDlgButtonChecked(hDlg, IDC_TUE4))		obj->options[13] |= 1<<2;
	if(IsDlgButtonChecked(hDlg, IDC_WED4))		obj->options[13] |= 1<<3;
	if(IsDlgButtonChecked(hDlg, IDC_THU4))		obj->options[13] |= 1<<4;
	if(IsDlgButtonChecked(hDlg, IDC_FRI4))		obj->options[13] |= 1<<5;
	if(IsDlgButtonChecked(hDlg, IDC_SAT4))		obj->options[13] |= 1<<6;
	obj->options[14] = SendDlgItemMessage(hDlg, IDC_HOUR4, CB_GETCURSEL, 0, 0);
	obj->options[15] = SendDlgItemMessage(hDlg, IDC_MIN4, CB_GETCURSEL, 0, 0);
}


/*********************************************************************
*		Weight & Height
*/
INT_PTR CALLBACK WHProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	if(WM_INITDIALOG == message) {
		obj = (W10PARAM *)lParam;
		//		Options
		SetDlgItemText(hDlg, IDC_WEIGHT, w10_LoadString(IDS_WWE_WEIGHT));
		SetDlgItemText(hDlg, IDC_HEIGHT, w10_LoadString(IDS_WWE_HEIGHT));
		SetDlgItemText(hDlg, IDC_CM, w10_LoadString(IDS_WWE_CM));
		SetDlgItemText(hDlg, IDC_POUND, w10_LoadString(IDS_WWE_POUND));
		SetDlgItemText(hDlg, IDC_KG, w10_LoadString(IDS_WWE_KG));
		SetDlgItemText(hDlg, IDC_INCH, w10_LoadString(IDS_WWE_INCH));

		SetDlgItemInt(hDlg, IDC_WEIGHT_KG, obj->valueList[0], FALSE);
		SetDlgItemInt(hDlg, IDC_WEIGHT_POUND, (DWORD)ceil(2.2046 * obj->valueList[0] - 0.4), FALSE);

		SetDlgItemInt(hDlg, IDC_HEIGHT_CM, obj->valueList[1], FALSE);
		SetDlgItemInt(hDlg, IDC_HEIGHT_INCH, (DWORD)ceil(0.3937 * obj->valueList[1] - 0.4), FALSE);

	} else if(WM_COMMAND == message) {
		if(EN_CHANGE == HIWORD(wParam)) {
			DWORD v0 = GetDlgItemInt(hDlg, LOWORD(wParam), NULL, FALSE);
			v0 = (DWORD)ceil(v0 - 0.4);
			DWORD v1 = 0;
			switch(LOWORD(wParam)) {
				case IDC_WEIGHT_KG:
					v1 = (DWORD)ceil(2.20462262 * v0 - 0.4);		// kg2pound
					SetDlgItemInt(hDlg, IDC_WEIGHT_POUND, v1, FALSE);
					break;
				/*case IDC_WEIGHT_POUND:
					v1 = ceil((double) v0 / 2.20462262 - 0.4);		// pound2kg
					SetDlgItemInt(hDlg, IDC_WEIGHT_KG, v1, FALSE);
					break;*/
				case IDC_HEIGHT_CM:
					v1 = (DWORD)ceil(0.3937 * v0 - 0.4);		// cm2inch
					SetDlgItemInt(hDlg, IDC_HEIGHT_INCH, v1, FALSE);
					break;
				/*case IDC_HEIGHT_INCH:
					v1 = ceil((double) v0 / 0.3937 - 0.4);		// inch2cm
					SetDlgItemInt(hDlg, IDC_HEIGHT_CM, v1, FALSE);
					break;*/
			}
		} else if(IDC_APPLY == LOWORD(wParam)) {
			//		TODO:	revise value here.
			obj->valueList[0] = GetDlgItemInt(hDlg, IDC_WEIGHT_KG, NULL, FALSE);
			obj->valueList[1] = GetDlgItemInt(hDlg, IDC_HEIGHT_CM, NULL, FALSE);
		}
	}
	return sampleProc(hDlg,message,wParam,lParam);
}

