/*
 * Copyright (C) 2003-2005 by egnite Software GmbH. All rights reserved.
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
 * $Log: dm9000e.c,v $
 * Revision 1.6  2007/05/02 11:22:51  haraldkipp
 * Added multicast table entry.
 *
 * Revision 1.5  2006/06/28 17:10:15  haraldkipp
 * Include more general header file for ARM.
 *
 * Revision 1.4  2006/03/16 19:04:48  haraldkipp
 * Adding a short delay before reading the status word makes it work with
 * compiler optimization. On receiver overflow interrupts the chip is
 * declared insane. The output routine will no more enter NutEventWait()
 * on insane chips.
 *
 * Revision 1.3  2006/03/02 19:51:16  haraldkipp
 * Replaced GCC specific inline specifications with their portable
 * counterparts.
 *
 * Revision 1.2  2006/01/23 17:33:14  haraldkipp
 * Possible memory alignment problem may start network interface too early.
 *
 * Revision 1.1  2005/10/24 08:49:05  haraldkipp
 * Initial check in.
 *
 */

#include <cfg/os.h>
#include <arch/arm.h>

#include <string.h>

#include <sys/atom.h>
#include <sys/heap.h>
#include <sys/thread.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/confnet.h>

#include <netinet/if_ether.h>
#include <net/ether.h>
#include <net/if_var.h>

#include <dev/irqreg.h>
#include <dev/dm9000e.h>

#ifdef NUTDEBUG
#include <stdio.h>
#endif

#ifndef NUT_THREAD_NICRXSTACK
#define NUT_THREAD_NICRXSTACK   768
#endif

/*
 * Determine ports, which had not been explicitely configured.
 */
#ifndef NIC_BASE_ADDR
#define NIC_BASE_ADDR   0x20000000
#endif

#ifndef NIC_DATA_ADDR
#define NIC_DATA_ADDR   (NIC_BASE_ADDR + 4)
#endif

#define INT0    0
#define INT1    1
#define INT2    2
#define INT3    3
#define INT4    4
#define INT5    5
#define INT6    6
#define INT7    7

#ifndef NIC_SIGNAL_IRQ
#define NIC_SIGNAL_IRQ  INT1
#endif

#ifdef NIC_RESET_BIT

#if (NIC_RESET_AVRPORT == AVRPORTB)
#define NIC_RESET_PORT   PORTB
#define NIC_RESET_DDR    DDRB

#elif (NIC_RESET_AVRPORT == AVRPORTD)
#define NIC_RESET_PORT   PORTD
#define NIC_RESET_DDR    DDRD

#elif (NIC_RESET_AVRPORT == AVRPORTE)
#define NIC_RESET_PORT   PORTE
#define NIC_RESET_DDR    DDRE

#elif (NIC_RESET_AVRPORT == AVRPORTF)
#define NIC_RESET_PORT   PORTF
#define NIC_RESET_DDR    DDRF

#endif                          /* NIC_RESET_AVRPORT */

#endif                          /* NIC_RESET_BIT */

/*
 * Determine interrupt settings.
 * DOES NOT WORK
 */
#if (NIC_SIGNAL_IRQ == INT0)
#define NIC_SIGNAL          sig_INTERRUPT0

#elif (NIC_SIGNAL_IRQ == INT2)
#define NIC_SIGNAL          sig_INTERRUPT2

#elif (NIC_SIGNAL_IRQ == INT3)
#define NIC_SIGNAL          sig_INTERRUPT3

#elif (NIC_SIGNAL_IRQ == INT4)
#define NIC_SIGNAL          sig_INTERRUPT4

#elif (NIC_SIGNAL_IRQ == INT5)
#define NIC_SIGNAL          sig_INTERRUPT5

#elif (NIC_SIGNAL_IRQ == INT6)
#define NIC_SIGNAL          sig_INTERRUPT6

#elif (NIC_SIGNAL_IRQ == INT7)
#define NIC_SIGNAL          sig_INTERRUPT7

#else
#define NIC_SIGNAL          sig_INTERRUPT1

#endif

/*!
 * \addtogroup xgDm9000eRegs
 */
/*@{*/

#define NIC_NCR     0x00        /* Network control register (0x00). */
#define NIC_NCR_LBM     0x06    /* Loopback mode. */
#define NIC_NCR_LBNORM  0x00    /* Normal mode. */
#define NIC_NCR_LBMAC   0x02    /* MAC loopback. */
#define NIC_NCR_LBPHY   0x04    /* PHY loopback. */
#define NIC_NCR_RST     0x01    /* Software reset, auto clear. */

#define NIC_NSR     0x01        /* Network status register (0x00). */
#define NIC_NSR_SPEED   0x80
#define NIC_NSR_LINKST  0x40
#define NIC_NSR_WAKEST  0x20
#define NIC_NSR_TX2END  0x08
#define NIC_NSR_TX1END  0x04
#define NIC_NSR_RXOV    0x02

#define NIC_TCR     0x02        /* TX control register (0x00). */
#define NIC_TCR_TXREQ    0x01   /* TX request */

#define NIC_TSR1    0x03        /* TX status register I (0x00). */

#define NIC_TSR2    0x04        /* TX status register II (0x00). */

#define NIC_RCR     0x05        /* RX control register (0x00). */
#define NIC_RCR_DIS_LONG 0x20   /* Discard long packets. */
#define NIC_RCR_DIS_CRC 0x10    /* Discard CRC error packets. */
#define NIC_RCR_PRMSC   0x02    /* Enable promiscuous mode. */
#define NIC_RCR_RXEN    0x01    /* Enable receiver. */

#define NIC_RSR     0x06        /* RX status register (0x00). */
#define NIC_RSR_ERRORS  0xBF    /* Error bit mask. */
#define NIC_RSR_RF      0x80    /* Runt frame. */
#define NIC_RSR_MF      0x40    /* Multicast frame. */
#define NIC_RSR_LCS     0x20    /* Late collision. */
#define NIC_RSR_RWTO    0x10    /* Receiver watchdog time out. */
#define NIC_RSR_PLE     0x08    /* Physical layer error. */
#define NIC_RSR_AE      0x04    /* Alignment error. */
#define NIC_RSR_CE      0x02    /* CRC error. */
#define NIC_RSR_FOE     0x01    /* FIFO overflow error. */

#define NIC_ROCR    0x07        /* Receive overflow counter register (0x00). */

#define NIC_BPTR    0x08        /* Back pressure threshold register (0x37). */

#define NIC_FCTR    0x09        /* Flow control threshold register (0x38). */

#define NIC_FCR     0x0A        /* RX flow control register (0x00). */

#define NIC_EPCR    0x0B        /* EEPROM and PHY control register. */

#define NIC_EPAR    0x0C        /* EEPROM and PHY address register. */

#define NIC_EPDRL   0x0D        /* EEPROM and PHY low byte data register. */

#define NIC_EPDRH   0x0E        /* EEPROM and PHY high byte data register. */

#define NIC_WCR     0x0F        /* Wake up control register (0x00). */

#define NIC_PAR     0x10        /* 6 byte physical address register. */

#define NIC_MAR     0x16        /* 8 byte multicast address register. */

#define NIC_GPCR    0x1E        /* General purpose control register (?). */

#define NIC_GPR     0x1F        /* General purpose register (?). */

#define NIC_TRPA    0x22        /* 2 byte TX SRAM read pointer address, low/high (0x0000). */

#define NIC_RWPA    0x24        /* 2 byte RX SRAM write pointer address, low/high (0x0000). */

#define NIC_VID     0x28        /* 2 byte vendor ID (0x0A46). */

#define NIC_PID     0x2A        /* 2 byte product ID (0x0900). */

#define NIC_CHIPR   0x2C        /* Chip revision (0x00). */

#define NIC_SMCR    0x2F        /* Special mode register (0x00). */

#define NIC_MRCMDX  0xF0        /* Memory data read command w/o increment (?). */

#define NIC_MRCMD   0xF2        /* Memory data read command with increment (?). */

#define NIC_MRR     0xF4        /* 2 byte memory data read register, low/high (?). */

#define NIC_MWCMDX  0xF6        /* Memory data write command register w/o increment (?). */

#define NIC_MWCMD   0xF8        /* Memory data write command register with increment (?). */

#define NIC_MWR     0xFA        /* Memory data write command register with increment (?). */

#define NIC_TXPL    0xFC        /* 2 byte TX packet length register. (?). */

#define NIC_ISR     0xFE        /* Interrupt status register (0x00). */
#define NIC_ISR_IOM     0xC0    /* I/O mode mask */
#define NIC_ISR_M16     0x00    /* 16-bit I/O mode */
#define NIC_ISR_M32     0x40    /* 32-bit I/O mode */
#define NIC_ISR_M8      0x80    /* 8-bit I/O mode */
#define NIC_ISR_ROOS    0x08    /* Receiver overflow counter interrupt. */
#define NIC_ISR_ROS     0x04    /* Receiver overflow interrupt. */
#define NIC_ISR_PTS     0x02    /* Transmitter interrupt. */
#define NIC_ISR_PRS     0x01    /* Receiver interrupt. */

#define NIC_IMR     0xFF        /* Interrupt mask register (0x00). */
#define NIC_IMR_PAR     0x80    /* Enable read/write pointer wrap around. */
#define NIC_IMR_ROOM    0x08    /* Enable receiver overflow counter interrupts. */
#define NIC_IMR_ROM     0x04    /* Enable receiver overflow interrupts. */
#define NIC_IMR_PTM     0x02    /* Enable transmitter interrupts. */
#define NIC_IMR_PRM     0x01    /* Enable receiver interrupts. */

#define NIC_PHY_BMCR    0x00    /* Basic mode control register. */

#define NIC_PHY_BMSR    0x01    /* Basic mode status register. */
#define NIC_PHY_BMSR_ANCOMPL    0x0020  /* Auto negotiation complete. */
#define NIC_PHY_BMSR_LINKSTAT   0x0004  /* Link status. */

#define NIC_PHY_ID1     0x02    /* PHY identifier register 1. */

#define NIC_PHY_ID2     0x03    /* PHY identifier register 2. */

#define NIC_PHY_ANAR    0x04    /* Auto negotiation advertisement register. */

#define NIC_PHY_ANLPAR  0x05    /* Auto negotiation link partner availability register. */

#define NIC_PHY_ANER    0x06    /* Auto negotiation expansion register. */

#define NIC_PHY_DSCR    0x10    /* Davicom specified configuration register. */

#define NIC_PHY_DSCSR   0x11    /* Davicom specified configuration and status register. */

#define NIC_PHY_10BTCSR 0x12    /* 10BASE-T configuration and status register. */

/*!
 * \brief Network interface controller information structure.
 */
struct _NICINFO {
#ifdef NUT_PERFMON
    u_long ni_rx_packets;       /*!< Number of packets received. */
    u_long ni_tx_packets;       /*!< Number of packets sent. */
    u_long ni_overruns;         /*!< Number of packet overruns. */
    u_long ni_rx_frame_errors;  /*!< Number of frame errors. */
    u_long ni_rx_crc_errors;    /*!< Number of CRC errors. */
    u_long ni_rx_missed_errors; /*!< Number of missed packets. */
#endif
    HANDLE volatile ni_rx_rdy;  /*!< Receiver event queue. */
    HANDLE volatile ni_tx_rdy;  /*!< Transmitter event queue. */
    HANDLE ni_mutex;            /*!< Access mutex semaphore. */
    volatile int ni_tx_queued;  /*!< Number of packets in transmission queue. */
    volatile int ni_tx_quelen;  /*!< Number of bytes in transmission queue not sent. */
    volatile int ni_insane;     /*!< Set by error detection. */
    int ni_iomode;              /*!< 8 or 16 bit access. 32 bit is not supported. */
};

/*!
 * \brief Network interface controller information type.
 */
typedef struct _NICINFO NICINFO;

/*@}*/

/*!
 * \addtogroup xgNicDm9000e
 */
/*@{*/


static INLINE void nic_outb(u_char reg, u_char val)
{
    outb(NIC_BASE_ADDR, reg);
    outb(NIC_DATA_ADDR, val);
}

static INLINE u_char nic_inb(u_short reg)
{
    outb(NIC_BASE_ADDR, reg);
    return inb(NIC_DATA_ADDR);
}

/*!
 * \brief Read contents of PHY register.
 *
 * \param reg PHY register number.
 *
 * \return Contents of the specified register.
 */
static u_short phy_inw(u_char reg)
{
    /* Select PHY register */
    nic_outb(NIC_EPAR, 0x40 | reg);

    /* PHY read command. */
    nic_outb(NIC_EPCR, 0x0C);
    NutDelay(1);
    nic_outb(NIC_EPCR, 0x00);

    /* Get data from PHY data register. */
    return ((u_short) nic_inb(NIC_EPDRH) << 8) | (u_short) nic_inb(NIC_EPDRL);
}

/*!
 * \brief Write value to PHY register.
 *
 * \note NIC interrupts must have been disabled before calling this routine.
 *
 * \param reg PHY register number.
 * \param val Value to write.
 */
static void phy_outw(u_char reg, u_short val)
{
    /* Select PHY register */
    nic_outb(NIC_EPAR, 0x40 | reg);

    /* Store value in PHY data register. */
    nic_outb(NIC_EPDRL, (u_char) val);
    nic_outb(NIC_EPDRH, (u_char) (val >> 8));

    /* PHY write command. */
    nic_outb(NIC_EPCR, 0x0A);
    NutDelay(1);
    nic_outb(NIC_EPCR, 0x00);
}

static int NicPhyInit(void)
{
    /* Restart auto negotiation. */
    phy_outw(NIC_PHY_ANAR, 0x01E1);
    phy_outw(NIC_PHY_BMCR, 0x1200);

    nic_outb(NIC_GPCR, 1);
    nic_outb(NIC_GPR, 0);

    return 0;
}

/*!
 * \brief Reset the Ethernet controller.
 *
 * \return 0 on success, -1 otherwise.
 */
static int NicReset(void)
{
    /* Hardware reset. */
#ifdef undef_NIC_RESET_BIT
    sbi(NIC_RESET_DDR, NIC_RESET_BIT);
    sbi(NIC_RESET_PORT, NIC_RESET_BIT);
    NutDelay(WAIT100);
    cbi(NIC_RESET_PORT, NIC_RESET_BIT);
    NutDelay(WAIT250);
    NutDelay(WAIT250);
#else
    /* Software reset. */
    nic_outb(NIC_NCR, NIC_NCR_RST | NIC_NCR_LBMAC);
    NutDelay(1);
    /* FIXME: Delay required. */
#endif

    return NicPhyInit();
}

/*
 * NIC interrupt entry.
 */
static void NicInterrupt(void *arg)
{
    u_char isr;
    NICINFO *ni = (NICINFO *) ((NUTDEVICE *) arg)->dev_dcb;

    /* Read interrupt status and disable interrupts. */
    isr = nic_inb(NIC_ISR);

    /* Receiver interrupt. */
    if (isr & NIC_ISR_PRS) {
        nic_outb(NIC_ISR, NIC_ISR_PRS);
        NutEventPostFromIrq(&ni->ni_rx_rdy);
    }

    /* Transmitter interrupt. */
    if (isr & NIC_ISR_PTS) {
        if (ni->ni_tx_queued) {
            if (ni->ni_tx_quelen) {
                /* Initiate transfer of a queued packet. */
                nic_outb(NIC_TXPL, (u_char) ni->ni_tx_quelen);
                nic_outb(NIC_TXPL + 1, (u_char) (ni->ni_tx_quelen >> 8));
                ni->ni_tx_quelen = 0;
                nic_outb(NIC_TCR, NIC_TCR_TXREQ);
            }
            ni->ni_tx_queued--;
        }
        nic_outb(NIC_ISR, NIC_ISR_PTS);
        NutEventPostFromIrq(&ni->ni_tx_rdy);
    }

    /* Receiver overflow interrupt. */
    if (isr & NIC_ISR_ROS) {
        nic_outb(NIC_ISR, NIC_ISR_ROS);
        ni->ni_insane = 1;
        NutEventPostFromIrq(&ni->ni_rx_rdy);
    }

    /* Receiver overflow counter interrupt. */
    if (isr & NIC_ISR_ROOS) {
        nic_outb(NIC_ISR, NIC_ISR_ROOS);
        NutEventPostFromIrq(&ni->ni_rx_rdy);
    }
}

/*!
 * \brief Write data block to the NIC.
 *
 * NIC interrupts must be disabled when calling this function.
 */
static void NicWrite8(u_char * buf, u_short len)
{
    while (len--) {
        outb(NIC_DATA_ADDR, *buf);
        buf++;
    }
}

/*!
 * \brief Write data block to the NIC.
 *
 * NIC interrupts must be disabled when calling this function.
 */
static void NicWrite16(u_char * buf, u_short len)
{
    u_short *wp = (u_short *) buf;

    len = (len + 1) / 2;
    while (len--) {
        outw(NIC_DATA_ADDR, *wp);
        wp++;
    }
}

/*!
 * \brief Read data block from the NIC.
 *
 * NIC interrupts must be disabled when calling this function.
 */
static void NicRead8(u_char * buf, u_short len)
{
    while (len--) {
        *buf++ = inb(NIC_DATA_ADDR);
    }
}

/*!
 * \brief Read data block from the NIC.
 *
 * NIC interrupts must be disabled when calling this function.
 */
static void NicRead16(u_char * buf, u_short len)
{
    u_short *wp = (u_short *) buf;

    len = (len + 1) / 2;
    while (len--) {
        *wp++ = inw(NIC_DATA_ADDR);
    }
}

/*!
 * \brief Fetch the next packet out of the receive ring buffer.
 *
 * \return Pointer to an allocated ::NETBUF. If there is no
 *         no data available, then the function returns a
 *         null pointer. If the NIC's buffer seems to be
 *         corrupted, a pointer to 0xFFFF is returned.
 */
static int NicGetPacket(NICINFO * ni, NETBUF ** nbp)
{
    int rc = -1;
    u_short fsw;
    u_short fbc;

    *nbp = NULL;

    /* Disable NIC interrupts. */
    NutIrqDisable(&NIC_SIGNAL);

    /* 
     * Read the status word w/o auto increment. If zero, no packet is 
     * available. Otherwise it should be set to one. Any other value 
     * indicates a weird chip crying for reset.
     */
    nic_inb(NIC_MRCMDX);
    fsw = inb(NIC_DATA_ADDR);
    if (fsw > 1) {
        ni->ni_insane = 1;
    } else if (fsw) {
        /* Now read status word and byte count with auto increment. */
        outb(NIC_BASE_ADDR, NIC_MRCMD);
        if (ni->ni_iomode == NIC_ISR_M16) {
            fsw = inw(NIC_DATA_ADDR);
            _NOP(); _NOP(); _NOP(); _NOP();
            fbc = inw(NIC_DATA_ADDR);
        } else {
            fsw = inb(NIC_DATA_ADDR) + ((u_short) inb(NIC_DATA_ADDR) << 8);
            _NOP(); _NOP(); _NOP(); _NOP();
            fbc = inb(NIC_DATA_ADDR) + ((u_short) inb(NIC_DATA_ADDR) << 8);
        }

        /*
         * Receiving long packets is unexpected, because we disabled 
         * this during initialization. Let's declare the chip insane.
         * Short packets will be handled by the caller.
         */
        if (fbc > 1536) {
            ni->ni_insane = 1;
        } else {
            /*
             * The high byte of the status word contains a copy of the 
             * receiver status register.
             */
            fsw >>= 8;
            fsw &= NIC_RSR_ERRORS;
#ifdef NUT_PERMON
            /* Update statistics. */
            if (fsw) {
                if (RxStatus & NIC_RSR_CE) {
                    ni->ni_crc_errors++;
                } else if (RxStatus & NIC_RSR_FOE) {
                    ni->ni_overruns++;
                } else {
                    ni->ni_rx_missed_errors++;
                }
            } else {
                ni->ni_rx_packets++;
            }
#endif
            /* 
             * If we got an error packet or failed to allocated the
             * buffer, then silently discard the packet.
             */
            if (fsw || (*nbp = NutNetBufAlloc(0, NBAF_DATALINK, fbc - 4)) == NULL) {
                if (ni->ni_iomode == NIC_ISR_M16) {
                    fbc = (fbc + 1) / 2;
                    while (fbc--) {
                        fsw = inw(NIC_DATA_ADDR);
                    }
                } else {
                    while (fbc--) {
                        fsw = inb(NIC_DATA_ADDR);
                    }
                }
            } else {
                if (ni->ni_iomode == NIC_ISR_M16) {
                    /* Read packet data from 16 bit bus. */
                    NicRead16((*nbp)->nb_dl.vp, (*nbp)->nb_dl.sz);
                    /* Read packet CRC. */
                    fsw = inw(NIC_DATA_ADDR);
                    fsw = inw(NIC_DATA_ADDR);
                } else {
                    /* Read packet data from 8 bit bus. */
                    NicRead8((*nbp)->nb_dl.vp, (*nbp)->nb_dl.sz);
                    /* Read packet CRC. */
                    fsw = inb(NIC_DATA_ADDR);
                    fsw = inb(NIC_DATA_ADDR);
                    fsw = inb(NIC_DATA_ADDR);
                    fsw = inb(NIC_DATA_ADDR);
                }
                /* Return success. */
                rc = 0;
            }
        }
    }

    /* Enable NIC interrupts if the chip is sane. */
    if (ni->ni_insane == 0) {
        NutIrqEnable(&NIC_SIGNAL);
    }
    return rc;
}

/*!
 * \brief Load a packet into the nic's transmit ring buffer.
 *
 * \param nb Network buffer structure containing the packet to be sent.
 *           The structure must have been allocated by a previous
 *           call NutNetBufAlloc(). This routine will automatically
 *           release the buffer in case of an error.
 *
 * \return 0 on success, -1 in case of any errors. Errors
 *         will automatically release the network buffer 
 *         structure.
 */
static int NicPutPacket(NICINFO * ni, NETBUF * nb)
{
    int rc = -1;
    u_short sz;

    /*
     * Calculate the number of bytes to be send. Do not send packets 
     * larger than the Ethernet maximum transfer unit. The MTU
     * consist of 1500 data bytes plus the 14 byte Ethernet header
     * plus 4 bytes CRC. We check the data bytes only.
     */
    if ((sz = nb->nb_nw.sz + nb->nb_tp.sz + nb->nb_ap.sz) > ETHERMTU) {
        return -1;
    }
    sz += nb->nb_dl.sz;
    if (sz & 1) {
        sz++;
    }

    /* Disable interrupts. */
    NutIrqDisable(&NIC_SIGNAL);

    /* TODO: Check for link. */
    if (ni->ni_insane == 0) {
        /* Enable data write. */
        outb(NIC_BASE_ADDR, NIC_MWCMD);

        /* Transfer the Ethernet frame. */
        if (ni->ni_iomode == NIC_ISR_M16) {
            NicWrite16(nb->nb_dl.vp, nb->nb_dl.sz);
            NicWrite16(nb->nb_nw.vp, nb->nb_nw.sz);
            NicWrite16(nb->nb_tp.vp, nb->nb_tp.sz);
            NicWrite16(nb->nb_ap.vp, nb->nb_ap.sz);
        } else {
            NicWrite8(nb->nb_dl.vp, nb->nb_dl.sz);
            NicWrite8(nb->nb_nw.vp, nb->nb_nw.sz);
            NicWrite8(nb->nb_tp.vp, nb->nb_tp.sz);
            NicWrite8(nb->nb_ap.vp, nb->nb_ap.sz);
        }

        /* If no packet is queued, start the transmission. */
        if (ni->ni_tx_queued == 0) {
            nic_outb(NIC_TXPL, (u_char) sz);
            nic_outb(NIC_TXPL + 1, (u_char) (sz >> 8));
            nic_outb(NIC_TCR, NIC_TCR_TXREQ);
        }
        /* ...otherwise mark this packet queued. */
        else {
            ni->ni_tx_quelen = sz;
        }
        ni->ni_tx_queued++;
        rc = 0;
#ifdef NUT_PERFMON
        ni->ni_tx_packets++;
#endif
    }

    /* Enable interrupts. */
    NutIrqEnable(&NIC_SIGNAL);

    /* If the controller buffer is filled with two packets, then
       wait for the first being sent out. */
    if (rc == 0 && ni->ni_tx_queued > 1) {
        NutEventWait(&ni->ni_tx_rdy, 500);
    }
    return rc;
}

/*!
 * \brief Fire up the network interface.
 *
 * NIC interrupts must be disabled when calling this function.
 *
 * \param mac Six byte unique MAC address.
 */
static int NicStart(CONST u_char * mac)
{
    int i;
    int link_wait = 20;

    /* Power up the PHY. */
    nic_outb(NIC_GPR, 0);
    NutDelay(5);

    /* Software reset with MAC loopback. */
    nic_outb(NIC_NCR, NIC_NCR_RST | NIC_NCR_LBMAC);
    NutDelay(5);
    nic_outb(NIC_NCR, NIC_NCR_RST | NIC_NCR_LBMAC);
    NutDelay(5);

    /* 
     * PHY power down followed by PHY power up. This should activate 
     * the auto sense link.
     */
    nic_outb(NIC_GPR, 1);
    nic_outb(NIC_GPR, 0);

    /* Set MAC address. */
    for (i = 0; i < 6; i++) {
        nic_outb(NIC_PAR + i, mac[i]);
    }

    /* Enable broadcast receive. */
    for (i = 0; i < 7; i++) {
        nic_outb(NIC_MAR + i, 0);
    }
    nic_outb(NIC_MAR + 7, 0x80);

    /* Clear interrupts. */
    nic_outb(NIC_ISR, NIC_ISR_ROOS | NIC_ISR_ROS | NIC_ISR_PTS | NIC_ISR_PRS);

    /* Enable receiver. */
    nic_outb(NIC_RCR, NIC_RCR_DIS_LONG | NIC_RCR_DIS_CRC | NIC_RCR_RXEN);

    /* Wait for link. */
    for (link_wait = 20;; link_wait--) {
        if (phy_inw(NIC_PHY_BMSR) & NIC_PHY_BMSR_ANCOMPL) {
            break;
        }
        if (link_wait == 0) {
            return -1;
        }
        NutSleep(200);
    }

    /* Enable interrupts. */
    nic_outb(NIC_IMR, NIC_IMR_PAR | NIC_IMR_PTM | NIC_IMR_PRM);

    return 0;
}

/*! \fn NicRxLanc(void *arg)
 * \brief NIC receiver thread.
 *
 */
THREAD(NicRxLanc, arg)
{
    NUTDEVICE *dev;
    IFNET *ifn;
    NICINFO *ni;
    NETBUF *nb;

    dev = arg;
    ifn = (IFNET *) dev->dev_icb;
    ni = (NICINFO *) dev->dev_dcb;

    /*
     * This is a temporary hack. Due to a change in initialization,
     * we may not have got a MAC address yet. Wait until one has been
     * set.
     */
    for (;;) {
        int i;

        for (i = 0; i < sizeof(ifn->if_mac); i++) {
            if (ifn->if_mac[i] && ifn->if_mac[i] != 0xFF) {
                break;
            }
        }
        if (i < sizeof(ifn->if_mac)) {
            break;
        }
        NutSleep(63);
    }

    /*
     * Do not continue unless we managed to start the NIC. We are
     * trapped here if the Ethernet link cannot be established.
     * This happens, for example, if no Ethernet cable is plugged
     * in.
     */
    while (NicStart(ifn->if_mac)) {
        NutSleep(1000);
    }

    /* Initialize the access mutex. */
    NutEventPost(&ni->ni_mutex);

    /* Run at high priority. */
    NutThreadSetPriority(9);

    /* Enable interrupts for P10. */
    outr(PIO_PDR, _BV(10));
    NutIrqEnable(&NIC_SIGNAL);

    for (;;) {
        /*
         * Wait for the arrival of new packets or poll the receiver 
         * every two seconds.
         */
        NutEventWait(&ni->ni_rx_rdy, 2000);

        /*
         * Fetch all packets from the NIC's internal buffer and pass 
         * them to the registered handler.
         */
        while (NicGetPacket(ni, &nb) == 0) {

            /* Discard short packets. */
            if (nb->nb_dl.sz < 60) {
                NutNetBufFree(nb);
            } else {
                (*ifn->if_recv) (dev, nb);
            }
        }

        /* We got a weird chip, try to restart it. */
        while (ni->ni_insane) {
            if (NicStart(ifn->if_mac) == 0) {
                ni->ni_insane = 0;
                ni->ni_tx_queued = 0;
                ni->ni_tx_quelen = 0;
                NutIrqEnable(&NIC_SIGNAL);
            } else {
                NutSleep(1000);
            }
        }
    }
}

/*!
 * \brief Send Ethernet packet.
 *
 * \param dev Identifies the device to use.
 * \param nb  Network buffer structure containing the packet to be sent.
 *            The structure must have been allocated by a previous
 *            call NutNetBufAlloc().
 *
 * \return 0 on success, -1 in case of any errors.
 */
int DmOutput(NUTDEVICE * dev, NETBUF * nb)
{
    static u_long mx_wait = 5000;
    int rc = -1;
    NICINFO *ni = (NICINFO *) dev->dev_dcb;

    /*
     * After initialization we are waiting for a long time to give
     * the PHY a chance to establish an Ethernet link.
     */
    while (rc) {
        if (ni->ni_insane) {
            break;
        }
        if (NutEventWait(&ni->ni_mutex, mx_wait)) {
            break;
        }

        /* Check for packet queue space. */
        if (ni->ni_tx_queued > 1) {
            if (NutEventWait(&ni->ni_tx_rdy, 500)) {
                /* No queue space. Release the lock and give up. */
                NutEventPost(&ni->ni_mutex);
                break;
            }
        } else if (NicPutPacket(ni, nb) == 0) {
            /* Ethernet works. Set a long waiting time in case we
               temporarly lose the link next time. */
            rc = 0;
            mx_wait = 5000;
        }
        NutEventPost(&ni->ni_mutex);
    }
    /*
     * Probably no Ethernet link. Significantly reduce the waiting
     * time, so following transmission will soon return an error.
     */
    if (rc) {
        mx_wait = 500;
    }
    return rc;
}

/*!
 * \brief Initialize Ethernet hardware.
 *
 * Resets the LAN91C111 Ethernet controller, initializes all required 
 * hardware registers and starts a background thread for incoming 
 * Ethernet traffic.
 *
 * Applications should do not directly call this function. It is 
 * automatically executed during during device registration by 
 * NutRegisterDevice().
 *
 * If the network configuration hasn't been set by the application
 * before registering the specified device, this function will
 * call NutNetLoadConfig() to get the MAC address.
 *
 * \param dev Identifies the device to initialize.
 */
int DmInit(NUTDEVICE * dev)
{
    u_long id;
    NICINFO *ni = (NICINFO *) dev->dev_dcb;

    /* Probe chip by verifying the identifier registers. */
    id = (u_long) nic_inb(NIC_VID);
    id |= (u_long) nic_inb(NIC_VID + 1) << 8;
    id |= (u_long) nic_inb(NIC_PID) << 16;
    id |= (u_long) nic_inb(NIC_PID + 1) << 24;
    if (id != 0x90000A46) {
        return -1;
    }

    /* Reset chip. */
    if (NicReset()) {
        return -1;
    }

    /* Clear NICINFO structure. */
    memset(ni, 0, sizeof(NICINFO));

    /* Determine bus mode. We do not support 32 bit access. */
    ni->ni_iomode = nic_inb(NIC_ISR) & NIC_ISR_IOM;
    if (ni->ni_iomode == NIC_ISR_M32) {
        return -1;
    }

    /* Register interrupt handler. */
    if (NutRegisterIrqHandler(&NIC_SIGNAL, NicInterrupt, dev)) {
        return -1;
    }

    /* Start the receiver thread. */
    if (NutThreadCreate("rxi1", NicRxLanc, dev, NUT_THREAD_NICRXSTACK) == NULL) {
        return -1;
    }
    return 0;
}

static NICINFO dcb_eth0;

/*!
 * \brief Network interface information structure.
 *
 * Used to call.
 */
static IFNET ifn_eth0 = {
    IFT_ETHER,                  /*!< \brief Interface type, if_type. */
    {0, 0, 0, 0, 0, 0},         /*!< \brief Hardware net address, if_mac. */
    0,                          /*!< \brief IP address, if_local_ip. */
    0,                          /*!< \brief Remote IP address for point to point, if_remote_ip. */
    0,                          /*!< \brief IP network mask, if_mask. */
    ETHERMTU,                   /*!< \brief Maximum size of a transmission unit, if_mtu. */
    0,                          /*!< \brief Packet identifier, if_pkt_id. */
    0,                          /*!< \brief Linked list of arp entries, arpTable. */
    0,                          /*!< \brief Linked list of multicast address entries, if_mcast. */
    NutEtherInput,              /*!< \brief Routine to pass received data to, if_recv(). */
    DmOutput,                   /*!< \brief Driver output routine, if_send(). */
    NutEtherOutput              /*!< \brief Media output routine, if_output(). */
};

/*!
 * \brief Device information structure.
 *
 * A pointer to this structure must be passed to NutRegisterDevice() 
 * to bind this Ethernet device driver to the Nut/OS kernel.
 * An application may then call NutNetIfConfig() with the name \em eth0 
 * of this driver to initialize the network interface.
 * 
 */
NUTDEVICE devDM9000E = {
    0,                          /*!< \brief Pointer to next device. */
    {'e', 't', 'h', '0', 0, 0, 0, 0, 0},        /*!< \brief Unique device name. */
    IFTYP_NET,                  /*!< \brief Type of device. */
    0,                          /*!< \brief Base address. */
    0,                          /*!< \brief First interrupt number. */
    &ifn_eth0,                  /*!< \brief Interface control block. */
    &dcb_eth0,                  /*!< \brief Driver control block. */
    DmInit,                     /*!< \brief Driver initialization routine. */
    0,                          /*!< \brief Driver specific control function. */
    0,                          /*!< \brief Read from device. */
    0,                          /*!< \brief Write to device. */
#ifdef __HARVARD_ARCH__
    0,                          /*!< \brief Write from program space data to device. */
#endif
    0,                          /*!< \brief Open a device or file. */
    0,                          /*!< \brief Close a device or file. */
    0                           /*!< \brief Request file size. */
};

/*@}*/
