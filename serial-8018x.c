//-------------------------------------------------------------------------------
// EMU86 - 8018X serial port
//-------------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "emu-serial.h"
#include "emu-int.h"

#include "int-8018x.h"
#include "serial-8018x.h"

//-------------------------------------------------------------------------------

#define SERIAL_STAT   0  // status register
#define SERIAL_TDAT   2  // transmit data register

static word_t serial_regs [SERIAL_REG_COUNT];

//#define SERIAL_CONTROL_TXIE 0x0800
//#define SERIAL_CONTROL_RXIE 0x0400

//#define SERIAL_STAT_TEMT  0x0040
//#define SERIAL_STAT_THRE  0x0020
#define SERIAL_STAT_TRDY   0x0008  // transmitter ready

//-------------------------------------------------------------------------------

// Set the serial interrupt line

void serial_int (void)
	{
	int stat = 0;

	/*
	if ((serial_regs [SERIAL_STAT] & SERIAL_STATUS_RDR)
		&&  (serial_regs [SERIAL_MODE] & SERIAL_CONTROL_RXIE))
		stat = 1;

	if ((serial_regs [SERIAL_STAT] & SERIAL_STATUS_THRE)
		&& (serial_regs [SERIAL_MODE] & SERIAL_CONTROL_TXIE))
		stat = 1;
	*/

	int_line_set (INT_LINE_SERIAL, stat);
	}

// Serial device procedure
// Called from main emulator loop

static int loop_count = 0;
#define LOOP_MAX 5000

int serial_proc (void)
	{
	int err = 0;

	while (1) {
		// Input stream

		while (1)
			{
			// Moderate the poll rate
			// for more instruction processing

			loop_count++;
			if (loop_count < LOOP_MAX) break;
			loop_count = 0;

			int res = serial_poll ();
			if (!res) break;

			byte_t c;
			err = serial_recv (&c);
			if (err) break;

			// TODO: overflow detection

			//serial_regs [SERIAL_RECV] = (word_t) c;
			//serial_regs [SERIAL_STAT] |= SERIAL_STATUS_RDR;

			serial_int ();
			break;
			}

		if (err) break;

		// Output stream

		while (1) {
			if (serial_regs [SERIAL_STAT] & SERIAL_STAT_TRDY) break;

			err = serial_send ((byte_t) serial_regs [SERIAL_TDAT]);
			if (err) break;

			serial_regs [SERIAL_STAT] |= SERIAL_STAT_TRDY;
			serial_int ();
			break;
			}

		break;
		}

	return err;
	}

// Serial I/O read

int serial_io_read (word_t p, word_t * w)
	{
	int r = p >> 1;
	*w = serial_regs [r];

	/*
	if (r == SERIAL_REG_RDATA) {
		serial_regs [SERIAL_STAT] &= ~SERIAL_STATUS_RDR;
		serial_int ();
		}
	 */

	return 0;
	}

// Serial I/O write

int serial_io_write (word_t p, word_t  w)
	{
	int r = p >> 1;
	serial_regs [r] = w;

	if (r == SERIAL_TDAT) {
		serial_regs [SERIAL_STAT] &= ~(SERIAL_STAT_TRDY);
		serial_int ();
		}

	return 0;
	}

// Serial device initialization

void serial_dev_init ()
	{
	// Initialize registers

	//serial_regs [SERIAL_MODE] = SERIAL_CONTROL_RXIE;
	serial_regs [SERIAL_STAT] = SERIAL_STAT_TRDY;
	}

void serial_dev_term ()
	{
	}
