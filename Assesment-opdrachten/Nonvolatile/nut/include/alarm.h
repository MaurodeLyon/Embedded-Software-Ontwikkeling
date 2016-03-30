/* ========================================================================
* [PROJECT]    SIR100
* [MODULE]     Lcd
* [TITLE]      lcd header file
* [FILE]       lcd.h
* [VSN]        1.0
* [CREATED]    18022016
* [LASTCHNGD]  18022016
* [COPYRIGHT]  Copyright (C) Avans BV 2010
* [PURPOSE]    API and gobal defines for display module
* ======================================================================== */
#ifndef _Alarm_H
#define _Alarm_H

#include <time.h>

/*-------------------------------------------------------------------------*/
/* global defines                                                          */
/*-------------------------------------------------------------------------*/
/*#define DISPLAY_SIZE                16
*#define NROF_LINES                  2
*#define MAX_SCREEN_CHARS            (NROF_LINES*DISPLAY_SIZE)
*
*#define LINE_0                      0
*#define LINE_1                      1
*/

/*-------------------------------------------------------------------------*/
/* typedefs & structs                                                      */
/*-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*  Global variables                                                        */
/*--------------------------------------------------------------------------*/
struct Alarm alarm1;
struct Alarm alarm2;
/*-------------------------------------------------------------------------*/
/* export global routines (interface)                                      */
/*-------------------------------------------------------------------------*/
/*extern void LcdChar(char);
*extern void LcdBackLight(u_char);
*extern void LcdInit(void);
*extern void LcdLowLevelInit(void);
*/


struct Alarm
{
	/*in this struct the date and time for a alarm wil be saved 
	the status is to tell if its active 0 = off 1 = on 
	mode is to determined if it's on beep or stream mode 0 = beep 1= stream
	volume is for the volume of this alarm with a value between 0 and 255*/
	tm dateTime;
	enum STATUS { off, on } status;
	enum MODE { beep, stream } mode;
	char* stream;
	int volume;
	int isSet;
};

extern void setVariableAlarm(void);
extern struct _tm getVariableAlarm(void);
extern void hardcodedAlarm(void);
extern void getHardcodedAlarm(void);
extern int CheckAlarm1(void);
extern int CheckAlarm2(void);
extern void initAlarms(void);
extern void setAlarm(struct Alarm *alarm, tm dt, enum STATUS st, enum MODE md,char* str, int vol);
extern void turnOffAlarm(void);
extern void playAlarm(struct Alarm a);
extern void snoozeAlarm(int snoozeTime, int alarm1or2);
extern void setAlarmVolume(struct Alarm a, int volume);
extern void setStream(char *stream);


#endif /* _Lcd_H */
/*  ����  End Of File  �������� �������������������������������������������� */





