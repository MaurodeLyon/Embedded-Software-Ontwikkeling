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
    printf("\nstarting Stream\n");
	startStream();
    for (;;) {
       NutSleep(1000);
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
		printf("creating thread with stream\n");
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
		printf("stopping threadwith stream\n");
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
