/*! \file
* remcon.c contains all interface- and low-level routines that
* perform handling of the infrared bitstream
* [COPYRIGHT]  Copyright (C) STREAMIT BV
*  \version 1.0
*  \date 26 september 2003
*/



#define LOG_MODULE  LOG_REMCON_MODULE

#include <stdio.h>
#include <stdlib.h>
#include <fs/typedefs.h>
#include <sys/heap.h>

#include <sys/event.h>
#include <sys/atom.h>
#include <sys/types.h>
#include <dev/irqreg.h>

#include "system.h"
#include "portio.h"
#include "remcon.h"
#include "display.h"
#include "keyboard.h"
#include "led.h"


/*-------------------------------------------------------------------------*/
/* local variable definitions                                              */
/*-------------------------------------------------------------------------*/
static HANDLE  hRCEvent;

/*-------------------------------------------------------------------------*/
/* local routines (prototyping)                                            */
/*-------------------------------------------------------------------------*/
static void RcInterrupt(void*);
static void RcClearEvent(HANDLE*);


/*!
* \addtogroup RemoteControl
*/

/*@{*/

/*-------------------------------------------------------------------------*/
/*                         start of code                                   */
/*-------------------------------------------------------------------------*/
#define LEADER  3109      	// Leader - ticks in 9 + 4.5 ms
#define LOW		260			// '0' is 1,25 msecs (260 ticks)
#define HIGH	517  		// '1' is 2,25 msecs (517 ticks)

/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
/*!
* \brief ISR Remote Control Interrupt (ISR called by Nut/OS)
*
*  NEC-code consists of 5 parts:
*
*  - leader (9 msec high, 4,5 msec low)
*  - address (8 bits)
*  - inverted address (8 bits)
*  - data (8 bits)
*  - inverted data (8 bits)
*
*  The first sequence contains these 5 parts, next
*  sequences only contain the leader + 1 '0' bit as long
*  as the user holds down the button
*  repetition time is 108 msec in that case
*
*  Resolution of the 16-bit timer we use here is 4,3 usec
*
*  13,5 msecs are 3109 ticks
*  '0' is 1,25 msecs (260 ticks)
*  '1' is 2,25 msecs (517 ticks)
*
* \param *p not used (might be used to pass parms from the ISR)
*/
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
unsigned int TIMEOUT  = 11;       	// the pulse should occur before this time excede Otherwise it is an error 
unsigned int PREPULSE = 8;			// the interrupt should occur after this time Otherwise it is an error
int necpoj = 0;
static void RcInterrupt(void *p)
{
	LedControl(LED_TOGGLE);
	uint32_t tdiff;       
	uint32_t prevTime;
	uint32_t millis = NutGetMillis();

	int pin;
	int rxbuffer;

	tdiff = millis - prevTime;    	// calculate how much time has been passed since last interrupt 
	

	// the time shold be less then time out and greater than PREPULSE 
	/*if ((tdiff>PREPULSE) && (tdiff<TIMEOUT) )		// the edge (interrupt) occurrence time should be less then the TIMOUT and greater then PREPULESE else it is an fake singal
	{												// At the very first edge (necpoj==0)  this conditon will always false and the false block of this if will bring the state machine (necpoj) to position 1(position 1 means 9ms leading pulse has started now we have to wait for 4.5ms start pulse to occur) 
		if(necpoj==1 || necpoj==2)					// when we are hear it means 9ms leding pulse has ended and now we are necpoj=1 or necpoj=2
		{

			if((pin==1) && (necpoj==1))
			{
				necpoj++;
				TIMEOUT 	= 6;  			// timeout for 3rd pulse 5.5ms	
				PREPULSE   	= 3;				// PREPULSE for 3rd pulse 3ms
			}
			else if((pin==0)&& (necpoj ==2))
			{
				necpoj++;
				
				TIMEOUT 	= 3;  			// now data starts so timeout is 2.3ms
				PREPULSE   	= 2;  
				
			}
			else								// this block handle the conditon if any error occur after the completing the pre pulses 
			{
				necpoj = 0;							//reset the state machine 
				TIMEOUT 	=  	11;
				PREPULSE 	= 	8;
			}
		}
		else if(necpoj>2)							//now we are picking the data 	
		{	
			
			necpoj++;					 		//necpoj sill inrement on every edge 	

			if(necpoj&0x01)	// here we check the if necpoj is an odd number because when necpoj goes greater then 3 then 
			//necpoj will always be and odd value when a single bit tranmission is over  
			{
				rxbuffer=rxbuffer<<1;		//shift the buffer 
				if(tdiff>1250)				//we are here means we just recevied the edge of finished tranmission of a bit 
				// so if last edge was more than 1.24 ms then the bit which is just over is one else it is zero 
				{
					rxbuffer = rxbuffer | 0x1;
					//	GPIObits.GPIO5 = !GPIObits.GPIO5;    
				}
				else
				{
					rxbuffer = rxbuffer |0x0;
					//	GPIObits.GPIO4 = !GPIObits.GPIO4;
				}
				
			}
			
			if(necpoj >66)						// we have reached (Leading pulse 2 +address 16+~address16+ command 16+ ~command 16+ last final burst first edge 1)=67th edge of the message frame means the date tranmission is now over 
			{
				
				address 	= (rxbuffer>>24)& 0xFF;			//extract the data from the buffer 
				notaddress 	= (rxbuffer>>16)& 0xFF;
				command 	= (rxbuffer>>8)	& 0xFF;
				notcommand 	= (rxbuffer)	& 0xFF;
				rxbuffer=0;									//clear the buffer	
				
				if((!(address & notaddress)) && (!(command & notcommand)))		// check weather the received data is vaild or not
				{
					dataready =1;
				}
				else
				{
					dataready=0;
				}
				TIMEOUT 	=  	TICKS11ms;					// weather we received the vaild data or not we have to reset the state machine 
				PREPULSE 	= 	TICKS8ms;
				necpoj=0;
			}

		}
		else 
		{	
			
			TIMEOUT 	=	TICKS11ms;							// some error occured reset state machine 
			PREPULSE 	=	TICKS8ms;
		}
		
		


	}
	else
	{
		
		if(pin==0)				//we are here means that after a longtimeout or PREPULSE we just detect a pulse which may be the start of 9ms pulse 
		{
			necpoj = 1;				// yes it could be the start of 9ms pulse 
		}
		else 
		{	
			necpoj = 0;				// no it's not start of 9ms pulse 
		}
		
		address 	= 0xFF;
		notaddress 	= 0xFF;
		command 	= 0xFF;
		notcommand 	= 0xFF;
		dataready 	= 0x000;
		TIMEOUT =  TICKS11ms;		//default timing  
		PREPULSE = TICKS8ms;
	}*/
	// Hier ISR implementeren voor bijv. NEC protocol
}

/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
/*!
* \brief Clear the eventbuffer of this module
*
* This routine is called during module initialization.
*
* \param *pEvent pointer to the event queue
*/
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
static void RcClearEvent(HANDLE *pEvent)
{
	NutEnterCritical();

	*pEvent = 0;

	NutExitCritical();
}



/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
/*!
* \brief Initialise the Remote Control module
*
* - register the ISR in NutOS
* - initialise the HW-timer that is used for this module (Timer1)
* - initialise the external interrupt that inputs the infrared data
* - flush the remote control buffer
* - flush the eventqueue for this module
*/
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
void RcInit()
{
	int nError = 0;

	EICRB &= ~RC_INT_SENS_MASK;    // clear b0, b1 of EICRB

	// Install Remote Control interrupt
	nError = NutRegisterIrqHandler(&sig_INTERRUPT4, RcInterrupt, NULL);
	if (nError == FALSE)
	{
		/*
*  ToDo: control External Interrupt following NutOS calls
#if (NUTOS_VERSION >= 421)
		NutIrqSetMode(&sig_INTERRUPT4, NUT_IRQMODE_FALLINGEDGE);
#else
		EICRB |= RC_INT_FALLING_EDGE;
#endif
		EIMSK |= 1<<IRQ_INT4;         // enable interrupt
*/
		EICRB |= RC_INT_FALLING_EDGE;
		EIMSK |= 1<<IRQ_INT4;         // enable interrupt
	}

	// Initialise 16-bit Timer (Timer1)
	TCCR1B |= (1<<CS11) | (1<<CS10); // clockdivider = 64
	TIFR   |= 1<<ICF1;
	//TIMSK = 1<<TICIE1;

	RcClearEvent(&hRCEvent);


}

/* ---------- end of module ------------------------------------------------ */

/*@}*/
