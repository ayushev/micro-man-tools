#include "timestamp.h"

#ifndef TIMESTAMP_N_MAX
#define TIMESTAMP_N_MAX 128
#endif

static uint_fast16_t	timestamp_n = 0;
static uint32_t			timestamp_buf[TIMESTAMP_N_MAX];

extern void		timestamp_send_byte(uint8_t byte);
extern uint32_t	timestamp_get_ticks(void);


/*
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
 * ___________________________________________________________________________
 */
void timestamp_flush(void) {

	uint8_t			byte;
	uint32_t		stamp;
	uint_fast16_t	i;
	uint_fast8_t	j;

	static const uint8_t timestamp_hex[16] = {
			'0', '1', '2', '3',	'4', '5', '6', '7',
			'8', '9', 'A', 'B',	'C', 'D', 'E', 'F' };

	for (i = 0; i < timestamp_n; i++) {
		stamp = timestamp_buf[i];
		for (j = 8; j > 0; j--) {
			byte = timestamp_hex[(stamp & 0xF0000000) >> 28];
			stamp = stamp << 4;
			timestamp_send_byte(byte);
		}
		timestamp_send_byte('\r');
		timestamp_send_byte('\n');
	}

	timestamp_n = 0;
}
