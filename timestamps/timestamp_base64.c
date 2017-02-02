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

#include "timestamp_base64.h"

#ifndef TIMESTAMP_N_MAX
#define TIMESTAMP_N_MAX 128
#endif


/* the counter to hold the current number of time stamps in the buffer */
static uint_fast16_t timestamp_n = 0;

/* the buffer to hold time stamps between being set and being set out */
static timestamp_t timestamp_buf[TIMESTAMP_N_MAX];

/* externally defined function to send out one byte */
extern void timestamp_send_byte(uint8_t byte);

/* externally defined function to return current tick counter */
extern uint32_t timestamp_get_ticks(void);


/*
 * Function to set a time stamp, i.e. write a time stamp to the buffer
 * ___________________________________________________________________________
 */
void timestamp_set(uint_fast16_t tag) {

	/* store in memory */
	timestamp_buf[timestamp_n].ticks = timestamp_get_ticks();
	timestamp_buf[timestamp_n++].tag = tag;
}


/*
 * Function to send out all time stamps from the buffer and clear the buffer
 * ___________________________________________________________________________
 */
void timestamp_flush(void) {

	uint_fast32_t	w1;
	uint_fast32_t	w2;
	uint_fast16_t	i;

    /* array used to convert data to base64 */
	static const uint8_t timestamp_hex[64] = {
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
            'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
            'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
            'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
            'w', 'x', 'y', 'z', '0', '1', '2', '3',
            '4', '5', '6', '7', '8', '9', '+', '/' };

    /* iterate over all time stamps in the buffer */
	for (i = 0; i < timestamp_n; i++) {

        w1 = timestamp_buf[i].ticks;
        w2 = timestamp_buf[i].tag;

        timestamp_send_byte(timestamp_hex[(w1 & (0xFC000000 >> 0)) >> 26]); 
        timestamp_send_byte(timestamp_hex[(w1 & (0xFC000000 >> 6)) >> 20]); 
        timestamp_send_byte(timestamp_hex[(w1 & (0xFC000000 >> 12)) >> 14]); 
        timestamp_send_byte(timestamp_hex[(w1 & (0xFC000000 >> 18)) >> 8]); 
        timestamp_send_byte(timestamp_hex[(w1 & (0xFC000000 >> 24)) >> 2]); 
        timestamp_send_byte(timestamp_hex[((w1 & (0xFC000000 >> 30)) << 4) | ((w2 & 0x0000F000) >> 12)]); 
        timestamp_send_byte(timestamp_hex[(w2 & 0x00000FC0 >> 0) >> 6]); 
        timestamp_send_byte(timestamp_hex[(w2 & 0x00000FC0 >> 6) >> 0]); 

        /* send out newline */
		timestamp_send_byte('\r');
		timestamp_send_byte('\n');
	}

    /* clear the buffer */
	timestamp_n = 0;
}
