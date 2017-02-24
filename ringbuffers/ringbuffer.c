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
void ringbuffer_init(ringbuffer_t* rb, uint8_t* mem, size_t memlen) {

    if (rb != 0 && mem != 0) {
        rb->buffer = mem;
        rb->size = memlen;
        rb->len = 0;
        rb->iw = 0;
        rb->ir = 0;
    }
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_get_len(ringbuffer_t* rb) {

    size_t len = 0;

    if (rb != 0) {
        len = rb->len;
    }

    return len;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_get_space(ringbuffer_t* rb) {

    size_t space = 0;

    if (rb != 0) {
        /* assuming len never exceeds size */
        space = (size_t)(rb->size - rb->len);
    }

    return space;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_write(ringbuffer_t* rb, uint8_t* data, size_t len) {

    /* the number of bytes actually written to ring buffer */
    size_t lenWritten = 0;

    if (rb != 0 && data != 0) {

        /* don't write more than there is space (assuming len never exceeds size) */
        size_t space = (size_t)(rb->size - rb->len);
        if (len > space) {
            len = space;
        }
        rb->len += len;
        lenWritten = len;

        /* assuming iw never exceeds size */
        size_t tmpLen = (size_t)(rb->size - rb->iw);
        if (len <= tmpLen) {

            /* copy data until end of buffer */
            memcpy(rb->buffer + rb->iw, data, len);

            /* advance write pointer */
            rb->iw += len;
            if (len == tmpLen) {
                /* here: rb->iw == rb->size, so wrap write pointer */
                rb->iw = 0;
            }

        } else {

            /* copy data until end of buffer */
            memcpy(rb->buffer + rb->iw, data, tmpLen);

            /* write pointer implicitly wrapped */
            rb->iw = len - tmpLen;

            /* copy remaining data to beginning of buffer */
            memcpy(rb->buffer, data + tmpLen, rb->iw);

        }
    }

    return lenWritten;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_read(ringbuffer_t* rb, uint8_t* data, size_t len) {

    /* the number of bytes actually read from ring buffer */
    size_t lenRead = 0;

    if (rb != 0 && data != 0) {

        /* don't read more than there is data */
        if (len > rb->len) {
            len = rb->len;
        }
        rb->len -= len;
        lenRead = len;

        /* assuming read pointer never exceeds size */
        size_t tmpLen = (size_t)(rb->size - rb->ir);
        if (len <= tmpLen) {

            /* copy data until end of buffer */
            memcpy(data, rb->buffer + rb->ir, len);

            /* advance read pointer */
            rb->ir += len;
            if (len == tmpLen) {
                /* here: rb->ir == rb->size, so wrap read pointer */
                rb->ir = 0;
            }

        } else {

            /* copy data until end of buffer (assuming iw never exceeds size) */
            memcpy(data, rb->buffer + rb->ir, tmpLen);

            /* read pointer implicitly wrapped */
            rb->ir = len - tmpLen;

            /* copy remaining data to beginning of buffer */
            memcpy(data + tmpLen, rb->buffer, rb->ir);

        }
    }

    return lenRead;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_sniff(ringbuffer_t* rb, uint8_t* data, size_t len) {

    /* the number of bytes actually sniffed from ring buffer */
    size_t lenSniffed = 0;

    if (rb != 0 && data != 0) {

        /* don't read more than there is data */
        if (len > rb->len) {
            len = rb->len;
        }
        lenSniffed = len;

        /* assuming ir never exceeds size */
        size_t tmpLen = (size_t)(rb->size - rb->ir);
        if (len <= tmpLen) {

            /* copy data until end of buffer */
            memcpy(data, rb->buffer + rb->ir, len);

        } else {

            /* copy data until end of buffer (assuming iw never exceeds size) */
            memcpy(data, rb->buffer + rb->ir, tmpLen);

            /* copy remaining data to beginning of buffer */
            memcpy(data + tmpLen, rb->buffer, len - tmpLen);

        }
    }

    return lenSniffed;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_discard(ringbuffer_t* rb, size_t len) {

    /* the number of bytes actually discarded from ring buffer */
    size_t lenDiscarded = 0;

    if (rb != 0) {

        /* don't discard more than there is data */
        if (len > rb->len) {
            len = rb->len;
        }
        rb->len -= len;
        lenDiscarded = len;

        /* assuming ir never exceeds size */
        size_t tmpLen = (size_t)(rb->size - rb->ir);
        if (len <= tmpLen) {

            /* advance read pointer */
            rb->ir += len;
            if (len == tmpLen) {
                /* here: rb->ir == rb->size, so wrap read pointer */
                rb->ir = 0;
            }

        } else {

            /* read pointer implicitly wrapped */
            rb->ir = len - tmpLen;

        }
    }

    return lenDiscarded;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_write_frame(ringbuffer_t* rb, uint8_t* frame, size_t len) {

    /* the number of bytes from frame written to ring buffer */
    size_t lenWritten = 0;

    if (rb != 0) {

        /* only write frame if there is enough space for
         * the full frame(assuming len never exceeds size) */
        size_t space = (size_t)(rb->size - rb->len);
        if ((len + sizeof(size_t)) <= space) {

            /* prepend and write frame length */
            ringbuffer_write(rb, (uint8_t*)&len, sizeof(size_t));

            /* write the actual frame */
            lenWritten = ringbuffer_write(rb, frame, len);
        }
    }

    return lenWritten;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_read_frame(ringbuffer_t* rb, uint8_t* frame, size_t len) {

    /* the number of bytes from frame read from ring buffer */
    size_t lenRead = 0;

    if (rb != 0) {

        size_t lenHeader = 0;
        
        if (ringbuffer_sniff(rb, (uint8_t*)&lenHeader, sizeof(size_t))
                == sizeof(size_t)) {

            if (lenHeader + sizeof(size_t) <= rb->len
                    && len >= lenHeader) {

                /* discard header (we already read it) */
                ringbuffer_discard(rb, sizeof(size_t));
                
                /* the actual frame */
                lenRead = ringbuffer_read(rb, frame, lenHeader);
            }
        }
    }

    return lenRead;
}

