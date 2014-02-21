// Settings.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Settings.h"
#include "mmi_frame.h"
#include "resource.h"
#include "w10Param.h"
#include "dlgProcs.h"

#include "math.h"
#include "ComIO.h"

extern HINSTANCE g_hInst;
HWND g_hDlg;
GLOBAL_SETTING g_Setting;
GLOBAL_TRAINING g_Training;

BOOL checkDoneTable(DWORD id);
/*
INT_PTR CALLBACK	SettingProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	InitParamW10();
	g_hInst = hInstance;
	memset(&g_Setting, 0, sizeof(GLOBAL_SETTING));
	swprintf(msg, 1024, L"%d", sizeof(GLOBAL_SETTING));
	dmsg(msg);

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_SETTINGS), NULL, SettingProc);
	return 0;
}
*/
static ComIO * p;

BOOL getSetting(void);
BOOL getTraining(void);
BOOL setSetting(void);
BOOL setTraining(void);

void RefreshSettings(void);
void reverseBool(unsigned int&);
// Message handler for about box.

INT_PTR CALLBACK SettingProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		p = (ComIO*)lParam;
		if( ! getSetting() ||
			! getTraining()) {
				EndDialog(hDlg, LOWORD(wParam));
		} else if(0) {
			DWORD len = 0;
			msg[0] = '\0';
			for (DWORD i = 0; i < sizeof(g_Setting); i++) {
				len = wcslen(msg);
				if (len > 1024)
					break;
				swprintf(msg + len, 1024 - len, L"%02x ", (unsigned char)*((char*)&g_Setting + i));
				if (0 == (1 + i) % 16) {
					dmsg(msg);
					msg[0] = '\0';
				}
			}
			dmsg(msg);
		}
		g_hDlg = hDlg;
		RefreshSettings();
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam)) {
			case IDCANCEL:
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			case IDOK:
				//		TODO: Revise Settings
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;

			case IDC_APPLY:
				setSetting();
				setTraining();
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;


			/*********************************************************************
			*		BOOL
			*			Settings
			*/
			case IDC_KEYTONE:
				reverseBool(g_Setting.KeyTone);
				break;
			case IDC_MESSAGETONE:
				reverseBool(g_Setting.MessageTone);
				break;
			case IDC_VIBRATION:
				reverseBool(g_Setting.Vibration);
				break;
			case IDC_ORIENTATION:
				reverseBool(g_Setting.Orientation);
				break;
			case IDC_USER_LOCATION:
				reverseBool(g_Setting.UserLocation);
				break;
			case IDC_SHOW_MAP:
				reverseBool(g_Setting.ShowMap);
				break;
			case IDC_ANT_WEIGHT_SCALE:
				reverseBool(g_Setting.ANTWeight);
				break;
			case IDC_ANT_FITNESS_EQUIPMENT:
				reverseBool(g_Setting.ANTEquipment);
				break;
			case IDC_TIME_FORMAT:
				reverseBool(g_Setting.TimeFormat);
				break;
			case IDC_DAYLIGHT_SAVING_1:
				reverseBool(g_Setting.TimeSaving);
				break;
			case IDC_UNIT:
				//		change all units
				reverseBool(g_Setting.DistanceUnit);
				g_Setting.ElevationUnit = g_Setting.WeightUnit = g_Setting.SpeedUnit = g_Setting.DistanceUnit;
				break;
			case IDC_VIRTUAL_PARTNER:
				reverseBool(g_Setting.VPEnable);
				break;
			case IDC_PACE_SPEED:
				reverseBool(g_Setting.PaceSpeed);
				break;
			case IDC_ANT_HEART_RATE_MONITOR:
				reverseBool(g_Setting.ANTHeartRate);
				break;
			case IDC_ANT_HEART_RATE_DISPLAY:
				reverseBool(g_Setting.ANTHRDisplay);
				break;
			case IDC_ANT_FOOT_PAD:
				reverseBool(g_Setting.ANTFootPod);
				break;
			case IDC_ANT_FOOT_PAD_SOURCE:
				reverseBool(g_Setting.ANTFootPadSource);
				break;
			case IDC_ANT_BIKE:
				reverseBool(g_Setting.ANTBikeAuto);
				break;
			case IDC_ANT_BIKE_SENSOR:
				reverseBool(g_Setting.ANTBikeSensor);
				break;
			case IDC_ANT_POWER_METER_SENSOR:
				reverseBool(g_Setting.ANTBikePower);
				break;
			case IDC_GSENSOR_COMPASS_SENSOR:
				reverseBool(g_Setting.GSensorOn);
				break;
			case IDC_BLE_FINDER:
				reverseBool(g_Setting.BLEFinder);
				break;
			case IDC_BLE_HEART_RATE:
				reverseBool(g_Setting.BLEHR);
				break;
			case IDC_DAYLIGHT_SAVING_2:
				reverseBool(g_Setting.TimeSaving2);
				break;
			case IDC_GPS_TIMER_AUTO_UPDATE:
				reverseBool(g_Setting.GPSAutoUpdate);
				break;
			case IDC_BAROMETER_SENSOR:
				reverseBool(g_Setting.BarometerOn);
				break;
			case IDC_THEME:
				reverseBool(g_Setting.Themes);
				break;
			case IDC_METRONOME_STATE:
				reverseBool(g_Setting.Metronome);
				break;


			/*********************************************************************
			*		BOOL
			*			Training
			*/
			case IDC_WARM_UP:
				reverseBool(g_Training.WarmUp);
				break;
			case IDC_COOL_DOWN:
				reverseBool(g_Training.CoolDown);
				break;
			case IDC_GENDER:
				reverseBool(g_Training.Gender);
				break;
			case IDC_LIFETIME_ATHLETE:
				reverseBool(g_Training.LFAthlete);
				break;


			/*********************************************************************
			*		COMBOBOX
			*/
			case IDC_MODE:
				if(0 < g_Setting.Mode && 6 > g_Setting.Mode) {
					mode.index = g_Setting.Mode - 1;
				} else {
					mode.index = 0;
				}
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_COMBO_LIST), hDlg, ComboListProc, (LPARAM)&mode);
				g_Setting.Mode = mode.index + 1;
				break;

			case IDC_LANG:
				if(0 < g_Setting.Language && 4 > g_Setting.Language) {
					lang.index = g_Setting.Language - 1;
				} else {
					lang.index = 0;
				}
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_COMBO_LIST), hDlg, ComboListProc, (LPARAM)&lang);
				g_Setting.Language = (0 == lang.index)? lang.index:lang.index + 1;
				break;

			case IDC_BK_LEVEL:
				if(0 <= g_Setting.BacklightLevel && 3 > g_Setting.BacklightLevel) {
					bklevel.index = g_Setting.BacklightLevel;
				} else {
					bklevel.index = 0;
				}
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_COMBO_LIST), hDlg, ComboListProc, (LPARAM)&bklevel);
				g_Setting.BacklightLevel = bklevel.index;
				break;

			case IDC_RECORD_DATA_POINTS:
				if(0 <= g_Setting.RecordData && 3 > g_Setting.RecordData) {
					recorddata.index = g_Setting.RecordData;
				} else {
					recorddata.index = 0;
				}
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_COMBO_LIST), hDlg, ComboListProc, (LPARAM)&recorddata);
				g_Setting.RecordData = recorddata.index;
				break;

			case IDC_POSITION_UNIT:
				if(0 <= g_Setting.PositionUnit && 3 > g_Setting.PositionUnit) {
					position.index = g_Setting.PositionUnit;
				} else {
					position.index = 0;
				}
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_COMBO_LIST), hDlg, ComboListProc, (LPARAM)&position);
				g_Setting.PositionUnit = position.index;
				break;
			
			case IDC_AUTO_SCROLL:
				if(0 <= g_Setting.AutoScroll && 4 > g_Setting.AutoScroll) {
					autoscroll.index = g_Setting.AutoScroll;
				} else {
					autoscroll.index = 0;
				}
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_COMBO_LIST), hDlg, ComboListProc, (LPARAM)&autoscroll);
				g_Setting.AutoScroll = autoscroll.index;
				break;

			case IDC_AUTO_LAP_EXT:
				if(0 <= g_Setting.LapAtPosition && 3 > g_Setting.LapAtPosition) {
					autolapext.index = g_Setting.LapAtPosition;
				} else {
					autolapext.index = 0;
				}
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_COMBO_LIST), hDlg, ComboListProc, (LPARAM)&autolapext);
				g_Setting.LapAtPosition = autolapext.index;
				break;

			/*********************************************************************
			*		TIMEZONE
			*/
			case IDC_TIME_ZONE_1:
				timezone1.index = g_Setting.TimeZone;
				swprintf(msg, 1024, L"%d", g_Setting.TimeZone);
				dmsg(msg);
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_TIMEZONE), hDlg, TimeZoneProc, (LPARAM)&timezone1);
				g_Setting.TimeZone = timezone1.index;
				break;
			case IDC_TIME_ZONE_2:
				timezone2.index = g_Setting.TimeZone2;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_TIMEZONE), hDlg, TimeZoneProc, (LPARAM)&timezone2);
				g_Setting.TimeZone2 = timezone2.index;
				break;

				
			/*********************************************************************
			*		EDITTEXT
			*/
			case IDC_BK_TIMEOUT:
				bktimeout.index = g_Setting.BacklightTimeout;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_EDIT), hDlg, EditProc, (LPARAM)&bktimeout);
				g_Setting.BacklightTimeout = bktimeout.index;
				break;

			case IDC_ANT_BIKE_WEIGHT:
				antbikeweight.index = g_Setting.ANTBikeWeight;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_EDIT), hDlg, EditProc, (LPARAM)&antbikeweight);
				g_Setting.ANTBikeWeight = antbikeweight.index;
				break;

			case IDC_ANT_BIKE_ODOMETER:
				antbikeodometer.index = g_Setting.ANTBikeOdometer;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_EDIT), hDlg, EditProc, (LPARAM)&antbikeodometer);
				g_Setting.ANTBikeOdometer = antbikeodometer.index;
				break;


			case IDC_ANT_BIKE_WHEEL_SIZE:
				antbikewheel.index = g_Setting.ANTBikeWheel;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_EDIT), hDlg, EditProc, (LPARAM)&antbikewheel);
				g_Setting.ANTBikeWheel = antbikewheel.index;
				break;

			case IDC_BT_NOTIFY_TONE_TIME:
				btspptone.index = g_Setting.BTSPPTone;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_EDIT), hDlg, EditProc, (LPARAM)&btspptone);
				g_Setting.BTSPPTone = btspptone.index;
				break;
				
			case IDC_BT_NOTIFY_VIBRATION_TIME:
				btsppvibration.index = g_Setting.BTSPPVibration;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_EDIT), hDlg, EditProc, (LPARAM)&btsppvibration);
				g_Setting.BTSPPVibration = btsppvibration.index;
				break;

			case IDC_METRONOME_INTERVAL:
				metronomeInterval.index = g_Setting.MetronomeTime;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_EDIT), hDlg, EditProc, (LPARAM)&metronomeInterval);
				g_Setting.MetronomeTime = metronomeInterval.index;
				break;

			case IDC_METRONOME_RING_TIME:
				metronomeRing.index = g_Setting.MetronomeRing;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_EDIT), hDlg, EditProc, (LPARAM)&metronomeRing);
				g_Setting.MetronomeRing = metronomeRing.index;
				break;

			case IDC_REPTS:
				reps.index = g_Training.Reps;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_EDIT), hDlg, EditProc, (LPARAM)&reps);
				g_Training.Reps = reps.index;
				break;

			case IDC_AGE:
				age.index = g_Training.Age;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_EDIT), hDlg, EditProc, (LPARAM)&age);
				g_Training.Age = age.index;
				break;


			/*********************************************************************
			*		COMBOBOX + EDITTEXT
			*/
			case IDC_AUTO_LAP:
				autolap.index = g_Setting.AutoLap;
				autolap.vListCount = 4;
				autolap.valueList = new DWORD[autolap.count];
				autolap.valueList[0] = MAXDWORD;
				autolap.valueList[1] = g_Setting.LapAtTime;
				autolap.valueList[2] = g_Setting.LapAtDistance;
				autolap.valueList[3] = MAXDWORD;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_COMBO_EDIT), hDlg, ComboEditProc, (LPARAM)&autolap);
				g_Setting.AutoLap = autolap.index;
				g_Setting.LapAtTime = autolap.valueList[1];
				g_Setting.LapAtDistance = autolap.valueList[2];
				delete [] autolap.valueList;
				break;
			case IDC_AUTO_PAUSE:
				autopause.index = g_Setting.AutoPause;
				autopause.valueList = new DWORD[autopause.count];
				autopause.valueList[0] = MAXDWORD;
				autopause.valueList[1] = MAXDWORD;
				autopause.valueList[2] = g_Setting.PauseTime;
				autopause.valueList[3] = g_Setting.PauseSpeed;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_COMBO_EDIT), hDlg, ComboEditProc, (LPARAM)&autopause);
				g_Setting.AutoPause = autopause.index;
				g_Setting.PauseTime = autopause.valueList[2];
				g_Setting.PauseSpeed = autopause.valueList[3];
				delete [] autopause.valueList;
				break;

			case IDC_TIME_ALERT:
				timeAlert.index = g_Training.TimeAlert;
				timeAlert.vListCount = 2;
				timeAlert.valueList = new DWORD [timeAlert.count];
				timeAlert.valueList[0] = MAXDWORD;
				timeAlert.valueList[1] = g_Training.TimeAlertTime;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_COMBO_EDIT), hDlg, ComboEditProc, (LPARAM)&timeAlert);
				g_Training.TimeAlert = timeAlert.index;
				g_Training.TimeAlertTime = timeAlert.valueList[1];
				delete [] timeAlert.valueList;
				break;

			case IDC_DIST_ALERT:
				distAlert.index = g_Training.DistanceAlert;
				distAlert.vListCount = 2;
				distAlert.valueList = new DWORD [distAlert.count];
				distAlert.valueList[0] = MAXDWORD;
				distAlert.valueList[1] = g_Training.DistanceAlertValue;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_COMBO_EDIT), hDlg, ComboEditProc, (LPARAM)&distAlert);
				g_Training.DistanceAlert = distAlert.index;
				g_Training.DistanceAlertValue = distAlert.valueList[1];
				delete [] distAlert.valueList;
				break;
				
			case IDC_CALORIE_ALERT:
				calorieAlert.index = g_Training.ColorieAlert;
				calorieAlert.vListCount = 2;
				calorieAlert.valueList = new DWORD [calorieAlert.count];
				calorieAlert.valueList[0] = MAXDWORD;
				calorieAlert.valueList[1] = g_Training.ColorieAlertValue;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_COMBO_EDIT), hDlg, ComboEditProc, (LPARAM)&calorieAlert);
				g_Training.ColorieAlert = calorieAlert.index;
				g_Training.ColorieAlertValue = calorieAlert.valueList[1];
				delete [] calorieAlert.valueList;
				break;

			/*case IDC_HR_ALERT:
				heartRateAlert.index = g_Training.HRAlert;
				heartRateAlert.vListCount = 2;
				heartRateAlert.valueList = new DWORD [heartRateAlert.count];
				heartRateAlert.valueList[0] = MAXDWORD;
				heartRateAlert.valueList[1] = g_Training.HRAlertValue;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_COMBO_EDIT), hDlg, ComboEditProc, (LPARAM)&heartRateAlert);
				g_Training.HRAlert = heartRateAlert.index;
				g_Training.HRAlertValue= heartRateAlert.valueList[1];
				delete [] heartRateAlert.valueList;
				break;*/
			case IDC_HR_ALERT_MIN:
				heartRateAlertMin.index = g_Training.HRMinAlert;
				heartRateAlertMin.vListCount = 2;
				heartRateAlertMin.valueList = new DWORD [heartRateAlertMin.count];
				heartRateAlertMin.valueList[0] = MAXDWORD;
				heartRateAlertMin.valueList[1] = g_Training.HRMinAlertValue;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_COMBO_EDIT), hDlg, ComboEditProc, (LPARAM)&heartRateAlertMin);
				g_Training.HRMinAlert = heartRateAlertMin.index;
				g_Training.HRMinAlertValue= heartRateAlertMin.valueList[1];
				delete [] heartRateAlertMin.valueList;
				break;
			case IDC_HR_ALERT_MAX:
				heartRateAlertMax.index = g_Training.HRMaxAlert;
				heartRateAlertMax.vListCount = 2;
				heartRateAlertMax.valueList = new DWORD [heartRateAlertMax.count];
				heartRateAlertMax.valueList[0] = MAXDWORD;
				heartRateAlertMax.valueList[1] = g_Training.HRMaxAlertValue;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_COMBO_EDIT), hDlg, ComboEditProc, (LPARAM)&heartRateAlertMax);
				g_Training.HRMaxAlert = heartRateAlertMax.index;
				g_Training.HRMaxAlertValue= heartRateAlertMax.valueList[1];
				delete [] heartRateAlertMax.valueList;
				break;

			case IDC_INTERVAL:
				interval.index = g_Training.IntervalType;
				interval.vListCount = 2;
				interval.valueList = new DWORD [interval.count];
				interval.valueList[0] = g_Training.IntervalTime;
				interval.valueList[1] = g_Training.IntervalDistance;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_COMBO_EDIT), hDlg, ComboEditProc, (LPARAM)&interval);
				g_Training.IntervalType = interval.index;
				g_Training.IntervalTime= interval.valueList[0];
				g_Training.IntervalDistance= interval.valueList[1];
				delete [] interval.valueList;
				break;

			case IDC_REST:
				rest.index = g_Training.RestType;
				rest.vListCount = 2;
				rest.valueList = new DWORD [rest.count];
				rest.valueList[0] = g_Training.RestTime;
				rest.valueList[1] = g_Training.RestDistance;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_COMBO_EDIT), hDlg, ComboEditProc, (LPARAM)&rest);
				g_Training.RestType = rest.index;
				g_Training.RestTime= rest.valueList[0];
				g_Training.RestDistance= rest.valueList[1];
				delete [] rest.valueList;
				break;


			/*********************************************************************
			*		EDITTEXT * 3
			*/
			case IDC_ANT_FOOT_STEP_DISTANCE:
				antfootstep.vListCount = 3;
				antfootstep.valueList = new DWORD[3];
				antfootstep.valueList[0] = g_Setting.ANTFootPodRunDistance;
				antfootstep.valueList[1] = g_Setting.ANTFootPodSwimDistance;
				antfootstep.valueList[2] = g_Setting.ANTFootPodOtherDistance;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_STEP), hDlg, StepProc, (LPARAM)&antfootstep);
				g_Setting.ANTFootPodRunDistance = antfootstep.valueList[0];
				g_Setting.ANTFootPodSwimDistance = antfootstep.valueList[1];
				g_Setting.ANTFootPodOtherDistance = antfootstep.valueList[2];
				delete [] antfootstep.valueList;
				break;

			case IDC_GSENSOR_STEP_DISTANCE:
				gsensorstep.vListCount = 3;
				gsensorstep.valueList = new DWORD[3];
				gsensorstep.valueList[0] = g_Setting.GSensorRunDistance;
				gsensorstep.valueList[1] = g_Setting.GSensorSwimDistance;
				gsensorstep.valueList[2] = g_Setting.GSensorOtherDistance;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_STEP), hDlg, StepProc, (LPARAM)&gsensorstep);
				g_Setting.GSensorRunDistance = gsensorstep.valueList[0];
				g_Setting.GSensorSwimDistance = gsensorstep.valueList[1];
				g_Setting.GSensorOtherDistance = gsensorstep.valueList[2];
				delete [] gsensorstep.valueList;
				break;

				
			case IDC_ALARM:
				memset(alarm4.options, 0, sizeof(alarm4.options));
				{
					int hour = g_Setting.TimeZone >> 16;
					hour = (hour > 20)? hour-8:hour;
					unsigned int min = g_Setting.TimeZone & 0xff;
					for(DWORD i = 0; i < 4; i++) {
						alarm4.options[4 * i + 0] = g_Setting.AlarmOn[i];
						alarm4.options[4 * i + 1] = g_Setting.AlarmWeek[i];
						alarm4.options[4 * i + 2] = (g_Setting.AlarmHour[i] + hour)%24;
						alarm4.options[4 * i + 3] = (g_Setting.AlarmMin[i] + min)%60;
					}
					DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_ALARM), hDlg, AlarmProc, (LPARAM)&alarm4);
					for(DWORD i = 0; i < 4; i++) {
						g_Setting.AlarmOn[i] = alarm4.options[4 * i + 0];
						g_Setting.AlarmWeek[i] = alarm4.options[4 * i + 1];
						g_Setting.AlarmHour[i] = (alarm4.options[4 * i + 2] + 24 - hour)%24;
						g_Setting.AlarmMin[i] = (alarm4.options[4 * i + 3] + 60 - min)%60;
					}
				}
				break;

			case IDC_WEIGHT_HEIGHT:
				weight_height.vListCount = 2;
				weight_height.valueList = new DWORD[2];
				weight_height.valueList[0] = g_Training.Weight;
				weight_height.valueList[1] = g_Training.Height;
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_WEIGHT_HEIGHT), hDlg, WHProc, (LPARAM)&weight_height);
				g_Training.Weight = weight_height.valueList[0];
				g_Training.Height = weight_height.valueList[1];
				break;

			default:
				break;
		}
		RefreshSettings();
		break;
	}
	return (INT_PTR)FALSE;
}


TCHAR* w10_LoadString(DWORD id);
void fillTrueFalse(DWORD id, DWORD value, DWORD title, DWORD no, DWORD yes);
void RefreshSettings(void) {
	TCHAR temp[1024+1];
	DWORD len = 0;

	//		Mode: 1-5
	swprintf(temp, 1024, L"%s", w10_LoadString(IDS_WWE_MODE));
	if(0 < g_Setting.Mode && 6 > g_Setting.Mode) {
		len = wcslen(temp);
		swprintf(temp + len, 1024 - len, L" %s", w10_LoadString(mode.options[g_Setting.Mode - 1]));
	}
	SetDlgItemText(g_hDlg, IDC_MODE, temp);

	//		Language: 0, 2, 3
	swprintf(temp, 1024, L"%s", w10_LoadString(IDS_WWE_LANGUAGE));
	len = wcslen(temp);
	if(0 == g_Setting.Language) {
		swprintf(temp + len, 1024 - len, L" %s", w10_LoadString(IDS_WWE_WWE));
	} else if(2 == g_Setting.Language) {
		swprintf(temp + len, 1024 - len, L" %s", w10_LoadString(IDS_WWE_CHT));
	} else if(3 == g_Setting.Language) {
		swprintf(temp + len, 1024 - len, L" %s", w10_LoadString(IDS_WWE_CHS));
	}
	SetDlgItemText(g_hDlg, IDC_LANG, temp);

	//		Key Tone
	fillTrueFalse(IDC_KEYTONE, g_Setting.KeyTone, IDS_WWE_KEY_TONE, IDS_WWE_OFF, IDS_WWE_ON);

	//		Message Tone
	fillTrueFalse(IDC_MESSAGETONE, g_Setting.MessageTone, IDS_WWE_MESSAGE_TONE, IDS_WWE_OFF, IDS_WWE_ON);

	//		Vibration
	fillTrueFalse(IDC_VIBRATION, g_Setting.Vibration, IDS_WWE_VIBRATION, IDS_WWE_OFF, IDS_WWE_ON);

	//		Backlight Timeout
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_BACKLIGHT_TIMEOUT));
	len = wcslen(temp);
	if(0 < g_Setting.BacklightTimeout) {
		swprintf(temp + len, 1024 - len, L"%d %s", g_Setting.BacklightTimeout, w10_LoadString(IDS_WWE_SECONDS));
	} else if(0 == g_Setting.BacklightTimeout) {
		swprintf(temp + len, 1024 - len, L"%s", w10_LoadString(IDS_WWE_STAYS_ON));
	}
	SetDlgItemText(g_hDlg, IDC_BK_TIMEOUT, temp);

	//		Backlight Level
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_BACKLIGHT_LEVEL));
	len = wcslen(temp);
	if(0 <= g_Setting.BacklightLevel && 3 > g_Setting.BacklightLevel) {
		swprintf(temp + len, 1024 - len, L"%s", w10_LoadString(bklevel.options[g_Setting.BacklightLevel]));
	}
	SetDlgItemText(g_hDlg, IDC_BK_LEVEL, temp);

	//		Orientation
	fillTrueFalse(IDC_ORIENTATION, g_Setting.Orientation, IDS_WWE_ORIENTATION, IDS_WWE_NORTH_UP, IDS_WWE_TRACK_UP);

	//		User Location
	fillTrueFalse(IDC_USER_LOCATION, g_Setting.UserLocation, IDS_WWE_USER_LOCATION, IDS_WWE_HIDE, IDS_WWE_SHOW);

	//		Show Map
	fillTrueFalse(IDC_SHOW_MAP, g_Setting.ShowMap, IDS_WWE_SHOW_MAP, IDS_WWE_NO, IDS_WWE_YES);

	//		ANT+ Weight Scale
	fillTrueFalse(IDC_ANT_WEIGHT_SCALE, g_Setting.ANTWeight, IDS_WWE_ANT_WEIGHT_SCALE, IDS_WWE_NO, IDS_WWE_YES);

	//		ANT+ Equipment
	fillTrueFalse(IDC_ANT_FITNESS_EQUIPMENT, g_Setting.ANTEquipment, IDS_WWE_ANT_FITNESS_EQUIPMENT, IDS_WWE_NO, IDS_WWE_YES);

	//		Record Data
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_RECORD_DATA_POINT));
	len = wcslen(temp);
	if(0 <= g_Setting.RecordData && 3 > g_Setting.RecordData) {
		swprintf(temp + len, 1024 - len, L"%s", w10_LoadString(recorddata.options[g_Setting.RecordData]));
	}
	SetDlgItemText(g_hDlg, IDC_RECORD_DATA_POINTS, temp);

	//		Time Format
	fillTrueFalse(IDC_TIME_FORMAT, g_Setting.TimeFormat, IDS_WWE_TIME_FORMAT, IDS_WWE_HOUR_12, IDS_WWE_HOUR_24);

	//		Time Zone 1
	DWORD hour = g_Setting.TimeZone >> 16;
	DWORD minute = g_Setting.TimeZone & 0xff;
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_TIME_ZONE_1));
	len = wcslen(temp);
	swprintf(temp + len, 1024 - len, L"%s%d:%02d",
		(20 > hour)? "+":"-", (20 > hour)? hour:32-hour, minute);
	SetDlgItemText(g_hDlg, IDC_TIME_ZONE_1, temp);

	//		DayLight Saving 1
	fillTrueFalse(IDC_DAYLIGHT_SAVING_1, g_Setting.TimeSaving, IDS_WWE_TIME_SAVING_1, IDS_WWE_NO, IDS_WWE_YES);

	//		Unit
	fillTrueFalse(IDC_UNIT, g_Setting.DistanceUnit, IDS_WWE_UNIT, IDS_WWE_METRIC, IDS_WWE_STATUTE);

	//		Position Unit
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_POSITION));
	len = wcslen(temp);
	if(0 <= g_Setting.PositionUnit && 3 > g_Setting.PositionUnit) {
		swprintf(temp + len, 1024 - len, L"%s", w10_LoadString(position.options[g_Setting.PositionUnit]));
	}
	SetDlgItemText(g_hDlg, IDC_POSITION_UNIT, temp);

	//		Auto Lap
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_AUTO_LAP_TRIGGER));
	len = wcslen(temp);
	if(0 == g_Setting.AutoLap) {
		swprintf(temp + len, 1024 - len, w10_LoadString(IDS_WWE_OFF));
	} else if( 1 == g_Setting.AutoLap) {
		swprintf(temp + len, 1024 - len, L"%d(%s)",
			g_Setting.LapAtTime, w10_LoadString(autolap.options[autolap.count+g_Setting.AutoLap]));
	} else if( 2 == g_Setting.AutoLap) {
		swprintf(temp + len, 1024 - len, L"%d(%s)",
			g_Setting.LapAtDistance, w10_LoadString(autolap.options[autolap.count+g_Setting.AutoLap]));
	} else if( 3 == g_Setting.AutoLap) {
		swprintf(temp + len, 1024 - len, w10_LoadString(IDS_WWE_BY_POSITION));
	}
	SetDlgItemText(g_hDlg, IDC_AUTO_LAP, temp);
	//		Auto Lap EXT
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_LAP_AT));
	len = wcslen(temp);
	swprintf(temp + len, 1024 - len, w10_LoadString(autolapext.options[g_Setting.LapAtPosition]));
	SetDlgItemText(g_hDlg, IDC_AUTO_LAP_EXT, temp);

	//		Auto Pause
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_AUTO_TIMER_PAUSE));
	len = wcslen(temp);
	if(0 == g_Setting.AutoPause || 1 == g_Setting.AutoPause) {
		swprintf(temp + len, 1024 - len, w10_LoadString(autopause.options[g_Setting.AutoPause]));
	} else if( 2 == g_Setting.AutoPause) {
		swprintf(temp + len, 1024 - len, L"%d %s",
			g_Setting.PauseTime, w10_LoadString(autopause.options[autolap.count+g_Setting.AutoPause]));
	} else if( 3 == g_Setting.AutoPause) {
		swprintf(temp + len, 1024 - len, L"%.2f km/hr", (float)g_Setting.PauseSpeed / 100);
	}
	SetDlgItemText(g_hDlg, IDC_AUTO_PAUSE, temp);

	//		Auto Scroll
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_AUTO_SCROLL));
	len = wcslen(temp);
	if(0 <= g_Setting.AutoScroll && 3 >= g_Setting.AutoScroll) {
		swprintf(temp + len, 1024 - len, L"%s", w10_LoadString(autoscroll.options[g_Setting.AutoScroll]));
	}
	SetDlgItemText(g_hDlg, IDC_AUTO_SCROLL, temp);

	//		Virtual Partner
	fillTrueFalse(IDC_VIRTUAL_PARTNER, g_Setting.VPEnable, IDS_WWE_VIRTUAL_PARTNER_ENABLE, IDS_WWE_OFF, IDS_WWE_ON);

	//		Pace / Speed
	fillTrueFalse(IDC_PACE_SPEED, g_Setting.PaceSpeed, IDS_WWE_PACE_SPEED, IDS_WWE_PACE, IDS_WWE_SPEED);

	//		ANT+ Heart Rate Monitor
	fillTrueFalse(IDC_ANT_HEART_RATE_MONITOR, g_Setting.ANTHeartRate, IDS_WWE_ANT_HEART_RATE_MONITOR, IDS_WWE_NO, IDS_WWE_YES);

	//		ANT+ HR Display
	fillTrueFalse(IDC_ANT_HEART_RATE_DISPLAY, g_Setting.ANTHRDisplay, IDS_WWE_ANT_HR_DISPLAY, IDS_WWE_BPM, IDS_WWE_PERCENT);

	//		ANT+ Foot Pad
	fillTrueFalse(IDC_ANT_FOOT_PAD, g_Setting.ANTFootPod, IDS_WWE_ANT_FOOT_PAD, IDS_WWE_NO, IDS_WWE_YES);
	
	//		ANT+ Foot Pad Source
	fillTrueFalse(IDC_ANT_FOOT_PAD_SOURCE, g_Setting.ANTFootPadSource, IDS_WWE_ANT_FOOT_PAD_SOURCE, IDS_WWE_GPS, IDS_WWE_FOOT_PAD);

	//		ANT+ Foot Pad Step Distance
	{
		swprintf(temp, 1024, w10_LoadString(IDS_WWE_ANT_FOOT_PAD_STEP_DISTANCE));
		len = wcslen(temp);
		swprintf(temp + len, 1024 - len, L" (%s)", w10_LoadString(IDS_WWE_CM));
		len = wcslen(temp);
		swprintf(temp + len, 1024 - len, L"\r\n%s: %d\r\n", w10_LoadString(IDS_WWE_RUN), g_Setting.ANTFootPodRunDistance);
		len = wcslen(temp);
		swprintf(temp + len, 1024 - len, L"%s: %d\r\n", w10_LoadString(IDS_WWE_SWIM), g_Setting.ANTFootPodSwimDistance);
		len = wcslen(temp);
		swprintf(temp + len, 1024 - len, L"%s: %d", w10_LoadString(IDS_WWE_OTHER), g_Setting.ANTFootPodOtherDistance);
		SetDlgItemText(g_hDlg, IDC_ANT_FOOT_STEP_DISTANCE, temp);
	}

	//		ANT+ Bike Weight
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_ANT_BIKE_WEIGHT));
	len = wcslen(temp);
	swprintf(temp + len, 1024 - len, L"%d %s", 	antbikeweight.index, w10_LoadString(IDS_WWE_KG));
	SetDlgItemText(g_hDlg, IDC_ANT_BIKE_WEIGHT, temp);

	//		ANT+ Bike Odometer
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_ANT_BIKE_ODOMETER));
	len = wcslen(temp);
	swprintf(temp + len, 1024 - len, L"%d %s", 	antbikeodometer.index, w10_LoadString(IDS_WWE_METERS));
	SetDlgItemText(g_hDlg, IDC_ANT_BIKE_ODOMETER, temp);

	//		ANT+ Bike
	fillTrueFalse(IDC_ANT_BIKE, g_Setting.ANTBikeAuto, IDS_WWE_ANT_BIKE_AUTO_MANUAL, IDS_WWE_ANT_BIKE_AUTO, IDS_WWE_ANT_BIKE_MANUAL);

	//		ANT+ Bike Wheel Size
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_ANT_BIKE_WHEEL_SIZE));
	len = wcslen(temp);
	swprintf(temp + len, 1024 - len, L"%d %s", 	antbikewheel.index, w10_LoadString(IDS_WWE_CM));
	SetDlgItemText(g_hDlg, IDC_ANT_BIKE_WHEEL_SIZE, temp);

	//		ANT+ Bike Sensor
	fillTrueFalse(IDC_ANT_BIKE_SENSOR, g_Setting.ANTBikeSensor, IDS_WWE_ANT_BIKE_SENSOR, IDS_WWE_NO, IDS_WWE_YES);

	//		ANT+ Power Meter Sensor
	fillTrueFalse(IDC_ANT_POWER_METER_SENSOR, g_Setting.ANTBikePower, IDS_WWE_ANT_POWER_METER_SENSOR, IDS_WWE_NO, IDS_WWE_YES);

	//		GSensor / Compass Sensor
	fillTrueFalse(IDC_GSENSOR_COMPASS_SENSOR, g_Setting.GSensorOn, IDS_WWE_GSENSOR_COMPASS, IDS_WWE_NO, IDS_WWE_YES);

	//		GSensor Step Distance
	{
		swprintf(temp, 1024, w10_LoadString(IDS_WWE_GSENSOR_STEP_DISTANCE));
		len = wcslen(temp);
		swprintf(temp + len, 1024 - len, L" (%s)", w10_LoadString(IDS_WWE_CM));
		len = wcslen(temp);
		swprintf(temp + len, 1024 - len, L"\r\n%s: %d\r\n", w10_LoadString(IDS_WWE_RUN), g_Setting.GSensorRunDistance);
		len = wcslen(temp);
		swprintf(temp + len, 1024 - len, L"%s: %d\r\n", w10_LoadString(IDS_WWE_SWIM), g_Setting.GSensorSwimDistance);
		len = wcslen(temp);
		swprintf(temp + len, 1024 - len, L"%s: %d", w10_LoadString(IDS_WWE_OTHER), g_Setting.GSensorOtherDistance);
		SetDlgItemText(g_hDlg, IDC_GSENSOR_STEP_DISTANCE, temp);
	}

	//		BLE Finder
	fillTrueFalse(IDC_BLE_FINDER, g_Setting.BLEFinder, IDS_WWE_BLE_FINDER, IDS_WWE_NO, IDS_WWE_YES);

	//		BLE Heart Rate Function
	fillTrueFalse(IDC_BLE_HEART_RATE, g_Setting.BLEHR, IDS_WWE_BLE_HR, IDS_WWE_NO, IDS_WWE_YES);

	//		Time Zone 2
	hour = g_Setting.TimeZone2 >> 16;
	minute = g_Setting.TimeZone2 & 0xff;
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_TIME_ZONE_2));
	len = wcslen(temp);
	swprintf(temp + len, 1024 - len, L"%s%d:%02d",
		(20 > hour)? "+":"-", (20 > hour)? hour:32-hour, minute);
	SetDlgItemText(g_hDlg, IDC_TIME_ZONE_2, temp);

	//		Daylight Saving 2
	fillTrueFalse(IDC_DAYLIGHT_SAVING_2, g_Setting.TimeSaving2, IDS_WWE_TIME_SAVING_2, IDS_WWE_NO, IDS_WWE_YES);

	//		GPS Auto Update
	fillTrueFalse(IDC_GPS_TIMER_AUTO_UPDATE, g_Setting.GPSAutoUpdate, IDS_WWE_GPS_AUTO_UPDATE, IDS_WWE_NO, IDS_WWE_YES);

	//		BT SPP Tone
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_BT_SPP_TONE));
	len = wcslen(temp);
	swprintf(temp + len, 1024 - len, L"%d %s", 	btspptone.index, w10_LoadString(IDS_WWE_MS));
	SetDlgItemText(g_hDlg, IDC_BT_NOTIFY_TONE_TIME, temp);

	//		BT SPP Vibration
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_BT_SPP_VIBRATION));
	len = wcslen(temp);
	swprintf(temp + len, 1024 - len, L"%d %s", 	btsppvibration.index, w10_LoadString(IDS_WWE_MS));
	SetDlgItemText(g_hDlg, IDC_BT_NOTIFY_VIBRATION_TIME, temp);

	//		Barometer Sensor
	fillTrueFalse(IDC_BAROMETER_SENSOR, g_Setting.BarometerOn, IDS_WWE_BAROMETER_SENSOR, IDS_WWE_NO, IDS_WWE_YES);

	//		Theme
	fillTrueFalse(IDC_THEME, g_Setting.Themes, IDS_WWE_THEME, IDS_WWE_DIGITAL, IDS_WWE_INDICATOR);

	//		Metronome
	fillTrueFalse(IDC_METRONOME_STATE, g_Setting.Metronome, IDS_WWE_METRONOME, IDS_WWE_NO, IDS_WWE_YES);

	//		Metronome Interval
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_METRONOME_TIME));
	len = wcslen(temp);
	swprintf(temp + len, 1024 - len, L"%d %s", 	metronomeInterval.index, w10_LoadString(IDS_WWE_MS));
	SetDlgItemText(g_hDlg, IDC_METRONOME_INTERVAL, temp);

	//		Metronome Ring Time
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_METRONOME_RING));
	len = wcslen(temp);
	swprintf(temp + len, 1024 - len, L"%d %s", 	metronomeRing.index, w10_LoadString(IDS_WWE_MS));
	SetDlgItemText(g_hDlg, IDC_METRONOME_RING_TIME, temp);

	//		Alarm * 4
	int _hour = g_Setting.TimeZone >> 16;
	_hour = (_hour > 20)? _hour-32:_hour;
	minute = g_Setting.TimeZone & 0xff;
	const DWORD week[] = {IDS_WWE_SUN, IDS_WWE_MON, IDS_WWE_TUE, IDS_WWE_WED, IDS_WWE_THU, IDS_WWE_FRI, IDS_WWE_SAT};
	len = 0;
	for(DWORD i = 0; i < 4; i++) {
		swprintf(temp + len, 1024 - len, w10_LoadString(IDS_WWE_ALARM_ID_STATE), 1+i);
		if(1 == g_Setting.AlarmOn[i]) {
			
			len = wcslen(temp);
			swprintf(temp + len, 1024 - len, L"\r\n%d:%02d\r\n[", (24+_hour + g_Setting.AlarmHour[i])%24, (minute + g_Setting.AlarmMin[i])%60);
			for(DWORD j = 0; j < 7; j++) {
				if((1<<j) & g_Setting.AlarmWeek[i]) {
					len = wcslen(temp);
					swprintf(temp + len, 1024 - len, (0 == j)? L"%s":L", %s", w10_LoadString(week[j]));
				}
			}
			len = wcslen(temp);
			swprintf(temp + len, 1024 - len, L"]\r\n");
		} else {
			len = wcslen(temp);
			swprintf(temp + len, 1024 - len, L"\r\nDisabled\r\n");
		}
		len = wcslen(temp);
	}
	SetDlgItemText(g_hDlg, IDC_ALARM, temp);

	//------------------------------------------------------------------
	//		Time Alert
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_TIME_ALART));
	len = wcslen(temp);
	if(0 == g_Training.TimeAlert) {
		swprintf(temp + len, 1024 - len, w10_LoadString(IDS_WWE_OFF));
	} else {
		swprintf(temp + len, 1024 - len, L"%d %s", g_Training.TimeAlertTime, w10_LoadString(IDS_WWE_SECONDS));
	}
	SetDlgItemText(g_hDlg, IDC_TIME_ALERT, temp);

	//		Distance Alert
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_DISTANCE_ALART));
	len = wcslen(temp);
	if(0 == g_Training.DistanceAlert) {
		swprintf(temp + len, 1024 - len, w10_LoadString(IDS_WWE_OFF));
	} else {
		swprintf(temp + len, 1024 - len, L"%d %s", g_Training.DistanceAlertValue, w10_LoadString(IDS_WWE_METERS));
	}
	SetDlgItemText(g_hDlg, IDC_DIST_ALERT, temp);

	//		Calorie Alert
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_CALORIE_ALART));
	len = wcslen(temp);
	if(0 == g_Training.ColorieAlert) {
		swprintf(temp + len, 1024 - len, w10_LoadString(IDS_WWE_OFF));
	} else {
		swprintf(temp + len, 1024 - len, L"%d %s", g_Training.ColorieAlertValue, w10_LoadString(IDS_WWE_CAL));
	}
	SetDlgItemText(g_hDlg, IDC_CALORIE_ALERT, temp);

	//		Heart Rate Alert
	/*swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_HR_ALART));
	len = wcslen(temp);
	if(0 == g_Training.HRAlert) {
		swprintf(temp + len, 1024 - len, w10_LoadString(IDS_WWE_OFF));
	} else {
		swprintf(temp + len, 1024 - len, L"%d %s", g_Training.HRAlertValue, w10_LoadString(IDS_WWE_BPM));
	}
	SetDlgItemText(g_hDlg, IDC_HR_ALERT, temp);*/
	//		Heart Rate Alert Min
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_HR_ALART_MIN));
	len = wcslen(temp);
	if(0 == g_Training.HRMinAlert) {
		swprintf(temp + len, 1024 - len, w10_LoadString(IDS_WWE_OFF));
	} else {
		swprintf(temp + len, 1024 - len, L"%d %s", g_Training.HRMinAlertValue, w10_LoadString(IDS_WWE_BPM));
	}
	SetDlgItemText(g_hDlg, IDC_HR_ALERT_MIN, temp);
	//		Heart Rate Alert Max
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_HR_ALART_MAX));
	len = wcslen(temp);
	if(0 == g_Training.HRMaxAlert) {
		swprintf(temp + len, 1024 - len, w10_LoadString(IDS_WWE_OFF));
	} else {
		swprintf(temp + len, 1024 - len, L"%d %s", g_Training.HRMaxAlertValue, w10_LoadString(IDS_WWE_BPM));
	}
	SetDlgItemText(g_hDlg, IDC_HR_ALERT_MAX, temp);

	//		Interval
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_INTERVAL_TYPE));
	len = wcslen(temp);
	if(0 == g_Training.IntervalType) {
		swprintf(temp + len, 1024 - len, L"%d %s", g_Training.IntervalTime, w10_LoadString(IDS_WWE_SECONDS));
	} else {
		swprintf(temp + len, 1024 - len, L"%d %s", g_Training.IntervalDistance, w10_LoadString(IDS_WWE_METERS));
	}
	SetDlgItemText(g_hDlg, IDC_INTERVAL, temp);

	//		Rest
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_REST_TYPE));
	len = wcslen(temp);
	if(0 == g_Training.RestType) {
		swprintf(temp + len, 1024 - len, L"%d %s", g_Training.RestTime, w10_LoadString(IDS_WWE_SECONDS));
	} else {
		swprintf(temp + len, 1024 - len, L"%d %s", g_Training.RestDistance, w10_LoadString(IDS_WWE_METERS));
	}
	SetDlgItemText(g_hDlg, IDC_REST, temp);

	//		Reps
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_REPEATS));
	len = wcslen(temp);
	swprintf(temp + len, 1024 - len, L"%d", g_Training.Reps);
	SetDlgItemText(g_hDlg, IDC_REPTS, temp);

	//		Warm Up
	fillTrueFalse(IDC_WARM_UP, g_Training.WarmUp, IDS_WWE_WARM_UP, IDS_WWE_DISABLE, IDS_WWE_ENABLE);

	//		Cool Down
	fillTrueFalse(IDC_COOL_DOWN, g_Training.CoolDown, IDS_WWE_COOL_DOWN, IDS_WWE_DISABLE, IDS_WWE_ENABLE);

	//		Gender
	fillTrueFalse(IDC_GENDER, g_Training.Gender, IDS_WWE_GENDER, IDS_WWE_MALE, IDS_WWE_FEMALE);

	//		Age
	swprintf(temp, 1024, L"%s ", w10_LoadString(IDS_WWE_AGE));
	len = wcslen(temp);
	swprintf(temp + len, 1024 - len, L"%d", g_Training.Age);
	SetDlgItemText(g_hDlg, IDC_AGE, temp);

	//		Weight & Height
	swprintf(temp, 1024, L"%s\r\n", w10_LoadString(IDS_WWE_WEIGHT));
	len = wcslen(temp);
	swprintf(temp + len, 1024 - len, L"%d %s / ", g_Training.Weight, w10_LoadString(IDS_WWE_KG));
	len = wcslen(temp);
	swprintf(temp + len, 1024 - len, L"%d %s\r\n", (DWORD)ceil(2.2046 * g_Training.Weight - 0.4), w10_LoadString(IDS_WWE_POUND));
	len = wcslen(temp);
	swprintf(temp + len, 1024 - len, L"%s\r\n", w10_LoadString(IDS_WWE_HEIGHT));
	len = wcslen(temp);
	swprintf(temp + len, 1024 - len, L"%d %s / ", g_Training.Height, w10_LoadString(IDS_WWE_CM));
	len = wcslen(temp);
	swprintf(temp + len, 1024 - len, L"%d %s\r\n", (DWORD)ceil(0.3937 * g_Training.Height - 0.4), w10_LoadString(IDS_WWE_INCH));
	SetDlgItemText(g_hDlg, IDC_WEIGHT_HEIGHT, temp);

	//		Lifetime Athlete
	fillTrueFalse(IDC_LIFETIME_ATHLETE, g_Training.LFAthlete, IDS_WWE_LIFETIME_ATHLETE, IDS_WWE_NO, IDS_WWE_YES);

}

TCHAR* w10_LoadString(DWORD id) {
	static TCHAR str[512+1];
	memset(str ,0, sizeof(str));
	LoadString(g_hInst, id, str, 512);
	return str;
}

void fillTrueFalse(DWORD id, DWORD value, DWORD title, DWORD no, DWORD yes) {
	TCHAR temp[512];
	swprintf(temp, 512, L"%s", w10_LoadString(title));
	DWORD len = wcslen(temp);
	if(0 == value) {
		swprintf(temp + len, 512 - len, L" %s", w10_LoadString(no));
	} else if(1 == value) {
		swprintf(temp + len, 512 - len, L" %s", w10_LoadString(yes));
	}
	SetDlgItemText(g_hDlg, id, temp);
}

void reverseBool(unsigned int & v) {
	v = (1+v)%2;
}



/****************************************************************************************************
*
*		SPP for Settings
*/
//GLOBAL_SETTING g_Setting;
//GLOBAL_TRAINING g_Training;
#include "stdio.h"
#define		MAXERR		200
#define		MAXWRITE	400
#define		dim(x)		sizeof(x)/sizeof(x[0])
const char read_cmd[] = "SPP+FREAD=%s\r\0";
const char read_setting[] = "SPP+FREAD=g_Setting\r\0";
const char read_training[] = "SPP+FREAD=g_Training\r\0";
const char read_ack[] = "FREAD,";
const char dev_reset[] = "SPP+RESET=\r\0";
const char write_cmd[] = "SPP+FWRITE=%s,%d,";
const char append_cmd[] = "SPP+FAPPEND=%s,%d,";


inline BOOL checkErrMax(DWORD err) {
	return MAXERR > err;
}

BOOL getData(char* file_name, char* buffer, DWORD bufSize) {
	BOOL result = TRUE;
	{
		char temp[512+1] = {0};
		char cmd[128+1] = {0};
		//		SPP+FREAD command
		sprintf_s(cmd, 128, read_cmd, file_name);
		p->WriteCom((char*)cmd, strlen(cmd) + 1);
		DWORD err = 0;
		char* ptr = NULL;
		DWORD read = 0;
		//		check tag FREAD,
		while( checkErrMax(err) && NULL == ptr ) {
			//Sleep(100);	DO NOT use this, set read comm timeout instead
			read = p->ReadCom(temp, 512);
			ptr = strstr(temp, read_ack);
			err++;
		}
		if( ! checkErrMax(err)) {
			return FALSE;
		}
		//		get Size
		DWORD len = 0;
		DWORD size = 0;
		ptr += strlen(read_ack);
		size = strtol(ptr, NULL, 10);
		if(size != bufSize) {
			return FALSE;
		}
		//		start reading file
		ptr = strstr(ptr, ",");
		if(NULL == ptr) {
			return FALSE;
		}
		ptr++;
		if(ptr > temp && (read-2) > (DWORD)(ptr-temp) ) {
			len = read - 2 - (ptr - temp);
			memcpy(buffer, ptr, len);
			swprintf(msg, 1024, L"len == %d", len);
			dmsg(L"getData()", msg);
		} else {
			len = 0;
		}
		//		get the rest
		err = 0;
		while( checkErrMax(err) && len < bufSize ) {
			read = p->ReadCom(temp, 512);
			if(0 == read) {
				err++;
				continue;
			}
			if( bufSize > (len + read) ) {
				memcpy(buffer+len, temp, read);
				len += read;
			} else {
				read = bufSize - len;
				memcpy(buffer+len, temp, read);
				len += read;
			}
			swprintf(msg, 1024, L"%d/%d", len, size);
			dmsg(msg);
		}
		result = len == bufSize;
	}
	return result;
}

BOOL getSetting(void) {
	return getData("g_Setting", (char*)&g_Setting, sizeof(g_Setting));
}


BOOL getTraining(void) {
	return getData("g_Training", (char*)&g_Training, sizeof(g_Training));
}

/*const char write_cmd[] = "SPP+FWRITE=%s,%d,";
const char append_cmd[] = "SPP+FAPPEND=%s,%d,";*/
BOOL setData(char* file_name, char* buffer, DWORD bufSize) {
	BOOL result = TRUE;
	DWORD w = 0;
	DWORD size = 0;
	DWORD preLen = 0;
	DWORD ww = 0;
	char buf[512];
	do
	{
		if(0 == size) {

			w = (400 > bufSize)? bufSize:400;
			sprintf_s(buf, 512, write_cmd, file_name, w);
			preLen = strlen(buf);
			memcpy(preLen + buf, buffer+size, w);
			memcpy(preLen + w + buf, "\r\0", 2);

			do
			{
				ww += p->WriteCom(buf, preLen + w + 2);
			} while( ww < (preLen + w + 2) );
			size += w;

		} else if(bufSize > size) {
			
			w = (bufSize - size > 400)? 400:(bufSize - size);
			sprintf_s(buf, 512, append_cmd, file_name, w);
			preLen = strlen(buf);
			memcpy(buf+preLen, buffer+size, w);
			memcpy(preLen + w + buf, "\r\0", 2);
			
			do
			{
				ww += p->WriteCom(buf, preLen + w + w);
			} while(ww < preLen + w + 2);
			size += w;
		}
	} while(size < bufSize);
	return result;
}

BOOL setSetting(void) {
	return setData("g_Setting", (char*)&g_Setting, sizeof(g_Setting));
}

BOOL setTraining(void) {
	return setData("g_Training", (char*)&g_Training, sizeof(g_Training));
}

