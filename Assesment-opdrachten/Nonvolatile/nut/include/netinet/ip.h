#ifndef _NETINET_IP_H_
#define _NETINET_IP_H_

/*
 * Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
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
 * -
 * Portions Copyright (C) 2000 David J. Hudson <dave@humbug.demon.co.uk>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "copying-gpl.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "copying-liquorice.txt" for details.
 * -
 * Portions Copyright (c) 1983, 1993 by
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * $Log: ip.h,v $
 * Revision 1.5  2005/08/02 17:46:49  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.4  2005/04/05 17:38:45  haraldkipp
 * ARM7 memory alignment bugs fixed.
 *
 * Revision 1.3  2004/12/16 18:47:02  haraldkipp
 * Added Damian Slee's IP filter function.
 *
 * Revision 1.2  2004/03/16 16:48:28  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:14  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.9  2003/02/04 18:00:45  harald
 * Version 3 released
 *
 * Revision 1.8  2002/06/26 17:29:21  harald
 * First pre-release with 2.4 stack
 *
 */

#include <net/if_arp.h>

/*!
 * \file netinet/ip.h
 * \brief Definitions for internet protocol version 4.
 *
 * RFC 791.
 */

/*!
 * \addtogroup xgIP
 */
/*@{*/

#define IPVERSION   4           /*!< \brief IP protocol version. */

#define IP_DF       0x4000      /*!< \brief Don't fragment flag. */
#define IP_MF       0x2000      /*!< \brief More fragments flag. */
#define IP_OFFMASK  0x1fff      /*!< \brief Mask for fragmenting bits. */

/*!
 * \brief Internet header type.
 */
typedef struct __attribute__ ((packed)) ip IPHDR;

/*!
 * \brief Structure of an internet header.
 */
struct __attribute__ ((packed)) ip {
#ifndef __BIG_ENDIAN__
#ifdef __IMAGECRAFT__
    unsigned ip_hl:4,           /*!< \brief Header length. */
     ip_v:4;                    /*!< \brief Version. */
#else
    u_char ip_hl:4,             /*!< \brief Header length. */
     ip_v:4;                    /*!< \brief Version. */
#endif
#else                           /* #ifndef __BIG_ENDIAN__ */
    u_char ip_v:4,              /*!< \brief Version. */
     ip_hl:4;                   /*!< \brief Header length. */
#endif                          /* #ifndef __BIG_ENDIAN__ */
    u_char ip_tos;              /*!< \brief Type of service. */
    short ip_len;               /*!< \brief Total length. */
    u_short ip_id;              /*!< \brief Identification. */
    short ip_off;               /*!< \brief Fragment offset field. */
    u_char ip_ttl;              /*!< \brief Time to live. */
    u_char ip_p;                /*!< \brief Protocol. */
    u_short ip_sum;             /*!< \brief Checksum. */
    u_long ip_src;              /*!< \brief Source IP address. */
    u_long ip_dst;              /*!< \brief Destination IP address. */
};

#define IPOPT_EOL       0       /*!< \brief End of option list. */
#define IPOPT_NOP       1       /*!< \brief No operation. */

#define IPOPT_RR        7       /*!< \brief Record packet route. */
#define IPOPT_TS        68      /*!< \brief Timestamp. */
#define IPOPT_SECURITY  130     /*!< \brief Provide s,c,h,tcc. */
#define IPOPT_LSRR      131     /*!< \brief Loose source route. */
#define IPOPT_SATID     136     /*!< \brief Satnet id. */
#define IPOPT_SSRR      137     /*!< \brief Strict source route. */

#define IPOPT_OPTVAL    0       /*!< \brief Option identifier offset. */
#define IPOPT_OLEN      1       /*!< \brief Option length offset. */
#define IPOPT_OFFSET    2       /*!< \brief Offset within option. */
#define IPOPT_MINOFF    4       /*!< \brief Minimum offset within option. */

#define MAXTTL      255         /*!< \brief Maximum time to live (seconds). */
#define IPDEFTTL    64          /*!< \brief Default time to live. */
#define IPFRAGTTL   60          /*!< \brief Time to live for fragments. */
#define IPTTLDEC    1           /*!< \brief Subtracted from time to live when forwarding. */

/*@}*/

__BEGIN_DECLS
/*
 * API declarations.
 */
#include <dev/netbuf.h>
extern int NutIpOutput(u_char proto, u_long dest, NETBUF * nb);

/*
 * Kernel declarations.
 */
#include <net/if_var.h>
extern void NutIpInput(NUTDEVICE * dev, NETBUF * nb);

/*
 * Ip Filter declarations.
 */
typedef int (*NutIpFilterFunc) (u_long);
extern void NutIpSetInputFilter(NutIpFilterFunc callbackFunc);

__END_DECLS
/* */
#endif
