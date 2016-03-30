#include <time.h>
#include <stdio.h>

#include "rtc.h"
#include "alarm.h"
#include "EEPROMHandler.h"
#include "rtc.h"
#include "vs10xx.h"
#include "flash.h"

int threeTimes;
void setHardcodedAlarm(void);
void getHardcodedAlarm(void);
/*
*
*Use initAlarms after EEPROMHandler readEEPROM() function.
*/

void initAlarms()
{
	threeTimes = 0;
	setHardcodedAlarm();
	getHardcodedAlarm();
}

//Flag long
int	 flag1 = 143;
//Integer for alarm checker

void setHardcodedAlarm()
{
	printf("------------------------------------------------\n");
	printf("\nAlarm 1 is dagelijks alarm: \n");
	tm alarmTime;
	alarmTime.tm_sec = 00;
	alarmTime.tm_min = 10;
	alarmTime.tm_hour = 13;
	alarmTime.tm_wday = 4;
	alarmTime.tm_mday = 9;
	//alarmTime.tm_mon = 17;
	//alarmTime.tm_year = 116;
	printf("Alarm time is: %02d:%02d:%02d\n", alarmTime.tm_hour, alarmTime.tm_min, alarmTime.tm_sec);
	//printf("Alarm date is: %02d.%02d.%04d\n", alarmTime.tm_mday, (alarmTime.tm_mon + 1), (alarmTime.tm_year + 1900));

	//Fill in alarm struct and set alarm
	setAlarm(&IMCconfig.alarm1, alarmTime, on, stream, "OK", 50);
	X12RtcSetAlarm(0, &IMCconfig.alarm1.dateTime, flag1);
}

void getHardcodedAlarm()
{
	//Recieve RTC time and display
	tm gmt;
	X12RtcGetAlarm(0, &gmt, &flag1);
	printf("\nSet Alarm: Date: %d-%d-2016 Time: %d:%d:%d \n", gmt.tm_mday, (gmt.tm_mon + 1), gmt.tm_hour, gmt.tm_min, gmt.tm_sec);
	printf("------------------------------------------------\n");
}


void setAlarm(struct Alarm *alarm,tm dt, enum STATUS st, enum MODE md,char* str, int vol)
{
	alarm->dateTime = dt;
	alarm->status = st;
	alarm->mode = md;
	alarm->stream = str;
	alarm->volume = vol;
	alarm->isSet=1;
	saveEEPROM();
}

void setVariableAlarm()
{
	X12RtcSetAlarm(1, &IMCconfig.alarm2.dateTime, flag1);
}

struct _tm getVariableAlarm()
{
	tm alarmTime;
	X12RtcGetAlarm(1, &alarmTime, &flag1);
	return alarmTime;
}

int CheckAlarm1()
{
	tm alarmTime;
	X12RtcGetAlarm(0, &alarmTime, &flag1);
	tm currentTime;
	X12RtcGetClock(&currentTime);
	int alarm1Seconds = 0;
	alarm1Seconds = (alarmTime.tm_hour * 3600) + (alarmTime.tm_min * 60) + (alarmTime.tm_sec);
	int currentSeconds = 0;
	currentSeconds = (currentTime.tm_hour * 3600) + (currentTime.tm_min * 60) + (currentTime.tm_sec);
	if(currentSeconds >= (alarm1Seconds - 30) && currentSeconds <= (alarm1Seconds + 30))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int CheckAlarm2()
{
	tm alarmTime;
	X12RtcGetAlarm(1, &alarmTime, &flag1);
	tm currentTime;
	X12RtcGetClock(&currentTime);
	int alarm1Seconds = 0;
	alarm1Seconds = (alarmTime.tm_hour * 3600) + (alarmTime.tm_min * 60) + (alarmTime.tm_sec);
	int currentSeconds = 0;
	currentSeconds = (currentTime.tm_hour * 3600) + (currentTime.tm_min * 60) + (currentTime.tm_sec);
	printf("Alarm1 : %d \t %d:%d:%d\n",alarm1Seconds,alarmTime.tm_hour,alarmTime.tm_min ,alarmTime.tm_sec);
	printf("Current: %d \t %d:%d:%d\n",currentSeconds,currentTime.tm_hour,currentTime.tm_min,currentTime.tm_sec);

	if(currentSeconds >= (alarm1Seconds - 30) && currentSeconds <= (alarm1Seconds + 30))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void turnOffAlarm()
{
	threeTimes = 0;

}
void snoozeAlarm(int snoozeTime, int alarm1or2)
{
	if (threeTimes < 3)
	{
		tm alarm1TimeSetting;
		int minSet;
		int hourSet;
		if(alarm1or2 ==1)
		{
			X12RtcGetClock(&alarm1TimeSetting);

			if (alarm1TimeSetting.tm_min + snoozeTime < 60)
			{
				minSet = alarm1TimeSetting.tm_min + snoozeTime;
				hourSet = alarm1TimeSetting.tm_hour;
			}
			else
			{
				minSet = (alarm1TimeSetting.tm_min + snoozeTime) - 60;
				hourSet = alarm1TimeSetting.tm_hour + 1;
			}
			tm alarmTime1 = { alarm1TimeSetting.tm_sec, minSet, hourSet, alarm1TimeSetting.tm_mday,alarm1TimeSetting.tm_mon,alarm1TimeSetting.tm_year ,alarm1TimeSetting.tm_wday,alarm1TimeSetting.tm_yday,alarm1TimeSetting.tm_isdst };
			X12RtcSetAlarm(0, &alarmTime1, flag1);
			printf("alarm 1 snoozed \n");
		}
		else
		{
			X12RtcGetClock(&alarm1TimeSetting);

			if (alarm1TimeSetting.tm_min + snoozeTime < 60)
			{
				minSet = alarm1TimeSetting.tm_min + snoozeTime;
				hourSet = alarm1TimeSetting.tm_hour;
			}
			else
			{
				minSet = (alarm1TimeSetting.tm_min + snoozeTime) - 60;
				hourSet = alarm1TimeSetting.tm_hour + 1;
			}
			tm alarmTime1 = { alarm1TimeSetting.tm_sec, minSet, hourSet, alarm1TimeSetting.tm_mday,alarm1TimeSetting.tm_mon,alarm1TimeSetting.tm_year ,alarm1TimeSetting.tm_wday,alarm1TimeSetting.tm_yday,alarm1TimeSetting.tm_isdst };
			X12RtcSetAlarm(1, &alarmTime1, flag1);
			printf("alarm 1 snoozed \n");
		}
		threeTimes++;
	}
}
void setVolume(struct Alarm *a, int vol)
{
	a->volume = vol;
}

void setStream(char *stream)
{

}
