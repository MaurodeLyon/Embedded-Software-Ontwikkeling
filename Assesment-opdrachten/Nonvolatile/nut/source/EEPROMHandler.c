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
#include <dev/board.h>

#include <sys/types.h>
#include <sys/timer.h>
#include <sys/event.h>
#include <sys/thread.h>
#include <sys/heap.h>
#include <dev/nvmem.h>

#include "system.h"
#include "portio.h"
#include "display.h"
#include "log.h"
#include "EEPROMHandler.h"
#include "led.h"
#include "alarm.h"
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
int checkEEPROM(void);

/*-------------------------------------------------------------------------*/
/*                         start of code                                   */
/*-------------------------------------------------------------------------*/

//EEPROM address table:
//address	||	contains
//256		||	config struct
//257		||	Alarm1
//258		||	Alarm2

/**Lees alle EEPROM uit en kijk of er wat in staat en sla het daarna weer op.*/
int readEEPROM(void)
{
	printf("reading EEPROM\n");
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
	
	result = checkEEPROM();
	
	//The size value of the struct is updated for future checks
	IMCconfig.len = sizeof(IMCconfig);

	//uconf.count++;
	
	//Save the new and/or updated 
	NutNvMemSave(256, &IMCconfig, sizeof(IMCconfig));

	//Return 0 if succes and -1 if 0 if success
	return result;
}
/**opslaan alle gegevens in EEPROM*/
int saveEEPROM(void)
{
	printf("saving EEPROM\n");
	int result = -1;
	//Initialise connection to AVR EEPROM
	unsigned long baud = 115200;
	NutRegisterDevice(&DEV_DEBUG, 0, 0);
	
	freopen(DEV_DEBUG_NAME, "w", stdout);
	_ioctl(_fileno(stdout), UART_SETSPEED, &baud);
	
	//Save config
	
	result = NutNvMemSave(256, &IMCconfig, sizeof(IMCconfig));

	//Return 0 if succes and -1 if 0 if success
	return result;
}
/**Verwijder de data in het EEPROM*/
void resetEEPROM(void)
{
	printf("reseting EEPROM\n");
	//Sets the size value of the config to 0. 
	//Next time the config is loaded it will think the config is either nonexistent or corrupt and thus will overwrite it.
	IMCconfig.len=0;
	saveEEPROM();
}
/**Controleer of er data in de EEPROM staat*/
int checkEEPROM(void)
{
	printf("\n-----Checking EEPROM-----\n");
	int result = -1;
	struct Alarm alarm11;
	struct Alarm alarm22;
	
	//checks if there is a size mismatch
	//A new config struct will be initialised if not.
	if (IMCconfig.len != sizeof(IMCconfig)) 
	{
		printf("Size mismatch: Either the UTC config is corrupt or it does not exist. Creating new config... \n");
		IMCconfig.UTC=0;
		alarm11.volume=0;
		alarm11.isSet=0;
		alarm11.status=0;
		alarm22.volume=0;
		alarm22.isSet=0;
		alarm22.status=0;
		IMCconfig.alarm1=alarm11;
		IMCconfig.alarm2=alarm22;
	} 
	else 
	{
		result=0;
		printf("configuration loaded succesfully. configuration size: %d : %d :%d \n", IMCconfig.len,IMCconfig.alarm1.volume, IMCconfig.alarm2.volume);
		printf("UTC: %d \n", IMCconfig.UTC);
	}
	return result;	
}
