/*
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
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
 * $Log: malloc.c,v $
 * Revision 1.5  2006/06/18 16:32:18  haraldkipp
 * Set errno on allocation failure.
 *
 * Revision 1.4  2005/08/02 17:46:47  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.3  2004/09/08 10:24:26  haraldkipp
 * RAMSTART is too platform dependant
 *
 * Revision 1.2  2003/08/05 20:01:55  haraldkipp
 * Typing errors corrected
 *
 * Revision 1.1.1.1  2003/05/09 14:40:30  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.1  2003/02/04 17:49:07  harald
 * *** empty log message ***
 *
 */

#include <sys/heap.h>

#include <stdlib.h>
#include <errno.h>

/*!
 * \addtogroup xgHeap
 */
/*@{*/

/*!
 * \brief Allocate a block from heap memory.
 *
 * This function simply calls NutHeapAlloc(). It overrides the function
 * of the runtime library, when the application is linked with nutcrt or
 * nutcrtf.
 *
 * \param len Size of the requested memory block.
 *
 * \return Pointer to the allocated memory block if the
 *         function is successful or NULL if the requested
 *         amount of memory is not available.
 */
void *malloc(size_t len)
{
    void *p;

    if ((p = NutHeapAlloc(len)) == NULL) {
        errno = ENOMEM;
    }
    return p;
}

/*!
 * \brief Return a block to heap memory.
 *
 * This function simply calls NutHeapFree(). It overrides the function
 * of the runtime library, when the application is linked with nutcrt or
 * nutcrtf.
 *
 * \param p Points to a memory block previously allocated
 *          through a call to malloc().
 */
void free(void *p)
{
    NutHeapFree(p);
}

/*@}*/
