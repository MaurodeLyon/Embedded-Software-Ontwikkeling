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

#ifndef _Lcd_H
#define _Lcd_H

void initLCD(void);
void printString(char* str);
void setCursorPos(int y, int x);
void clearLine(int line);
void clearScreen(void);
void setBackLight(int state);
void setSreenOnPeriod(int periodInMS);
void setAmountOfAlarm(int mode);
void printCustomCharacter(int character);

#endif

