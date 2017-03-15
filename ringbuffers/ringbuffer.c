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
void ringbuffer_clear(ringbuffer_t* rb) {

    if (rb != 0) {
        rb->len = 0;
        rb->iw = 0;
        rb->ir = 0;
    }
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_write(ringbuffer_t* rb, uint8_t* data, size_t len) {

    /* the number of bytes actually written to ring buffer */
    size_t lenWritten = 0;

    if (rb != 0 && data != 0) {

        /* don't write more than there is space
         * (assuming len never exceeds size) */
        size_t space = (size_t)(rb->size - rb->len);
        if (len > space) {
            len = space;
        }
        rb->len += len;
        lenWritten = len;

        /* assuming write index never exceeds size */
        size_t tmpLen = (size_t)(rb->size - rb->iw);
        if (len <= tmpLen) {

            /* copy data until end of buffer */
            memcpy(rb->buffer + rb->iw, data, len);

            /* advance write index */
            rb->iw += len;
            if (len == tmpLen) {
                /* here: rb->iw == rb->size, so wrap write index */
                rb->iw = 0;
            }

        } else {

            /* copy data until end of buffer */
            memcpy(rb->buffer + rb->iw, data, tmpLen);

            /* write index implicitly wrapped */
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

        /* assuming read index never exceeds size */
        size_t tmpLen = (size_t)(rb->size - rb->ir);
        if (len <= tmpLen) {

            /* copy data until end of buffer */
            memcpy(data, rb->buffer + rb->ir, len);

            /* advance read index */
            rb->ir += len;
            if (len == tmpLen) {
                /* here: rb->ir == rb->size, so wrap read index */
                rb->ir = 0;
            }

        } else {

            /* copy data until end of buffer (assuming
             * write index never exceeds size) */
            memcpy(data, rb->buffer + rb->ir, tmpLen);

            /* read index implicitly wrapped */
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
size_t ringbuffer_read_memory(ringbuffer_t* rb, uint8_t** data, size_t len) {

	return 0;
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

        /* assuming read index never exceeds size */
        size_t tmpLen = (size_t)(rb->size - rb->ir);
        if (len <= tmpLen) {

            /* copy data until end of buffer */
            memcpy(data, rb->buffer + rb->ir, len);

        } else {

            /* copy data until end of buffer (assuming
             * write index never exceeds size) */
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
size_t ringbuffer_sniff_offset(
        ringbuffer_t* rb, size_t offset, uint8_t* data, size_t len) {

    /* the number of bytes actually sniffed from ring buffer */
    size_t lenSniffed = 0;

    if (rb != 0 && data != 0) {

        /* vLen is the "virtual" length of the ring buffer's
         *  content after considering data to disregard (offset) */
        size_t vLen = rb->len;
        if (offset < vLen) {
            vLen -= offset;
        } else {
            vLen = 0;
        }

        /* don't read more than there is data */
        if (len > vLen) {
            len = vLen;
        }
        lenSniffed = len;

        /* the "virtual" read index of the ring buffer's
         * after considering data to disregard (offset) */
        size_t vir = rb->ir + offset;
        if (vir >= rb->size) {
            vir -= rb->size;
        }

        /* assuming read index never exceeds size */
        size_t tmpLen = (size_t)(rb->size - vir);
        if (len <= tmpLen) {

            /* copy data until end of buffer */
            memcpy(data, rb->buffer + vir, len);

        } else {

            /* copy data until end of buffer (assuming
             * write index never exceeds size) */
            memcpy(data, rb->buffer + vir, tmpLen);

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

        /* assuming read index never exceeds size */
        size_t tmpLen = (size_t)(rb->size - rb->ir);
        if (len <= tmpLen) {

            /* advance read index */
            rb->ir += len;
            if (len == tmpLen) {
                /* here: rb->ir == rb->size, so wrap read index */
                rb->ir = 0;
            }

        } else {

            /* read index implicitly wrapped */
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


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_sniff_frame(ringbuffer_t* rb, uint8_t* frame, size_t len) {

    /* the number of bytes from frame sniffed from ring buffer */
    size_t lenSniffed = 0;

    if (rb != 0) {

    	/* length of frame read from header in ring buffer */
        size_t lenHeader = 0;
        
        if ((ringbuffer_sniff(rb, (uint8_t*)&lenHeader,
        		sizeof(size_t)) == sizeof(size_t))
        				&& (lenHeader + sizeof(size_t) <= rb->len)
						&& (len >= lenHeader)) {
                
			/* sniff the actual frame */
			lenSniffed = ringbuffer_sniff_offset(
					rb, sizeof(size_t), frame, lenHeader);
        }
    }

    return lenSniffed;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_sniff_frame_length(ringbuffer_t* rb) {

    /* the length of the next frame in the ring buffer */
    size_t length = 0;

    if (rb != 0) {
        if ((ringbuffer_sniff(rb, (uint8_t*)&length,
        		sizeof(size_t)) != sizeof(size_t))
        				|| (length + sizeof(size_t) > rb->len)) {
        	/* no frame or invalid frame */
        	length = 0;
        }
    }

    return length;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_discard_frame(ringbuffer_t* rb) {

    /* the number of bytes from frame discarded from ring buffer */
    size_t lenDiscarded = 0;

    if (rb != 0) {

        size_t lenHeader = 0;
        
        if ((ringbuffer_sniff(rb, (uint8_t*)&lenHeader,
        		sizeof(size_t)) == sizeof(size_t))
        				&& ((lenHeader + sizeof(size_t)) <= rb->len)) {

			/* discard frame */
			lenDiscarded = ringbuffer_discard(rb, lenHeader + sizeof(size_t));
        }
    }

    return lenDiscarded;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_count_frames(ringbuffer_t* rb) {

    size_t n = 0;

    if (rb != 0) {

    	size_t flen;

    	size_t len = rb->len;
    	size_t offset = 0;
    	while (len > sizeof(size_t)) {
    		len -= sizeof(size_t);
    		if (ringbuffer_sniff_offset(rb, offset, (uint8_t*)&flen, sizeof(size_t))
    				== sizeof(size_t) && flen <= len) {
    			/* skip this frame */
        		len -= flen;
        		offset += sizeof(size_t) + flen;
        		++n;
    		} else {
    			/* something is wrong */
    			n = 0;
    			len = 0;
    		}
    	}
    }

    return n;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_write_frame_with_header(ringbuffer_t* rb,
		uint8_t* header, size_t hlen, uint8_t* frame, size_t flen) {

    /* the total number of bytes written to ring buffer */
    size_t n = 0;

    if (rb != 0) {

        /* only write frame if there is enough space for
         * the full frame (assuming len never exceeds size) */
        if ((sizeof(size_t) + hlen + flen) <= (size_t)(rb->size - rb->len)) {

        	/* the total frame length including header */
        	size_t len = hlen + flen;

            /* prepend and write total frame length */
            n += ringbuffer_write(rb, (uint8_t*)&len, sizeof(size_t));

            /* write header */
            n += ringbuffer_write(rb, header, hlen);

            /* write frame */
            n += ringbuffer_write(rb, frame, flen);

        }
    }

    return n;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_read_frame_with_header(ringbuffer_t* rb,
		uint8_t* header, size_t hlen, uint8_t* frame, size_t max_flen) {

    /* the number of frame bytes read from ring buffer */
    size_t n = 0;

    if (rb != 0) {

    	/* the length of the next frame in the ring buffer */
        size_t len = 0;

        if (ringbuffer_sniff(rb, (uint8_t*)&len, sizeof(size_t))
                == sizeof(size_t)) {

            if (len + sizeof(size_t) <= rb->len
            		&& (hlen + max_flen) >= len) {

                /* discard length (we already know it) */
                ringbuffer_discard(rb, sizeof(size_t));

                /* the header */
                ringbuffer_read(rb, header, hlen);

                /* the frame */
                n = ringbuffer_read(rb, frame, len - hlen);

            }
        }
    }

    return n;
}


/*
 * ___________________________________________________________________________
 */
size_t ringbuffer_sniff_frame_with_header(ringbuffer_t* rb,
		uint8_t* header, size_t hlen, uint8_t* frame, size_t max_flen) {

    /* the number of frame bytes read from ring buffer */
    size_t n = 0;

    if (rb != 0) {

    	/* the length of the next frame in the ring buffer */
        size_t len = 0;

        if (ringbuffer_sniff(rb, (uint8_t*)&len, sizeof(size_t))
                == sizeof(size_t)) {

            if (len + sizeof(size_t) <= rb->len
            		&& (hlen + max_flen) >= len) {

                /* the header */
                ringbuffer_sniff_offset(rb, sizeof(size_t), header, hlen);

                /* the frame */
                n = ringbuffer_sniff_offset(rb, sizeof(size_t) + hlen,
                		frame, len - hlen);

            }
        }
    }

    return n;
}
