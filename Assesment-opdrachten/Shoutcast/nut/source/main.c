#define LOG_MODULE  LOG_MAIN_MODULE

#include <stdio.h>
#include <string.h>

#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/version.h>
#include <dev/irqreg.h>

#include "system.h"
#include "portio.h"
#include "display.h"
#include "remcon.h"
#include "keyboard.h"
#include "led.h"
#include "log.h"
#include "uart0driver.h"
#include "mmc.h"
#include "watchdog.h"
#include "flash.h"
#include "spidrv.h"

#include <time.h>
#include "rtc.h" 

static void SysMainBeatInterrupt(void*);
static void SysControlMainBeat(u_char);

static void SysMainBeatInterrupt(void *p)
{
	KbScan();
	CardCheckCard();
}

void SysInitIO(void)
{
	/*
	*  Port B:     VS1011, MMC CS/WP, SPI
	*  output:     all, except b3 (SPI Master In)
	*  input:      SPI Master In
	*  pull-up:    none
	*/	
	outp(0xF7, DDRB);

	/*
	*  Port C:     Address bus
	*/

	/*
	*  Port D:     LCD_data, Keypad Col 2 & Col 3, SDA & SCL (TWI)
	*  output:     Keyboard colums 2 & 3
	*  input:      LCD_data, SDA, SCL (TWI)
	*  pull-up:    LCD_data, SDA & SCL
	*/
	outp(0x0C, DDRD);
	outp((inp(PORTD) & 0x0C) | 0xF3, PORTD);

	/*
	*  Port E:     CS Flash, VS1011 (DREQ), RTL8019, LCD BL/Enable, IR, USB Rx/Tx
	*  output:     CS Flash, LCD BL/Enable, USB Tx
	*  input:      VS1011 (DREQ), RTL8019, IR
	*  pull-up:    USB Rx
	*/
	outp(0x8E, DDRE);
	outp((inp(PORTE) & 0x8E) | 0x01, PORTE);

	/*
	*  Port F:     Keyboard_Rows, JTAG-connector, LED, LCD RS/RW, MCC-detect
	*  output:     LCD RS/RW, LED
	*  input:      Keyboard_Rows, MCC-detect
	*  pull-up:    Keyboard_Rows, MCC-detect
	*  note:       Key row 0 & 1 are shared with JTAG TCK/TMS. Cannot be used concurrent
	*/
	#ifndef USE_JTAG
	sbi(JTAG_REG, JTD); // disable JTAG interface to be able to use all key-rows
	sbi(JTAG_REG, JTD); // do it 2 times - according to requirements ATMEGA128 datasheet: see page 256
	#endif 				//USE_JTAG

	outp(0x0E, DDRF);
	outp((inp(PORTF) & 0x0E) | 0xF1, PORTF);

	/*
	*  Port G:     Keyboard_cols, Bus_control
	*  output:     Keyboard_cols
	*  input:      Bus Control (internal control)
	*  pull-up:    none
	*/
	outp(0x18, DDRG);
}

static void SysControlMainBeat(u_char OnOff)
{
	int nError = 0;
	if (OnOff==ON)
	{
		nError = NutRegisterIrqHandler(&OVERFLOW_SIGNAL, SysMainBeatInterrupt, NULL);
		if (nError == 0)
		{
			init_8_bit_timer();
		}
	}
	else
	{
		// disable overflow interrupt
		disable_8_bit_timer_ovfl_int();
	}
}

THREAD(T1, arg)
{
	for (;;) {
		NutSleep(1000);
		LogMsg_P(LOG_INFO, PSTR("T1"));
	}
}

int main(void)
{
	tm gmt;
	WatchDogDisable();
	NutDelay(100);
	SysInitIO();
	SPIinit();
	LedInit();
	LcdLowLevelInit();
	Uart0DriverInit();
	Uart0DriverStart();
	LogInit();
	LogMsg_P(LOG_INFO, PSTR("-----------------------------------------------------------------------------------------------"));
	CardInit();
	X12Init();
	if (X12RtcGetClock(&gmt) == 0)
	{
		LogMsg_P(LOG_INFO, PSTR("RTC time [%02d:%02d:%02d]"), gmt.tm_hour, gmt.tm_min, gmt.tm_sec );
	}
	if (At45dbInit() == AT45DB041B)
	{
		
	}
	RcInit();
	KbInit();
	SysControlMainBeat(ON);             // enable 4.4 msecs hartbeat interrupt
	NutThreadSetPriority(1);
	NutThreadCreate("T1", T1, NULL, 512);
	sei();
	NutTimerInit();
	for (;;)
	{
		NutDelay(100);
		if(KbGetKey() == 7){
			LcdBackLight(LCD_BACKLIGHT_ON);
		}else{
			LcdBackLight(LCD_BACKLIGHT_OFF);
		}
		WatchDogRestart();
	}

	return(0);      // never reached, but 'main()' returns a non-void, so.....
}
/* ---------- end of module ------------------------------------------------ */

/*@}*/
