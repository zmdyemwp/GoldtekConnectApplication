#ifndef __STORAGE_H
#define __STORAGE_H


#define STORAGE_ID_VALUE				0x47544948

#define STORAGE_STATE_GPS				(1<<0)
#define STORAGE_STATE_GSENSOR			(1<<1)
#define STORAGE_STATE_ANT_HR			(1<<8)
#define STORAGE_STATE_ANT_FOOTPOD		(1<<9)
#define STORAGE_STATE_ANT_BIKE			(1<<10)
#define STORAGE_STATE_BLE_FINDER		(1<<16)
#define STORAGE_STATE_BTSPP				(1<<23)
#define STORAGE_STATE_BAROMETER			(1<<24)

#define STORAGE_AVAILABLE_DISTANCE		(1<<0)
#define STORAGE_AVAILABLE_CALORIES		(1<<1)
#define STORAGE_AVAILABLE_STEP			(1<<4)
#define STORAGE_AVAILABLE_CADENCE		(1<<5)
#define STORAGE_AVAILABLE_HEARTRATE		(1<<7)
#define STORAGE_AVAILABLE_PACE			(1<<8)
#define STORAGE_AVAILABLE_SPEED			(1<<9)
#define STORAGE_AVAILABLE_HIGH			(1<<10)
#define STORAGE_AVAILABLE_BAROMETER		(1<<12)
#define STORAGE_AVAILABLE_TEMPERATURE	(1<<13)
#define STORAGE_AVAILABLE_ANGLE			(1<<15)
#define STORAGE_AVAILABLE_GPS			(1<<16)	//Include Satelcnt, Lat, Long, Accuracy, Altitude

#ifndef FILE_FORMAT_OLD
// Total 64 Bytes
typedef struct {
	unsigned int Id;				// File's Id. Content is 0x47544957
	unsigned short Year;			// RTC's Year
	unsigned char Month;			// RTC's Month
	unsigned char Day;				// RTC's Day
	unsigned char Hour;				// RTC's Hour (24 Hour)
	unsigned char Min;				// RTC's Minute
	unsigned char Sec;				// RTC's Second
	unsigned char Mode;				// Mode: 1:Run, 2:Bike, 3:Swim, 4:Other
	unsigned short Weight;			// Weight: Number (kg)
	unsigned short Height;			// Height: Number (cm)
	unsigned short Gender;			// Gender: 0:Male, 1:Female
	unsigned short Age;				// Age: Number
	unsigned int Sunrise;			// Sunrise: Number (Sec)
	unsigned int Sunset;			// Sunset: Number (Sec)
	unsigned int Reserve[9];		// Reserve 6 Word
}STORAGE_HEAD, *P_STORAGE_HEAD;

#else
// Total 64 Bytes
typedef struct {
	unsigned int Id;				// File's Id. Content is 0x47544957
	unsigned short Year;			// RTC's Year
	unsigned short Month;			// RTC's Month
	unsigned short Day;				// RTC's Day
	unsigned short Hour;			// RTC's Hour (24 Hour)
	unsigned short Min;				// RTC's Minute
	unsigned short Sec;				// RTC's Second
	unsigned short Mode;			// Mode
	unsigned short LapCount;		// Total Lap Number (Number)
	unsigned int Distance;			// Total Distance (Meter)
	unsigned int TotalTick;			// Total Tick (mSec)
	unsigned int Speed;				// Average Speed (Meter/hour)
	unsigned int Weight;			// Weight: Number (kg)
	unsigned int Height;			// Height: Number (cm)
	unsigned int Reserve[6];		// Reserve 6 Word
}STORAGE_HEAD, *P_STORAGE_HEAD;
#endif

typedef struct {
	unsigned int Tick;		// Run Tick (mSec)
	unsigned int SensorState;
	unsigned int Available;	
	unsigned short Lap;		// 
	unsigned short Accuracy;	//
	unsigned char HeartRate;	// bpm
	unsigned char Fix;		// A or V
	unsigned char Battery;	// 0~100%
	unsigned char Satelcnt;	// Satel Count
	unsigned int Step;		// Count the Step
	unsigned int Cadence;	// Count per Minute
	unsigned int Barometer;	// 1/100
	unsigned int Temperature;// 1/100 Degree
	unsigned int Pace;		// Sec per Km
	int Altitude;			// 1/10 meter
	int High;				// 1/10 meter
	double Lat;				// Latitude 
	double Long;			// Longitude
	double Speed;			// Speed: (Km per Hour)
	double Calories;		// kCol
	double Distance;		// meter
	double Angle;			// Compass Angle
}STORAGE_DATA, *P_STORAGE_DATA;


#define STORAGE_GPS_VALUE				0x10

#define STORAGE_ANT_HEARTBEAT_VALUE		0x20
#define STORAGE_ANT_PEDOMETER_VALUE		0x21
#define STORAGE_ANT_BIKE_VALUE			0x22

#define STORAGE_BAROMETER_VALUE			0x30	//Barometer
#define STORAGE_TEMPERATURE_VALUE		0x31	//Temperature
#define STORAGE_COMPASS_VALUE			0x35	//Compass Info
#define STORAGE_GSENSOR_VALUE			0x36	//GSensor

#define STORAGE_LAP_VALUE				0x82	//LAP


// Total 28 Bytes
// STORAGE_GPS	0x10
typedef struct {
	unsigned int Tick;
	double Lat;				// Latitude 
	double Long;			// Longitude
	double Speed;
}STORAGE_GPS, *P_STORAGE_GPS;

// Total 8 Bytes
// STORAGE_ANT_HEARTBEAT 0x20
typedef struct {
	unsigned int Tick;
	unsigned int Data;
}STORAGE_HEARTBEAT, *P_STORAGE_HEARTBEAT;

// Total 8 Bytes
// STORAGE_ANT_PEDOMETER 0x21
typedef struct {
	unsigned int Tick;
	unsigned int Data;
}STORAGE_PEDOMETER, *P_STORAGE_PEDOMETER;

// Total 8 Bytes
// STORAGE_ANT_BIKE 0x22
typedef struct {
	unsigned int Tick;
	unsigned int Data;
}STORAGE_BIKE, *P_STORAGE_BIKE;

// Total 8 Bytes
// STORAGE_BAROMETER 0x30
typedef struct {
	unsigned int Tick;
	unsigned int Data;
}STORAGE_BAROMETER, *P_STORAGE_BAROMETER;

// Total 8 Bytes
// STORAGE_TEMPERATURE 0x31
typedef struct {
	unsigned int Tick;
	unsigned int Data;
}STORAGE_TEMPERATURE, *P_STORAGE_TEMPERATURE;


// Total 20 Bytes
// STORAGE_LAP 0x82
typedef struct {
	unsigned int Tick;
	double Speed;				// Lap Average Speed: Number (m/h)
	double Distance;			// Lap Distance: Number (Meter)
}STORAGE_LAP, *P_STORAGE_LAP;


typedef struct {
	unsigned int P1;		// Lat Pos, N=0, S=1
	unsigned int D1;		// Lat Degree
	unsigned int M1;		// Lat Minute
	unsigned int S1;		// Lat Second
	unsigned int cS1;		// Lat 1/100 Second
	unsigned int P2;		// Long Degree, E=0, W=1
	unsigned int D2;		// Long Degree
	unsigned int M2;		// Long Minute
	unsigned int S2;		// Long Second
	unsigned int cS2;		// Long 1/100 Second
	unsigned int High;		// High		//cm
}STORAGE_GPS_LOCATION, *P_STORAGE_GPS_LOCATION;


typedef struct {
	unsigned int Distance;		// Virtual Race Distance
	unsigned int Time;			// Virtual Race Time(Sec)
}STORAGE_VR_LOCATION, *P_STORAGE_VR_LOCATION;




#define STORAGE_WORKOUT_STATE_OFF			0
#define STORAGE_WORKOUT_STATE_RUN			1
#define STORAGE_WORKOUT_STATE_REST			2
#define STORAGE_WORKOUT_STATE_WARMUP		8
#define STORAGE_WORKOUT_STATE_COOLDOWN		9

#define STORAGE_WORKOUT_TYPE_LAPPRESS		0
#define STORAGE_WORKOUT_TYPE_TIME			1
#define STORAGE_WORKOUT_TYPE_DISTANCE		2
#define STORAGE_WORKOUT_TYPE_HR_GT			3	// >
#define STORAGE_WORKOUT_TYPE_HR_ST			4	// <
#define STORAGE_WORKOUT_TYPE_CALORIC		5

typedef struct {
	unsigned int State;			// Workout State;
	unsigned int Type;			// Workout Type
	unsigned int Info;			// Workout Info
}STORAGE_WORKOUT, *P_STORAGE_WORKOUT;




#endif
