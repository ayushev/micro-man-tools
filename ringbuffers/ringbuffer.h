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

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <stdint.h>
#include <stddef.h>


typedef struct {

    /* pointer to actual buffer */
    uint8_t* buffer;

    /* size of buffer */
    size_t size;

    /* length of content */
    size_t len;

    /* writing index */
    size_t iw;

    /* reading index */
    size_t ir;

} ringbuffer_t;


/* TODO: Add description */
void ringbuffer_init(ringbuffer_t* rb, uint8_t* mem, size_t memlen);

/* TODO: Add description */
size_t ringbuffer_get_len(ringbuffer_t* rb);

/* TODO: Add description */
size_t ringbuffer_get_space(ringbuffer_t* rb);

/* TODO: Add description */
void ringbuffer_clear(ringbuffer_t* rb);

/* TODO: Add description */
size_t ringbuffer_write(ringbuffer_t* rb, uint8_t* data, size_t len);

/* TODO: Add description */
size_t ringbuffer_read(ringbuffer_t* rb, uint8_t* data, size_t len);

/* TODO: Add description */
size_t ringbuffer_read_memory(ringbuffer_t* rb, uint8_t** data, size_t len);

/* TODO: Add description */
size_t ringbuffer_sniff(ringbuffer_t* rb, uint8_t* data, size_t len);

/* TODO: Add description */
size_t ringbuffer_sniff_offset(
        ringbuffer_t* rb, size_t offset, uint8_t* data, size_t len);

/* TODO: Add description */
size_t ringbuffer_discard(ringbuffer_t* rb, size_t len);


/* TODO: Add description */
size_t ringbuffer_write_frame(ringbuffer_t* rb, uint8_t* frame, size_t len);

/* TODO: Add description */
size_t ringbuffer_read_frame(ringbuffer_t* rb, uint8_t* frame, size_t len);

/* TODO: Add description */
size_t ringbuffer_sniff_frame(ringbuffer_t* rb, uint8_t* frame, size_t len);

/* TODO: Add description */
size_t ringbuffer_discard_frame(ringbuffer_t* rb);

#endif
