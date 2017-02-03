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

#include "microtags.h"

#ifndef MICROTAGS_N_MAX
    #define MICROTAGS_N_MAX 128
#endif

#ifndef MICROTAGS_GET_TICKS

    #define MICROTAGS_GET_TICKS() microtags_get_ticks()

    /* externally defined function to return current tick counter */
    extern uint32_t microtags_get_ticks(void);

#endif


/* the counter to hold the current number of microtags in the buffer */
static uint_fast16_t n_microtags = 0;

/* the buffer to hold microtags between being set and being sent out */
static microtag_t buf_microtags[MICROTAGS_N_MAX];


/*
 * Function to set a ticks-based microtag, i.e. write a microtag to the buffer
 * ___________________________________________________________________________
 */
void microtags_set_ticks(uint_fast16_t id) {

	/* store in memory */
	buf_microtags[n_microtags].data = MICROTAGS_GET_TICKS();
	buf_microtags[n_microtags++].id = id;
}


/*
 * Function to set a data-based microtag, i.e. write a microtag to the buffer
 * ___________________________________________________________________________
 */
void microtags_set_data(uint_fast16_t id, uint_fast32_t data) {

	/* store in memory */
	buf_microtags[n_microtags].data = data;
	buf_microtags[n_microtags++].id = id;
}


/*
 * Function to send out all microtags from the buffer and clear the buffer
 * ___________________________________________________________________________
 */
void microtags_flush_text(microtags_send_byte_t microtags_send_byte) {

    /* array used to convert data to base64 */
	static const uint8_t microtags_base64[64] = {
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
            'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
            'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
            'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
            'w', 'x', 'y', 'z', '0', '1', '2', '3',
            '4', '5', '6', '7', '8', '9', '+', '/' };

	uint_fast32_t	data;
	uint_fast32_t	id;
	uint_fast16_t	i;

    if (microtags_send_byte != 0) {

        /* iterate over all microtags in the buffer */
	    for (i = 0; i < n_microtags; ++i) {

            data = buf_microtags[i].data;
            id = buf_microtags[i].id;

            (*microtags_send_byte)(microtags_base64[
                /* most-significant bits of data ... */
                (data & 0xFC000000) >> 26]); 
            (*microtags_send_byte)(microtags_base64[
                (data & (0xFC000000 >> 6)) >> 20]); 
            (*microtags_send_byte)(microtags_base64[
                (data & (0xFC000000 >> 12)) >> 14]); 
            (*microtags_send_byte)(microtags_base64[
                (data & (0xFC000000 >> 18)) >> 8]); 
            (*microtags_send_byte)(microtags_base64[
                (data & (0xFC000000 >> 24)) >> 2]);
            (*microtags_send_byte)(microtags_base64[
                /* ... least-significant bits of data */
                ((data & (0xFC000000 >> 30)) << 4)
                /* most-significant bits of ID ... */
                | ((id & 0x0000F000) >> 12)]); 
            (*microtags_send_byte)(microtags_base64[
                (id & 0x00000FC0 >> 0) >> 6]); 
            (*microtags_send_byte)(microtags_base64[
                /* ... least-significant bits of ID */
                id & 0x00000FC0 >> 6]); 

            /* send out newline */
		    (*microtags_send_byte)('\r');
		    (*microtags_send_byte)('\n');
	    }

        /* clear the buffer */
	    n_microtags = 0;
    }
}


