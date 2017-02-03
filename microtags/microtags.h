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

#ifndef MICROTAGS_H_
#define MICROTAGS_H_

#include <stdint.h>


/* definition of a single microtag */
typedef struct {

    /* 32-bit data of the microtag */
    uint32_t data;

    /* 16-bit id of the microtag */
    uint16_t id;

} microtag_t;

/* definition of function pointer to send out a single byte */
typedef void (*microtags_send_byte_t)(uint8_t byte);

/* Function to set a ticks-based microtag, i.e. write a microtag to the buffer */
void microtags_set_ticks(uint_fast16_t id);

#define MICROTAGS_SET_TICKS(id) microtags_set_ticks(id)

/* Function to set a data-based microtag, i.e. write a microtag to the buffer */
void microtags_set_data(uint_fast16_t id, uint_fast32_t data);

#define MICROTAGS_SET_DATA(id, data) microtags_set_data(id, data)

/* Function to send out all microtags from the buffer and clear the buffer */
void microtags_flush_text(microtags_send_byte_t microtags_send_byte);

/* Function to clear the buffer */
void microtags_clear(void);


#endif
