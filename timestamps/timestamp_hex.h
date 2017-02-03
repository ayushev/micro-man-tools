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

#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_

#include <stdint.h>


/* Function to set a time stamp, i.e. write a time stamp to the buffer */
void timestamp_set(uint_fast8_t tag);

/* Function to set a time stamp without a tag */
void timestamp_set_notag(void);

/* Function to send out all time stamps from the buffer and clear the buffer */
void timestamp_flush(void);


#endif
