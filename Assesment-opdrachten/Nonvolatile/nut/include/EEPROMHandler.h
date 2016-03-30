/* ========================================================================
 * [PROJECT]    SIR100
 * [MODULE]     EEPROMHandler
 * [TITLE]      EEPROMHandler header file
 * [FILE]       EEPROMHandler.h
 * [VSN]        1.0
 * [CREATED]    18022016
 * [LASTCHNGD]  18022016
 * [COPYRIGHT]  Copyright (C) Avans BV 2010
 * [PURPOSE]    API and gobal defines for display module
 * ======================================================================== */
#ifndef _EEPROMHandler_H
#define _EEPROMHandler_H
/*-------------------------------------------------------------------------*/
/* global defines                                                          */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* typedefs & structs                                                      */
/*-------------------------------------------------------------------------*/

/* wordt UTC en 2 alarmen in opgeslagen*/
struct DataBlock{
	int UTC;
	unsigned char len;
};
struct DataBlock IMCconfig;
/*--------------------------------------------------------------------------*/
/*  Global variables                                                        */
/*--------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* export global routines (interface)                                      */
/*-------------------------------------------------------------------------*/



int saveEEPROM(void);
int readEEPROM(void);
void resetEEPROM(void);

#endif
