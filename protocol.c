/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the 
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "fastboot.h"

static char ERROR[128];

char *fb_get_error(void)
{
    return ERROR;
}

static int save_to_file(int fd, void *data, unsigned long sz)
{
    if (fd < 0 || sz < 0)
        return -1;

    if (write(fd, data, sz) != sz) {
        fprintf(stderr, "write failed.\n");
        return -1;
    }

    return 0;
}

static int check_response(usb_handle *usb, unsigned size, 
                          unsigned data_okay, char *response)
{
    /* FIXME: not clear why 64 doesn't work */
#define SIZE 512
    unsigned char status[SIZE + 1];
    int r;

    for(;;) {
        r = usb_read(usb, status, SIZE);
        if(r < 0) {
            sprintf(ERROR, "status read failed (%s)", strerror(errno));
            usb_close(usb);
            return -1;
        }
        status[r] = 0;

        if(r < 4) {
            sprintf(ERROR, "status malformed (%d bytes)", r);
            usb_close(usb);
            return -1;
        }

        if(!memcmp(status, "INFO", 4)) {
            printf("%s", status + 4);
            continue;
        }

        if(!memcmp(status, "OKAY", 4)) {
            if(response) {
                strcpy(response, (char*) status + 4);
            }
            return 0;
        }

        if(!memcmp(status, "FAIL", 4)) {
            if(r > 4) {
                sprintf(ERROR, "remote: %s", status + 4);
            } else {
                strcpy(ERROR, "remote failure");
            }
            return -1;
        }

        if(!memcmp(status, "DATA", 4) && data_okay){
            unsigned dsize = strtoul((char*) status + 4, 0, 16);
            if(dsize > size) {
                strcpy(ERROR, "data size too large");
                usb_close(usb);
                return -1;
            }
            return dsize;
        }

        if (!memcmp(status, "FILE", 4)) {
            char size[9];
            unsigned dsize;
            unsigned left;
            void *data;
            char response[64];

            if (snprintf(size, sizeof(size), "%s", (char *)status + 4) < 0)
                goto err;
            size[8] = 0;

            dsize = strtoul(size, 0, 16);
            if (dsize <= 0)
                goto err;

            if (dsize == r - 12) {
                /* already read in status */
                if (save_to_file(fd_pull, status + 12, r - 12))
                    goto err;
            } else {
                left = dsize - (r - 12);
                data = malloc(dsize);
                if (!data)
                    goto err;

                memcpy(data, status + 12, r - 12);
                if (usb_read(usb, data + r - 12, dsize) != left)
                    goto err;

                if (save_to_file(fd_pull, data, dsize))
                    goto err;
            }

            /* send bytes written */
            snprintf(response, sizeof(response), "FILE%08x", dsize);
            if (usb_write(usb, response, 12) != 12)
                goto usb_err;

            continue;
err:
            snprintf(response, sizeof(response), "FILE%08x", 0);
            if (usb_write(usb, response, 12) == 12)
                continue;
usb_err:
            usb_close(usb);
            return -1;
        }

        strcpy(ERROR,"unknown status code");
        usb_close(usb);
        break;
    }

    return -1;
}

static int _command_send(usb_handle *usb, const char *cmd,
                         const void *data, unsigned size,
                         char *response)
{
    int cmdsize = strlen(cmd);
    int r;
    
    if(response) {
        response[0] = 0;
    }

    if(cmdsize > 64) {
        sprintf(ERROR,"command too large");
        return -1;
    }

    if(usb_write(usb, cmd, cmdsize) != cmdsize) {
        sprintf(ERROR,"command write failed (%s)", strerror(errno));
        usb_close(usb);
        return -1;
    }

    if(data == 0) {
        return check_response(usb, size, 0, response);
    }

    r = check_response(usb, size, 1, 0);
    if(r < 0) {
        return -1;
    }
    size = r;

    if(size) {
        r = usb_write(usb, data, size);
        if(r < 0) {
            sprintf(ERROR, "data transfer failure (%s)", strerror(errno));
            usb_close(usb);
            return -1;
        }
        if(r != ((int) size)) {
            sprintf(ERROR, "data transfer failure (short transfer)");
            usb_close(usb);
            return -1;
        }
    }
    
    r = check_response(usb, 0, 0, 0);
    if(r < 0) {
        return -1;
    } else {
        return size;
    }
}

int fb_command(usb_handle *usb, const char *cmd)
{
    return _command_send(usb, cmd, 0, 0, 0);
}

int fb_command_response(usb_handle *usb, const char *cmd, char *response)
{
    return _command_send(usb, cmd, 0, 0, response);
}

int fb_download_data(usb_handle *usb, const void *data, unsigned size)
{
    char cmd[64];
    int r;
    
    sprintf(cmd, "download:%08x", size);
    r = _command_send(usb, cmd, data, size, 0);
    
    if(r < 0) {
        return -1;
    } else {
        return 0;
    }
}

int fb_stream_flash(usb_handle *usb, const char *cmd,
        const void *data, unsigned size)
{
    int r;
    r = _command_send(usb, cmd, data, size, 0);
    if (r < 0) {
        return -1;
    } else {
        return 0;
    }
}
