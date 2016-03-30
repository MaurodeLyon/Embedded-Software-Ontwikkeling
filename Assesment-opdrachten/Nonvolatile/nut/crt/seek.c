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
 * $Log: seek.c,v $
 * Revision 1.1  2005/08/05 11:17:53  olereinhardt
 * Added support for _seek, _tell, fseek, ftell functions
 *
 */

#include "nut_io.h"

#include <sys/device.h>
#include <fs/fs.h>
#include <errno.h>

/*!
 * \addtogroup xgCrtLowio
 */
/*@{*/
 
/*!
 * \brief Move read/write position of a file.
 *
 * \param fd     Descriptor of a previously opened file, device or
 *               connected socket.
 * \param offset Number of bytes from initial position.
 * \param origin Initial position to seek from.
 *
 * \return 0 if no error occured -1 on error. errno is set
 *
 */
 
int _seek(int fd, long offset, int origin)
{
    NUTFILE *fp = (NUTFILE *) ((uptr_t) fd);
    NUTDEVICE *dev = fp->nf_dev;
    IOCTL_ARG3 conf;
    
    conf.arg1 = (void*) fp;
    conf.arg2 = (void*) &offset;
    conf.arg3 = (void*) origin;
    
    if (dev != 0) {
        if ((*dev->dev_ioctl) (dev, FS_FILE_SEEK, &conf)) {
            return 0;
        } else {
            errno = EINVAL;
            return -1;
        }
    } else {
        errno = EINVAL;
        return -1;
    }
}

/*@}*/
