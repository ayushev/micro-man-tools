/*
 * MICRO-MAN-TOOLS: A set of tools for embedded system development 
 * Copyright (C) 2016 Andreas Walz
 *
 * Author: Andreas Walz (andreas.walz@hs-offenburg.de)
 *
 * This file is part of MICRO-MAN-TOOLS.
 *
 * THE-MAN-TOOLS are free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * THE-MAN-TOOLS are distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with THE-MAN-TOOLS; if not, see <http://www.gnu.org/licenses/>
 * or write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "ringbuffer.h"
#include <string.h>


/*
 * ___________________________________________________________________________
 */
void ringbuffer_init(ringbuffer_t* ringbuf, uint8_t* mem, size_t memlen) {

    if (ringbuf != 0 && mem != 0) {
        ringbuf->buffer = mem;
        ringbuf->size = memlen;
        ringbuf->len = 0;
        ringbuf->iw = 0;
        ringbuf->ir = 0;
    }
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_write(ringbuffer_t* ringbuf, uint8_t* data, size_t len) {

    /* the number of bytes actually written to ring buffer */
    size_t lenWritten = 0;

    if (ringbuf != 0 && data != 0) {

        /* don't write more than there is space (assuming len never exceeds size) */
        size_t space = (size_t)(ringbuf->size - ringbuf->len);
        if (len > space) {
            len = space;
        }
        ringbuf->len += len;
        lenWritten = len;

        size_t tmpLen = ringbuf->size - ringbuf->iw;
        if (len <= tmpLen) {

            /* copy data until end of buffer (assuming iw never exceeds size) */
            memcpy(ringbuf->buffer + ringbuf->iw, data, len);

            /* advance write pointer */
            ringbuf->iw += len;
            if (len == tmpLen) {
                /* here: ringbuf->iw == ringbuf->size, so wrap write pointer */
                ringbuf->iw = 0;
            }

        } else {

            /* copy data until end of buffer (assuming iw never exceeds size) */
            memcpy(ringbuf->buffer + ringbuf->iw, data, tmpLen);

            /* write pointer implicitly wrapped */
            ringbuf->iw = len - tmpLen;

            /* copy remaining data to beginning of buffer */
            memcpy(ringbuf->buffer, data + tmpLen, ringbuf->iw);

        }
    }

    return lenWritten;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_read(ringbuffer_t* ringbuf, uint8_t* data, size_t len) {

    /* the number of bytes actually read from ring buffer */
    size_t lenRead = 0;

    if (ringbuf != 0 && data != 0) {

        /* don't read more than there is data */
        if (len > ringbuf->len) {
            len = ringbuf->len;
        }
        ringbuf->len -= len;
        lenRead = len;

        size_t tmpLen = ringbuf->size - ringbuf->ir;
        if (len <= tmpLen) {

            /* copy data until end of buffer (assuming ir never exceeds size) */
            memcpy(data, ringbuf->buffer + ringbuf->ir, len);

            /* advance read pointer */
            ringbuf->ir += len;
            if (len == tmpLen) {
                /* here: ringbuf->ir == ringbuf->size, so wrap read pointer */
                ringbuf->ir = 0;
            }

        } else {

            /* copy data until end of buffer (assuming iw never exceeds size) */
            memcpy(data, ringbuf->buffer + ringbuf->ir, tmpLen);

            /* read pointer implicitly wrapped */
            ringbuf->ir = len - tmpLen;

            /* copy remaining data to beginning of buffer */
            memcpy(data + tmpLen, ringbuf->buffer, ringbuf->ir);

        }
    }

    return lenRead;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_sniff(ringbuffer_t* ringbuf, uint8_t* data, size_t len) {

    /* the number of bytes actually sniffed from ring buffer */
    size_t lenSniffed = 0;

    if (ringbuf != 0 && data != 0) {

        /* don't read more than there is data */
        if (len > ringbuf->len) {
            len = ringbuf->len;
        }
        lenSniffed = len;

        size_t tmpLen = ringbuf->size - ringbuf->ir;
        if (len <= tmpLen) {

            /* copy data until end of buffer (assuming ir never exceeds size) */
            memcpy(data, ringbuf->buffer + ringbuf->ir, len);

        } else {

            /* copy data until end of buffer (assuming iw never exceeds size) */
            memcpy(data, ringbuf->buffer + ringbuf->ir, tmpLen);

            /* copy remaining data to beginning of buffer */
            memcpy(data + tmpLen, ringbuf->buffer, len - tmpLen);

        }
    }

    return lenSniffed;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_discard(ringbuffer_t* ringbuf, size_t len) {

    /* the number of bytes actually discarded from ring buffer */
    size_t lenDiscarded = 0;

    if (ringbuf != 0) {

        /* don't discard more than there is data */
        if (len > ringbuf->len) {
            len = ringbuf->len;
        }
        ringbuf->len -= len;
        lenDiscarded = len;

        size_t tmpLen = ringbuf->size - ringbuf->ir;
        if (len <= tmpLen) {

            /* advance read pointer */
            ringbuf->ir += len;
            if (len == tmpLen) {
                /* here: ringbuf->ir == ringbuf->size, so wrap read pointer */
                ringbuf->ir = 0;
            }

        } else {

            /* read pointer implicitly wrapped */
            ringbuf->ir = len - tmpLen;

        }
    }

    return lenDiscarded;
}



