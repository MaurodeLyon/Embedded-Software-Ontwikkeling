/* ========================================================================
* [PROJECT]    SIR100
* [MODULE]     EEPROMHandler
* [TITLE]      EEPROMHandler source file
* [FILE]       EEPROMHandler.c
* [VSN]        1.0
* [CREATED]    18022016
* [LASTCHNGD]  18022016
* [COPYRIGHT]  Copyright (C) Avans BV
* [PURPOSE]    Contains all functions for initializing and handling first time
				use of the system.
* ======================================================================== */

#define LOG_MODULE  LOG_DISPLAY_MODULE

#include <stdio.h>
#include <string.h>
#include <io.h>


#include <sys/types.h>
#include <sys/timer.h>
#include <sys/event.h>
#include <sys/thread.h>
#include <sys/heap.h>
#include <dev/board.h>
#include <dev/nvmem.h>

#include "system.h"
#include "portio.h"
#include "display.h"
#include "log.h"
#include "EEPROMHandler.h"
#include "led.h"
#include "rtc.h"




/*-------------------------------------------------------------------------*/
/* local defines                                                           */
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/* local variable definitions                                              */
/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/* local routines (prototyping)                                            */
/*-------------------------------------------------------------------------*/

/*@{*/

/*-------------------------------------------------------------------------*/
/*                         start of code                                   */
/*-------------------------------------------------------------------------*/

//EEPROM address table:
//address	||	contains
//256		||	config struct


int readEEPROM(void)
{
			printf("-----Reading EEPROM------\n");
			int result=-1;
			
			//Initialise connection to AVR EEPROM
			unsigned long baud = 115200;
			//struct _USER_CONFIG uconf;
		 
			NutRegisterDevice(&DEV_DEBUG, 0, 0);
		 
			freopen(DEV_DEBUG_NAME, "w", stdout);
			_ioctl(_fileno(stdout), UART_SETSPEED, &baud);
		 
			
			//Load previously saved struct if available
			NutNvMemLoad(256, &IMCconfig, sizeof(IMCconfig));
		 
			//Checks if the loaded struct has the correct size
			if (IMCconfig.len != sizeof(IMCconfig)) 
			{
				//If not it means that either the struct is corrupt or it did not exist. A new config struct will be initialised
				puts("Size mismatch: Either the config is corrupt or it does not exist. Creating new config... \n");
				IMCconfig.UTC=0;
			} 
			else 
			{
				result=0;
				printf("configuration loaded succesfully. configuration size: %d \n", IMCconfig.len);
			}
			
			//The size value of the struct is updated for future checks
			IMCconfig.len = sizeof(IMCconfig);
			//uconf.count++;
			
			//Save the new and/or updated 
			NutNvMemSave(256, &IMCconfig, sizeof(IMCconfig));
			
			//Return 0 if succes and -1 if 0 if success
			return result;
}

int saveEEPROM(void)
{
			printf("-----Saving EEPROM------\n");
			int result = -1;
			//Initialise connection to AVR EEPROM
			unsigned long baud = 115200;
			NutRegisterDevice(&DEV_DEBUG, 0, 0);
		 
			freopen(DEV_DEBUG_NAME, "w", stdout);
			_ioctl(_fileno(stdout), UART_SETSPEED, &baud);
		 
			//Save config
			
			result = NutNvMemSave(256, &IMCconfig, sizeof(IMCconfig));
			
			//Return 0 if succes and -1 if failed
			return result;
}

void resetEEPROM(void)
{
	printf("-----Resetting EEPROM------\n");
	//Sets the size value of the config to 0. 
	//Next time the config is loaded it will think the config is either nonexistent or corrupt and thus will overwrite it.
	IMCconfig.len=0;
	saveEEPROM();

}
