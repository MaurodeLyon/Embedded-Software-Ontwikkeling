/*
 * <MFS> Modified for Streamit
 * Added cpu-load modifications in THREAD(NutIdle, arg)
 *
 * Copyright (C) 2001-2006 by egnite Software GmbH. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 *
 */

/*
 * $Log: nutinit.c,v $
 * Revision 1.9  2006/10/07 00:24:14  hwmaier
 * Include of sys/heap.h added.
 *
 * Revision 1.8  2006/09/29 12:39:23  haraldkipp
 * Added support for ATmega2561.
 *
 * Revision 1.7  2006/07/04 03:38:56  hwmaier
 * Changed binary constants to hex constants in XNUT board
 * initialization code.
 *
 * Revision 1.6  2006/05/15 11:46:49  haraldkipp
 * Added heartbeat port bit, which is regularly toggled by the idle thread.
 * Helps to develop on boards with external watchdog hardware that can't be
 * disabled.
 *
 * Revision 1.5  2005/10/17 08:24:55  hwmaier
 * All platform specific initialisation (CPLD, IO pins etc.) has been consolidated using the new PLATFORM macro into a new function called NutCustomInit()
 *
 * Revision 1.4  2005/10/04 06:11:11  hwmaier
 * Added support for separating stack and conventional heap as required by AT09CAN128 MCUs
 *
 * Revision 1.3  2005/09/07 16:22:45  christianwelzel
 * Added MMnet02 CPLD initialization
 *
 * Revision 1.2  2005/08/02 17:46:46  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.1  2005/05/27 17:17:31  drsung
 * Moved the file
 *
 * Revision 1.18  2005/05/16 08:49:37  haraldkipp
 * Arthernet requires different wait state settings.
 *
 * Revision 1.17  2005/02/28 08:44:54  drsung
 * Fixed missing return type of NutInitSP
 * Removed inlcude file avrpio.h
 *
 * Revision 1.16  2005/02/26 12:09:28  drsung
 * Moved heap initialization to section .init5 to support c++ constructors for static objects.
 *
 * Revision 1.15  2005/02/10 07:06:48  hwmaier
 * Changes to incorporate support for AT90CAN128 CPU
 *
 * Revision 1.12  2005/01/22 19:30:56  haraldkipp
 * Fixes Ethernut 1.3G memory bug.
 *
 * Revision 1.11  2004/11/08 18:58:59  haraldkipp
 * Configurable stack sizes
 *
 * Revision 1.10  2004/09/01 14:27:03  haraldkipp
 * Using configuration values from cfg/memory.h.
 * Added configurable reserved memory area.
 * Automatic check for external memory removed.
 *
 * Revision 1.9  2004/07/28 13:43:25  drsung
 * Corrected a misplaced #endif after last change.
 *
 * Revision 1.8  2004/07/09 19:51:17  freckle
 * Added new function NutThreadSetSleepMode to tell nut/os to set the MCU
 * into sleep mode when idle (avr-gcc && avr128 only)
 *
 * Revision 1.7  2004/07/09 14:40:43  freckle
 * Moved ((volatile u_char *) NUTRAMEND) cast into NUTRAMENDPTR define
 *
 * Revision 1.6  2004/07/09 14:23:13  freckle
 * Allow setting of NUTRAMEND by giving it as a compiler flag
 *
 * Revision 1.5  2004/05/25 17:13:48  drsung
 * Bit name SRW10 is not defined for atmega103, so added some defines to make it compatible.. :-X
 *
 * Revision 1.4  2004/05/25 12:03:37  olereinhardt
 * Sorry, fixed typing bug
 *
 * Revision 1.3  2004/05/25 12:00:37  olereinhardt
 * Newly added 3Waitstate support now needs to be enabled by
 * defining NUT_3WAITSTATES. By default this behaves like normal
 *
 *
 * Revision 1.1  2004/03/16 16:48:46  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 *
 */

#include <sys/thread.h>
#include <sys/heap.h>
#include <cfg/memory.h>
#include <cfg/os.h>
#include <cfg/arch/avr.h>
#include <cfg/arch.h>

#include <cpu_load.h>

/*!
 * \addtogroup xgNutArchAvrInit
 */
/*@{*/

#ifdef NUTXMEM_SIZE
/*!
 * \brief Last memory address using external SRAM.
 */
#define NUTMEM_END (u_short)(NUTXMEM_START + (u_short)NUTXMEM_SIZE - 1U)

#else
/*!
 * \brief Last memory address without using external SRAM.
 *
 * \todo Shall we support NUTRAMEND for backward compatibility? If, then
 *       let's do it in cfg/memory.h.
 */
#define NUTMEM_END (u_short)(NUTMEM_START + (u_short)NUTMEM_SIZE - 1U)

#endif

#ifdef NUTMEM_RESERVED
/*!
 * \brief Number of bytes reserved in on-chip memory.
 *
 * AVR offers the option to temporarly use address and data bus
 * lines as general purpose I/O. If such drivers need data memory,
 * this must be located at internal memory addresses.
 *
 * \todo Not a nice implementation but works as long as this module
 *       is linked first. Should be made a linker option.
 */
u_char nutmem_onchip[NUTMEM_RESERVED];
#endif

/* sleep mode to put avr in idle thread, SLEEP_MODE_NONE is used for for non sleeping */
#if defined(__GNUC__) && defined(__AVR_ENHANCED__)

u_char idle_sleep_mode = SLEEP_MODE_NONE;

/* AT90CAN128 uses a different register to enter sleep mode */
#if defined(SMCR)
#define AVR_SLEEP_CTRL_REG    SMCR
#else
#define AVR_SLEEP_CTRL_REG    MCUCR
#endif

#endif

/*
 * Macros used for RTL8019AS EEPROM Emulation.
 * See FakeNicEeprom().
 */
#if defined(RTL_EESK_BIT) && defined(__GNUC__)

#ifndef RTL_BASE_ADDR
#define RTL_BASE_ADDR 0x8300
#endif
#define NIC_CR  _MMIO_BYTE(RTL_BASE_ADDR)
#define NIC_EE  _MMIO_BYTE(RTL_BASE_ADDR + 1)

#if (RTL_EEMU_AVRPORT == AVRPORTB)
#define RTL_EEMU_PORT   PORTB
#define RTL_EEMU_DDR    DDRB

#elif (RTL_EEMU_AVRPORT == AVRPORTD)
#define RTL_EEMU_PORT   PORTD
#define RTL_EEMU_DDR    DDRD

#elif (RTL_EEMU_AVRPORT == AVRPORTE)
#define RTL_EEMU_PORT   PORTE
#define RTL_EEMU_DDR    DDRE

#elif (RTL_EEMU_AVRPORT == AVRPORTF)
#define RTL_EEMU_PORT   PORTF
#define RTL_EEMU_DDR    DDRF

#else
#define RTL_EE_MEMBUS
#define RTL_EEMU_PORT   PORTC
#define RTL_EEMU_DDR    DDRC

#endif /* RTL_EEMU_AVRPORT */

#if (RTL_EEDO_AVRPORT == AVRPORTB)
#define RTL_EEDO_PORT   PORTB
#define RTL_EEDO_DDR    DDRB

#elif (RTL_EEDO_AVRPORT == AVRPORTD)
#define RTL_EEDO_PORT   PORTD
#define RTL_EEDO_DDR    DDRD

#elif (RTL_EEDO_AVRPORT == AVRPORTE)
#define RTL_EEDO_PORT   PORTE
#define RTL_EEDO_DDR    DDRE

#elif (RTL_EEDO_AVRPORT == AVRPORTF)
#define RTL_EEDO_PORT   PORTF
#define RTL_EEDO_DDR    DDRF

#else
#define RTL_EE_MEMBUS
#define RTL_EEDO_PORT   PORTC
#define RTL_EEDO_DDR    DDRC

#endif /* RTL_EEDO_AVRPORT */

#if (RTL_EESK_AVRPORT == AVRPORTB)
#define RTL_EESK_PIN    PINB
#define RTL_EESK_DDR    DDRB

#elif (RTL_EESK_AVRPORT == AVRPORTD)
#define RTL_EESK_PIN    PIND
#define RTL_EESK_DDR    DDRD

#elif (RTL_EESK_AVRPORT == AVRPORTE)
#define RTL_EESK_PIN    PINE
#define RTL_EESK_DDR    DDRE

#elif (RTL_EESK_AVRPORT == AVRPORTF)
#define RTL_EESK_PIN    PINF
#define RTL_EESK_DDR    DDRF

#else
#define RTL_EE_MEMBUS
#define RTL_EESK_PIN    PINC
#define RTL_EESK_DDR    DDRC

#endif /* RTL_EESK_AVRPORT */
#endif /* RTL_EESK_BIT && __GNUC__ */

#ifdef __GNUC__
/*
 * Some special declarations for AVRGCC. The runtime library
 * executes section .init8 before finally jumping to main().
 * We never let it do that jump, but start main() as a
 * separate thread. This introduces new problems:
 * 1. The compiler reinitializes the stack pointer when
 *    entering main, at least version 3.3 does it.
 * 2. The compiler doesn't allow to redeclare main to make
 *    it fit for NutThreadCreate().
 * 3. The runtime library requires, that main is defined
 *    somewhere.
 * Solutions:
 * 1. We do not use main at all, but let the preprocessor
 *    redefine it to NutAppMain() in the application code.
 *    See compiler.h. Note, that the declaration of NutAppMain
 *    in this module differs from the one in the application
 *    code. Fortunately the linker doesn't detect this hack.
 * 2. We use a linker option to set the symbol main to zero.
 *
 * Thanks to Joerg Wunsch, who helped to solve this.
 */
void NutInit(void) __attribute__ ((naked)) __attribute__ ((section(".init8")));
extern void NutAppMain(void *arg) __attribute__ ((noreturn));
#else
extern void main(void *);
#endif

/*
 * External memory interface for GCC.
 */
#if defined(__GNUC__) && defined(NUTXMEM_SIZE)

/*
 * At the very beginning enable extended memory interface.
 */
static void NutInitXRAM(void) __attribute__ ((naked, section(".init1"), used));
void NutInitXRAM(void)
{
#if defined(__AVR_AT90CAN128__) || defined(__AVR_ATmega2561__)
/*
 * Note: Register structure of ATCAN128 differs from ATMEGA128 in regards
 * to wait states.
 */
#ifdef NUT_3WAITSTATES /* One wait state 1 for low, 3 for high memory range */
    XMCRA = _BV(SRE) | _BV(SRL2) | _BV(SRW00) | _BV(SRW10) | _BV(SRW11);
#else
    XMCRA = _BV(SRE) | _BV(SRW10); /* One wait state for the whole memory range */
#endif

#elif defined(__AVR_ATmega128__)

    MCUCR = _BV(SRE) | _BV(SRW10);

/* Configure two sectors, lower sector = 0x1100 - 0x7FFF,
 * Upper sector = 0x8000 - 0xFFFF and run 3 wait states for the
 * upper sector (NIC), 1 wait state for lower sector (XRAM).
 */
#ifdef NUT_3WAITSTATES
    XMCRA |= _BV(SRL2) | _BV(SRW00) | _BV(SRW11); /* SRW10 is set in MCUCR */
    XMCRB = 0;
#endif

#else  /* ATmega103 */
    MCUCR = _BV(SRE) | _BV(SRW);
#endif
}

#endif


#if defined(RTL_EESK_BIT) && defined(__GNUC__) && defined(NUTXMEM_SIZE)
/*
 * Before using extended memory, we need to run the RTL8019AS EEPROM emulation.
 * Not doing this may put this controller in a bad state, where it interferes
 * the data/address bus.
 *
 * This function has to be called as early as possible but after the external
 * memory interface has been enabled.
 *
 * The following part is used by the GCC environment only. For ICCAVR it has
 * been included in the C startup file.
 */
static void FakeNicEeprom(void) __attribute__ ((naked, section(".init1"), used));
void FakeNicEeprom(void)
{
    /*
     * Prepare the EEPROM emulation port bits. Configure the EEDO
     * and the EEMU lines as outputs and set both lines to high.
     */
#ifdef RTL_EEMU_BIT
    sbi(RTL_EEMU_PORT, RTL_EEMU_BIT);
    sbi(RTL_EEMU_DDR, RTL_EEMU_BIT);
#endif
    sbi(RTL_EEDO_PORT, RTL_EEDO_BIT);
    sbi(RTL_EEDO_DDR, RTL_EEDO_BIT);

    /* Force the chip to re-read the EEPROM contents. */
    NIC_CR = 0xE1;
    NIC_EE = 0x40;

    /* No external memory access beyond this point. */
#ifdef RTL_EE_MEMBUS
    cbi(MCUCR, SRE);
#endif

    /*
     * Loop until the chip stops toggling our EESK input. We do it in
     * assembly language to make sure, that no external RAM is used
     * for the counter variable.
     */
    __asm__ __volatile__("\n"   /* */
                         "EmuLoop:               " "\n" /* */
                         "        ldi  r24, 0    " "\n" /* Clear counter. */
                         "        ldi  r25, 0    " "\n" /* */
                         "        sbis %0, %1    " "\n" /* Check if EESK set. */
                         "        rjmp EmuClkClr " "\n" /* */
                         "EmuClkSet:             " "\n" /* */
                         "        adiw r24, 1    " "\n" /* Count loops with EESK set. */
                         "        breq EmuDone   " "\n" /* Exit loop on counter overflow. */
                         "        sbis %0, %1    " "\n" /* Test if EESK is still set. */
                         "        rjmp EmuLoop   " "\n" /* EESK changed, do another loop. */
                         "        rjmp EmuClkSet " "\n" /* Continue waiting. */
                         "EmuClkClr:             " "\n" /* */
                         "        adiw r24, 1    " "\n" /* Count loops with EESK clear. */
                         "        breq EmuDone   " "\n" /* Exit loop on counter overflow. */
                         "        sbic %0, %1    " "\n" /* Test if EESK is still clear. */
                         "        rjmp EmuLoop   " "\n" /* EESK changed, do another loop. */
                         "        rjmp EmuClkClr " "\n" /* Continue waiting. */
                         "EmuDone:               \n\t"  /* */
                         :      /* No outputs. */
                         :"I"(_SFR_IO_ADDR(RTL_EESK_PIN)), /* Emulation port. */
                          "I"(RTL_EESK_BIT)                 /* EESK bit. */
                         :"r24", "r25");

    /* Enable memory interface. */
#ifdef RTL_EE_MEMBUS
    sbi(MCUCR, SRE);
#endif

    /* Reset port outputs to default. */
#ifdef RTL_EEMU_BIT
    cbi(RTL_EEMU_PORT, RTL_EEMU_BIT);
    cbi(RTL_EEMU_DDR, RTL_EEMU_BIT);
#endif
    cbi(RTL_EEDO_PORT, RTL_EEDO_BIT);
    cbi(RTL_EEDO_DDR, RTL_EEDO_BIT);
}

#endif /* RTL_EESK_BIT && __GNUC__ && NUTXMEM_SIZE */

/*! \fn NutThreadSetSleepMode(u_char mode)
 * \brief Sets the sleep mode to enter in Idle thread
 *
 * If the idle thread is running, no other thread is active
 * so we can safely put the mcu to sleep.
 *
 * \param mode one of the sleep modes defined in avr/sleep.h or
 *             sleep_mode_none (don't enter sleep mode)
 */
#if defined(__GNUC__) && defined(__AVR_ENHANCED__)
void NutThreadSetSleepMode(u_char mode)
{
    idle_sleep_mode = mode;
}
#endif

/*!
 * \brief AVR Idle thread.
 *
 * Running at priority 254 in an endless loop.
 */
THREAD(NutIdle, arg)
{
#if defined(__GNUC__) && defined(__AVR_ENHANCED__)
    u_char sleep_mode;
#endif
#ifdef IDLE_HEARTBEAT_BIT
    u_char beat = 0;
#endif
    /* Initialize system timers. */
    NutTimerInit();

    /* Create the main application thread. */
    NutThreadCreate("main", main, 0, NUT_THREAD_MAINSTACK);

    /*
     * Run in an idle loop at the lowest priority. We can still
     * do something useful here, like killing terminated threads
     * or putting the CPU into sleep mode.
     */
    NutThreadSetPriority(254);
    for (;;) {

#ifdef CPU_LOAD_MEASURE
    cbi (CPU_LOAD_PORT, CPU_LOAD_BIT); /* I have something to do... */
#endif
        NutThreadYield();
        NutThreadDestroy();

#ifdef CPU_LOAD_MEASURE
    sbi (CPU_LOAD_PORT, CPU_LOAD_BIT); /* I am ready to sleep... */
    sbi (CPU_LOAD_DDR, CPU_LOAD_BIT);
#endif

#ifdef IDLE_HEARTBEAT_BIT
        if ((beat = !beat) == 0) {
            //UDR = '*';
            cbi(IDLE_HEARTBEAT_PORT, IDLE_HEARTBEAT_BIT);
        }
        else {
            sbi(IDLE_HEARTBEAT_PORT, IDLE_HEARTBEAT_BIT);
        }
        sbi(IDLE_HEARTBEAT_DDR, IDLE_HEARTBEAT_BIT);
#endif

#if defined(__GNUC__) && defined(__AVR_ENHANCED__)
        if (idle_sleep_mode != SLEEP_MODE_NONE) {
            sleep_mode = AVR_SLEEP_CTRL_REG & _SLEEP_MODE_MASK;
            set_sleep_mode(idle_sleep_mode);
            /* Note:  avr-libc has a sleep_mode() function, but it's broken for
            AT90CAN128 with avr-libc version earlier than 1.2 */
            AVR_SLEEP_CTRL_REG |= _BV(SE);
            __asm__ __volatile__ ("sleep" "\n\t" :: );
            AVR_SLEEP_CTRL_REG &= ~_BV(SE);
            set_sleep_mode(sleep_mode);
        }
#endif
    }
}

#if defined(__GNUC__)
static void NutInitSP(void) __attribute__ ((naked, section (".init5"), used));
void NutInitSP(void)
{
#if defined (__AVR_AT90CAN128__)
    /* Stack must remain in internal RAM where avr-libc's runtime lib init placed it */
#else
   /* Initialize stack pointer to end of external RAM while starting up the system
    * to avoid overwriting .data and .bss section.
    */
    SP = (u_short)(NUTMEM_END);
#endif
}
#endif

#if defined(__GNUC__)
static void NutInitHeap(void) __attribute__ ((naked, section (".init5"), used));
#endif
void NutInitHeap()
{
#if defined (NUTMEM_STACKHEAP) /* Stack resides in internal memory */
    NutStackAdd((void *) NUTMEM_START, NUTMEM_STACKHEAP);
#endif
    /* Then add the remaining RAM to heap.
     *
     * 20.Aug.2004 haraldkipp: This had been messed up somehow. It's nice to have
     * one continuous heap area, but we lost the ability to have systems with
     * a gap between internal and external RAM.
     */
    if ((u_short)NUTMEM_END - (u_short) (&__heap_start) > 384) {
        NutHeapAdd(&__heap_start, (u_short) NUTMEM_END - 256 - (u_short) (&__heap_start));
    }
}

#if defined(__GNUC__)
static void NutCustomInit(void) __attribute__ ((naked, section (".init1"), used));
#endif
/*!
 * NutCustomInit is a container function for hardware specific init code.
 *
 * The hardware is selected with a PLATFORM macro definition.
 *
 * Typically this function configures CPLDs, enables chips,
 * overwrites NutInitXRAM's default wait state settings, sets the default
 * baudrate for NUDEBUG as they depend on the crystal frequency used, etc.
 */
void NutCustomInit(void)
/*
* MMnet02 CPLD initialization.
*/
#if defined(MMNET02)
{
    volatile u_char *breg = (u_char *)((size_t)-1 & ~0xFF);

    *(breg + 1) = 0x01; // Memory Mode 1, Banked Memory

    /* Assume 14.745600 MHz crystal, set to 115200bps */
    outp(7, UBRR);
    outp(7, UBRR1L);
}
/*
 * Arthernet CPLD initialization.
 */
#elif defined(ARTHERNET1)
{
    /* Arthernet1 memory setup - mt - TODO: check this
    Note: This overwrites the default settings of NutInitXRAM()!
    0x1100-0x14FF  CLPD area  -> use 3 Waitstates for 0x1100-0x1FFF (no Limit at 0x1500 available)
    0x1500-0xFFFF  Heap/Stack -> use 1 Waitstate  for 0x2000-0xFFFF
    */
    MCUCR  = _BV(SRE); /* enable xmem-Interface */
    XMCRA |= _BV(SRL0) | _BV(SRW01) | _BV(SRW00); /* sep. at 0x2000, 3WS for lower Sector */
    XMCRB = 0;

    *((volatile u_char *)(ARTHERCPLDSTART)) = 0x10; // arthernet cpld init - Bank
    *((volatile u_char *)(ARTHERCPLDSPI)) = 0xFF; // arthernet cpld init - SPI

    /* Assume standard Arthernet1 with 16 MHz crystal, set to 38400 bps */
    outp(25, UBRR);
    outp(25, UBRR1L);
}
/*
* XNUT board initialization
*/
#elif defined(XNUT_100) || defined(XNUT_105)
{
    PORTB = 0x35;
    DDRB  = 0x3F;
    PORTD = 0xE8;
    DDRD  = 0xB0;
    PORTE = 0x0E;
    DDRE  = 0x02;
    PORTF = 0xF0;
    DDRF  = 0x0F;
    PORTG = 0x1F;
    DDRG  = 0x07;

    ACSR |= _BV(ACD); /* Switch off analog comparator to reduce power consumption */

    /* Init I2C bus w/ 100 kHz */
    TWSR = 0;
    TWBR = (NUT_CPU_FREQ / 100000UL - 16) / 2; /* 100 kHz I2C */

    /* Set default baudrate */
#if NUT_CPU_FREQ == 14745600
    UBRR0L = (NUT_CPU_FREQ / (16 * 9600UL)) - 1;
    UBRR1L = (NUT_CPU_FREQ / (16 * 9600UL)) - 1;
#else
    sbi(UCSR0A, U2X0);
    sbi(UCSR1A, U2X1);
    UBRR0L = (NUT_CPU_FREQ / (8 * 9600UL)) - 1;
    UBRR1L = (NUT_CPU_FREQ / (8 * 9600UL)) - 1;
#endif
}
/*
 * Rest of the world and standard ETHERNUT 1/2
 */
#else
{
    /* Assume standard Ethernut with 14.745600 MHz crystal, set to 115200bps */
    outp(7, UBRR);
#ifdef __AVR_ENHANCED__
    outp(7, UBRR1L);
#endif
}
#endif

/*!
 * \brief Nut/OS Initialization.
 *
 * Initializes the memory management and the thread system and starts
 * an idle thread, which in turn initializes the timer management.
 * Finally the application's main() function is called.
 *
 * Depending on the compiler, different methods are used to execute this
 * function before main() is called.
 *
 * For ICCAVR the default crtatmega.o startup file is replaced by
 * crtnut.o, which calls NutInit instead of main(). This is done
 * by adding the following compiler options in the project:
 * \code -ucrtnut.o nutinit.o \endcode
 *
 * crtnut.o should be replaced by crtnutram.o, if the application's
 * variable space exceeds 4kB. For boards with RTL8019AS and EEPROM
 * emulation (like Ethernut 1.3 Rev-G) use crtenut.o or crtenutram.o.
 *
 * For AVRGCC this function is located in section .init8, which is
 * called immediately before jumping to main(). NutInit is defined
 * as:
 * \code
 * void NutInit(void) __attribute__ ((naked)) __attribute__ ((section (".init8")));
 * \endcode
 *
 * \todo Make heap threshold configurable, currently hardcoded at 384.
 *
 * \todo Make wait states for external memory access configuratble.
 *
 * \todo Make early UART initialization for kernel debugging configurable.
 */
void NutInit(void)
{
    /*
     * We can't use local variables in naked functions.
     */

#ifdef NUTDEBUG
    /* Note: The platform's default baudrate will be set in NutCustomInit() */
    outp(BV(RXEN) | BV(TXEN), UCR);
#endif

#ifndef __GNUC__
    NutCustomInit();

    /* Initialize stack pointer to end of external RAM while starting up the system
     * to avoid overwriting .data and .bss section.
     */
    SP = (u_short)(NUTMEM_END);

    /* Initialize the heap memory
     */
    NutInitHeap();
#endif /* __GNUC__ */

    /*
     * Read eeprom configuration.
     */
    if (NutLoadConfig()) {
        strcpy(confos.hostname, "ethernut");
        NutSaveConfig();
    }

    /* Create idle thread
     */
    NutThreadCreate("idle", NutIdle, 0, NUT_THREAD_IDLESTACK);
}

/*@}*/
