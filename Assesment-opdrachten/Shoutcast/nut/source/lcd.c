/* ========================================================================
* [PROJECT]    SIR100
* [MODULE]     LCD
* [TITLE]      lcd source file
* [FILE]       lcd.c
* [VSN]        1.0
* [CREATED]    18022016
* [LASTCHNGD]  18022016
* [COPYRIGHT]  Copyright (C) Avans BV
* [PURPOSE]    contains all interface- and low-level routines to
*              control the LCD and write characters or strings (menu-items)
* ======================================================================== */

#define LOG_MODULE  LOG_DISPLAY_MODULE

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/timer.h>
#include <sys/event.h>
#include <sys/thread.h>
#include <sys/heap.h>

#include "system.h"
#include "portio.h"
#include "display.h"
#include "log.h"
#include "lcd.h"
#include "led.h"

#define customCharacterAmount 4

int lcdBackLightStatus = 0;
int alarmStatus = 0;
int onTimer = 0;
int backlightTimer = 0;
void createCustomChar(int);
void updateScreenBackLight(void);


static unsigned char charset[customCharacterAmount][8] = //let op: maximaal 8 karakters
{
	{ 0x02, 0x06, 0x0e, 0x1e, 0x1e, 0x0e, 0x06, 0x02 },// char 0: pijl links
	{ 0x08, 0x0c, 0x0e, 0x0f, 0x0f, 0x0e, 0x0c, 0x08 }, // char 1: pijl rechts
	{ 0x1F, 0x1F, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f },
	{ 0x00, 0x02, 0x1F, 0x02, 0x08, 0x1F, 0x08, 0x00 }// vol block
													   // char 3 enz t/m char 7
};

/*-------------------------------------------------------------------------*/
/* local defines                                                           */
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/* local variable definitions                                              */
/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/* local routines (prototyping)                                            */
/*-------------------------------------------------------------------------*/

/*!
* \addtogroup Lcd
*/

/*@{*/

/*-------------------------------------------------------------------------*/
/*                         start of code                                   */
/*-------------------------------------------------------------------------*/



/*
* Missing following functions:
*  - Shifting strings bigger then 16 chars
*  - Set cursor on 1-32 place
*/
void initLCD()
{
	LcdLowLevelInit();
	createCustomChar(customCharacterAmount);
}

void printString(char* str)
{
	int i, j;
	int rest;

	if (strlen(str) <= 16)
	{
		for (i = 0; i < strlen(str); i++)
		{
			LcdChar(str[i]);
		}
	}
	else
	{
		rest = strlen(str) - 16;

		for (j = 0; j <= rest; j++)
		{
			clearScreen();
			for (i = 0 + j; i<strlen(str); i++)
			{
				LcdChar(str[i]);
			}
			NutDelay(250);
		}
		NutSleep(1000);
	}

}

void clearLine(int line)
{
	setCursorPos(line, 0);
	printString("                   ");
	setCursorPos(line, 0);
}


void setBackLight(int state)
{
	switch (state)
	{
	case 0:
		LcdBackLight(LCD_BACKLIGHT_OFF);
		lcdBackLightStatus = state;
		break;
	case 1:
		LcdBackLight(LCD_BACKLIGHT_ON);
		lcdBackLightStatus = state;
		break;
	}
}

void setCursorPos(int YPos, int XPos)
{
	// 0 is the first line. 1 the second line
	switch (YPos)
	{
	case 0:	LcdWriteByte(WRITE_COMMAND, 0x80 + XPos);
		break;
	case 1:	LcdWriteByte(WRITE_COMMAND, 0xC0 + XPos);
		break;
	}


}

void clearScreen()
{
	LcdWriteByte(WRITE_COMMAND, 0x01);
	NutDelay(5);
}

void setScreenOnPeriod(int period)
{
	onTimer = period;
}

void updateScreenBackLight()
{
	if (alarmStatus == 2)
	{
		LedControl(LED_TOGGLE);
	}
	if (lcdBackLightStatus > 0)
	{
		backlightTimer++;
		if (backlightTimer >= onTimer)
		{
			backlightTimer = 0;
			lcdBackLightStatus = 0;
			LcdBackLight(LCD_BACKLIGHT_OFF);
		}
	}

}

void setAmountOfAlarm(int mode)
{
	alarmStatus = mode;
	switch (mode)
	{
	case 0:
		LedControl(LED_OFF);
		break;
	case 1:
		LedControl(LED_ON);
		break;
	case 2:
		break;
	default:
		break;
	}
	LedControl(LED_TOGGLE);
}

void createCustomChar(int characters) {
	int ch = 0;
	int k = 0;
	LcdWriteByte(WRITE_COMMAND, 64 );
	for (k = 0; k < characters; k++)
	{
		
		for (ch = 0; ch < 8; ch++)
		{
			LcdWriteByte(WRITE_DATA, charset[k][ch]|(ch << 5)); // schrijf data in CG-RAM | 
		}
	}

	LcdWriteByte(WRITE_COMMAND, 0x80);
}
void printCustomCharacter(int character)
{
	LcdChar(character);
}
