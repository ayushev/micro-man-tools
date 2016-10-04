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

#include "timestamp.h"

#ifndef TIMESTAMP_N_MAX
#define TIMESTAMP_N_MAX 128
#endif


/* the counter to hold the current number of time stamps in the buffer */
static uint_fast16_t timestamp_n = 0;

/* the buffer to hold time stamps between being set and being set out */
static uint32_t timestamp_buf[TIMESTAMP_N_MAX];

/* externally defined function to send out one byte */
extern void timestamp_send_byte(uint8_t byte);

/* externally defined function to return current tick counter */
extern uint32_t timestamp_get_ticks(void);


/*
 * Function to set a time stamp, i.e. write a time stamp to the buffer
 * ___________________________________________________________________________
 */
void timestamp_set(uint_fast8_t tag) {

	/* read the current state of the tick counter */
	uint32_t ticks = timestamp_get_ticks();

	/* mask out 8 most significant bits ... */
	ticks &= 0x00FFFFFF;
	/* ... and place tag there instead */
	ticks |= ((uint32_t)tag) << 24;

	/* store in memory */
	timestamp_buf[timestamp_n++] = ticks;
}


/*
 * Function to send out all time stamps from the buffer and clear the buffer
 * ___________________________________________________________________________
 */
void timestamp_flush(void) {

	uint8_t			byte;
	uint32_t		stamp;
	uint_fast16_t	i;
	uint_fast8_t	j;

    /* array used to convert data to hex */
	static const uint8_t timestamp_hex[16] = {
			'0', '1', '2', '3',	'4', '5', '6', '7',
			'8', '9', 'A', 'B',	'C', 'D', 'E', 'F' };

    /* iterate over all time stamps in the buffer */
	for (i = 0; i < timestamp_n; i++) {

        /* load time stamp from buffer ... */
		stamp = timestamp_buf[i];

        /* ... and process it (nibble by nibble) */
		for (j = 8; j > 0; j--) {
			byte = timestamp_hex[(stamp & 0xF0000000) >> 28];
			stamp = stamp << 4;
			timestamp_send_byte(byte);
		}

        /* send out newline */
		timestamp_send_byte('\r');
		timestamp_send_byte('\n');
	}

    /* clear the buffer */
	timestamp_n = 0;
}
