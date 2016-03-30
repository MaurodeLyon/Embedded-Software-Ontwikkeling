#include <stdio.h>
#include <string.h>
#include <sys/thread.h>
#include <dev/board.h>
#include <io.h>
#include <sys/timer.h>

#include "menu.h"
#include "keyboard.h"
#include "lcd.h"
#include "StreamWork.h"

/*-------------------------------------------------------------------------*/
/* local variable definitions                                              */
/*-------------------------------------------------------------------------*/
#define DBG_DEVNAME "uart0"
#define DBG_BAUDRATE 115200
#define MY_MAC { 0x00, 0x06, 0x98, 0x10, 0x01, 0x10 }
#define MY_IPADDR "192.168.192.100"
#define MY_IPMASK "255.255.255.0"
#define MY_IPGATE "192.168.192.122"
#define RADIO_IPADDR "145.58.53.152" //3FM 80
#define RADIO_PORT 80
//#define RADIO_URL "/juizefm"
#define RADIO_URL "/3fm-bb-mp3"
//#define RADIO_URL "/Qmusic_nl_live_96.mp3"
//#define RADIO_URL "/3fm-test-alt-mp3"
//#define RADIO_URL "/stream/1020"
#define MAX_HEADERLINE 512
#define TCPIP_BUFSIZ 8760
#define TCPIP_MSS 1460
#define TCPIP_READTIMEOUT 3000
#define DBG_DEVICE devDebug0
/*-------------------------------------------------------------------------*/
/* local routines (prototyping)                                            */
/*-------------------------------------------------------------------------*/
void drawMenu(void);
void onMenu(int input);
void offMenu(int input);
//on/off

//scrollmenu

/*-------------------------------------------------------------------------*/
/*                         start of code                                   */
/*-------------------------------------------------------------------------*/

THREAD(stream, arg)
{
	startStream();
	for(;;)
	{
		NutSleep(100);
	}	
}

void initMenu(void)
{	
	stateMenu = onMenu;
	clearScreen();
	drawMenu();
}

void drawMenu()
{
	if(stateMenu == onMenu)
	{
		printString("on");
	}
	else if(stateMenu == offMenu)
	{
		printString("off");
	}
}

//menu
void onMenu(int input)
{
	switch(input)
	{
	case KEY_OK:
		NutThreadCreate("Bg", stream, NULL, 512);
		break;
	case KEY_LEFT:
		stateMenu = offMenu;
		break;
	case KEY_RIGHT:
		stateMenu = offMenu;
		break;
	default:
		break;
	}
	clearScreen();
	drawMenu();
}

void offMenu(int input)
{
	switch(input)
	{
	case KEY_OK:
		stopStream();
		break;
	case KEY_LEFT:
		stateMenu = onMenu;
		break;
	case KEY_RIGHT:
		stateMenu = onMenu;
		break;
	default:
		break;
	}
	clearScreen();
	drawMenu();
}
