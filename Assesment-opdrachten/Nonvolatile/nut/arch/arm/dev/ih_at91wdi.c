/*
 * Copyright (C) 2005 by egnite Software GmbH. All rights reserved.
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
 * $Log: ih_at91wdi.c,v $
 * Revision 1.3  2006/07/05 07:56:34  haraldkipp
 * Interrupt handler will be included only, if the related interrupt
 * ID is defined in the platform specific header file.
 *
 * Revision 1.2  2006/06/28 17:10:35  haraldkipp
 * Include more general header file for ARM.
 *
 * Revision 1.1  2005/10/24 08:56:09  haraldkipp
 * First check in.
 *
 */

#include <arch/arm.h>
#include <dev/irqreg.h>

#if defined(WDI_ID)

#ifndef NUT_IRQPRI_WDI
#define NUT_IRQPRI_WDI  4
#endif

static int WatchdogIrqCtl(int cmd, void *param);

IRQ_HANDLER sig_WDI = {
#ifdef NUT_PERFMON
    0,              /* Interrupt counter, ir_count. */
#endif
    NULL,           /* Passed argument, ir_arg. */
    NULL,           /* Handler subroutine, ir_handler. */
    WatchdogIrqCtl  /* Interrupt control, ir_ctl. */
};

/*!
 * \brief Watchdog interrupt entry.
 */
void WatchdogIrqEntry(void) __attribute__ ((naked));
void WatchdogIrqEntry(void)
{
    IRQ_ENTRY();
#ifdef NUT_PERFMON
    sig_WDI.ir_count++;
#endif
    if (sig_WDI.ir_handler) {
        (sig_WDI.ir_handler) (sig_WDI.ir_arg);
    }
    IRQ_EXIT();
}

/*!
 * \brief Watchdog interrupt control.
 *
 * \param cmd   Control command.
 *              - NUT_IRQCTL_INIT Initialize and disable interrupt.
 *              - NUT_IRQCTL_STATUS Query interrupt status.
 *              - NUT_IRQCTL_ENABLE Enable interrupt.
 *              - NUT_IRQCTL_DISABLE Disable interrupt.
 *              - NUT_IRQCTL_GETPRIO Query interrupt priority.
 *              - NUT_IRQCTL_SETPRIO Set interrupt priority.
 *              - NUT_IRQCTL_GETCOUNT Query and clear interrupt counter.
 * \param param Pointer to optional parameter.
 *
 * \return 0 on success, -1 otherwise.
 */
static int WatchdogIrqCtl(int cmd, void *param)
{
    int rc = 0;
    u_int *ival = (u_int *)param;
    int enabled = inr(AIC_IMR) & _BV(WDI_ID);

    /* Disable interrupt. */
    if (enabled) {
        outr(AIC_IDCR, _BV(WDI_ID));
    }

    switch(cmd) {
    case NUT_IRQCTL_INIT:
        /* Set the vector. */
        outr(AIC_SVR(WDI_ID), (unsigned int)WatchdogIrqEntry);
        /* Initialize to edge triggered with defined priority. */
        outr(AIC_SMR(WDI_ID), AIC_SRCTYPE_INT_EDGE_TRIGGERED | NUT_IRQPRI_WDI);
        /* Clear interrupt */
        outr(AIC_ICCR, _BV(WDI_ID));
        break;
    case NUT_IRQCTL_STATUS:
        if (enabled) {
            *ival |= 1;
        }
        else {
            *ival &= ~1;
        }
        break;
    case NUT_IRQCTL_ENABLE:
        enabled = 1;
        break;
    case NUT_IRQCTL_DISABLE:
        enabled = 0;
        break;
    case NUT_IRQCTL_GETPRIO:
        *ival = inr(AIC_SMR(WDI_ID)) & AIC_PRIOR;
        break;
    case NUT_IRQCTL_SETPRIO:
        outr(AIC_SMR(WDI_ID), (inr(AIC_SMR(WDI_ID)) & ~AIC_PRIOR) | *ival);
        break;
#ifdef NUT_PERFMON
    case NUT_IRQCTL_GETCOUNT:
        *ival = (u_int)sig_WDI.ir_count;
        sig_WDI.ir_count = 0;
        break;
#endif
    default:
        rc = -1;
        break;
    }

    /* Enable interrupt. */
    if (enabled) {
        outr(AIC_IECR, _BV(WDI_ID));
    }
    return rc;
}

#endif /* WDI_ID */
