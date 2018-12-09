#include "sim_avr.h"
#include "avr_eeprom.h"
#include "avr_watchdog.h"
#include "avr_extint.h"
#include "avr_ioport.h"
#include "avr_timer.h"
#include "avr_adc.h"
#include "avr_acomp.h"

#define _AVR_IO_H_
#define __ASSEMBLER__
#include "avr/iotn9.h"

#include "sim_core_declare.h"

static void init(struct avr_t * avr);
static void reset(struct avr_t * avr);


static const struct mcu_t {
	avr_t core;
	avr_watchdog_t	watchdog;
	avr_extint_t	extint;
	avr_ioport_t	portb;
	avr_timer_t		timer0;
} mcu = {
	.core = {
		.mmcu = "attiny9",

		/*
		 * tiny9 has no extended fuse byte, so can not use DEFAULT_CORE macro
		 */
		.ramend = RAMEND,
		.flashend = FLASHEND,
		.e2end = E2END,
		.vector_size = 2,
// Disable signature when using an old avr toolchain
#ifdef SIGNATURE_0
		.signature = { SIGNATURE_0,SIGNATURE_1,SIGNATURE_2 },
		//.fuse = { LFUSE_DEFAULT, HFUSE_DEFAULT },
#endif
		.init = init,
		.reset = reset,
	},
	.watchdog = {
		.wdrf = AVR_IO_REGBIT(RSTFLR, WDRF),
		//.wdce = AVR_IO_REGBIT(_WDSR, WDCE),
		.wde = AVR_IO_REGBIT(WDTCSR, WDE),
		.wdp = { AVR_IO_REGBIT(WDTCSR, WDP0),AVR_IO_REGBIT(WDTCSR, WDP1),
				AVR_IO_REGBIT(WDTCSR, WDP2),AVR_IO_REGBIT(WDTCSR, WDP3) },
		.watchdog = {
			.enable = AVR_IO_REGBIT(WDTCSR, WDIE),
			.raised = AVR_IO_REGBIT(WDTCSR, WDIF),
			.vector = WDT_vect,
		},
	},
	.extint = {
		//AVR_EXTINT_TINY_DECLARE(0, 'B', 2, EIFR),
		.eint[0] = {
			.port_ioctl = AVR_IOCTL_IOPORT_GETIRQ('B'),
			.port_pin = 2,
			.isc = { AVR_IO_REGBIT(EICRA, ISC00), AVR_IO_REGBIT(EICRA, ISC01) },
			.vector = {
				.enable = AVR_IO_REGBIT(EIMSK, INT0),
				.raised = AVR_IO_REGBIT(EIFR, INTF0),
				.vector = INT0_vect,
			},
		},
	},
	.portb = {
		.name = 'B',  .r_port = PORTB, .r_ddr = DDRB, .r_pin = PINB,
		.pcint = {
			.enable = AVR_IO_REGBIT(PCICR, PCIE0),
			.raised = AVR_IO_REGBIT(PCIFR, PCIF0),
			.vector = PCINT0_vect,
		},
		.r_pcint = PCMSK,
	},
	.timer0 = {
		.name = '0',
		.wgm = { AVR_IO_REGBIT(TCCR0A, WGM00), AVR_IO_REGBIT(TCCR0A, WGM01), AVR_IO_REGBIT(TCCR0B, WGM02) },
		.wgm_op = {
			[0] = AVR_TIMER_WGM_NORMAL8(),
			[2] = AVR_TIMER_WGM_CTC(),
			[3] = AVR_TIMER_WGM_FASTPWM8(),
			[7] = AVR_TIMER_WGM_OCPWM(),
		},
		.cs = { AVR_IO_REGBIT(TCCR0B, CS00), AVR_IO_REGBIT(TCCR0B, CS01), AVR_IO_REGBIT(TCCR0B, CS02) },
		.cs_div = { 0, 0, 3 /* 8 */, 6 /* 64 */, 8 /* 256 */, 10 /* 1024 */ },

		.r_tcnt = TCNT0,

		.overflow = {
			.enable = AVR_IO_REGBIT(TIMSK0, TOIE0),
			.raised = AVR_IO_REGBIT(TIFR0, TOV0),
			.vector = TIM0_OVF_vect,
		},
		.comp = {
			[AVR_TIMER_COMPA] = {
				.r_ocr = OCR0A,
				.interrupt = {
					.enable = AVR_IO_REGBIT(TIMSK0, OCIE0A),
					.raised = AVR_IO_REGBIT(TIFR0, OCF0A),
					.vector = TIM0_COMPA_vect,
				}
			},
			[AVR_TIMER_COMPB] = {
				.r_ocr = OCR0B,
				.interrupt = {
					.enable = AVR_IO_REGBIT(TIMSK0, OCIE0B),
					.raised = AVR_IO_REGBIT(TIFR0, OCF0B),
					.vector = TIM0_COMPB_vect,
				}
			}
		}
	},
};

static avr_t * make()
{
	return avr_core_allocate(&mcu.core, sizeof(struct mcu_t));
}

avr_kind_t tiny9 = {
	.names = { "attiny9" },
	.make = make
};

static void init(struct avr_t * avr)
{
	struct mcu_t * mcu = (struct mcu_t*)avr;

	avr_watchdog_init(avr, &mcu->watchdog);
	avr_extint_init(avr, &mcu->extint);
	avr_ioport_init(avr, &mcu->portb);
	avr_timer_init(avr, &mcu->timer0);
}

static void reset(struct avr_t * avr)
{
//	struct mcu_t * mcu = (struct mcu_t*)avr;
}

/*
 	This file is part of simavr.

	simavr is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	simavr is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with simavr.  If not, see <http://www.gnu.org/licenses/>.
 */
