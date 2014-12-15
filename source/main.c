
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_timers.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <gba_sio.h>
#include <mappy.h>

#include <stdio.h>

#define STATE(x) ((bool)x ? "on " : "off")

void redraw(int line, char id, char keyname, char* state) {
	iprintf("\x1b[%d;3HS%c (%c): %s", line, id, keyname, state);
}

void redrawTimer(char *state) {
	iprintf("\x1b[9;1HTimer 0 ISR (START): %s", state);
}

#define IWRAM_CODE	__attribute__((section(".iwram"), long_call))
// void InterruptProcess(void) __attribute__ ((section(".iwram")));   // the ISR must be declared in this way
inline void timerInterrupt(void) IWRAM_CODE;

inline void timerInterrupt(void) {
	// clear interrupt flag
	// dprintf("Hi!\n");
	REG_RCNT ^= GPIO_SC;
	redraw(6, 'C', 'L', STATE(REG_RCNT & GPIO_SC));
	REG_IF |= REG_IF;
}

//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	int keys_pressed, keys_released;
	
	irqInit();
	irqEnable(IRQ_VBLANK);
	
	// set the base GPIO flags
	REG_RCNT = R_GPIO | GPIO_SC_IO | GPIO_SD_IO | GPIO_SI_IO | GPIO_SO_IO;
	
	irqSet( IRQ_TIMER0, timerInterrupt );
	irqEnable( IRQ_TIMER0 );
	
	// turn off timer 0
	REG_TM0CNT_H = 0;
	// setup timer 0
	// 65536 - 64999 = 537
	// (16 * 1024 * 1024) / 537 = 31242 bps
	// MIDI is 31250 bsp at +/-1% tolerance (-1% => 30937, +1% => 31562)
	// so 31242 bps is a valid bps value
	REG_TM0CNT_L = 0xFDE7;   // 64999
	REG_TM0CNT_H = TIMER_START | TIMER_IRQ | 0x3;   // start + irq + f (16MHz)
	
	consoleDemoInit();
	
	iprintf("\x1b[2J");
	iprintf("\x1b[1;1HGPIO Tester");
	iprintf("\x1b[2;1H-----------");
	redraw(4, 'O', 'A', STATE(REG_RCNT & GPIO_SO));
	redraw(5, 'I', 'B', STATE(REG_RCNT & GPIO_SI));
	redraw(6, 'C', 'L', STATE(REG_RCNT & GPIO_SC));
	redraw(7, 'D', 'R', STATE(REG_RCNT & GPIO_SD));
	redrawTimer(STATE(REG_TM0CNT_H & TIMER_START));
	dprintf("Hi!\n");
	
	while (1) {
		VBlankIntrWait();
		//for (i=0; i<27; i++) {
		//	iprintf("\x1b[%d;0H*", i);
		//}
		
		// while(REG_VCOUNT != 160);
		scanKeys();

		keys_pressed = keysDown();
		keys_released = keysUp();

		// turn on/off GPIOs.
		if ( keys_pressed & KEY_A ) {
			// toggle
			REG_RCNT ^= GPIO_SO;
			redraw(4, 'O', 'A', STATE(REG_RCNT & GPIO_SO));
		}

		// turn on/off GPIOs.
		if ( keys_pressed & KEY_B ) {
			// toggle
			REG_RCNT ^= GPIO_SI;
			redraw(5, 'I', 'B', STATE(REG_RCNT & GPIO_SI));
		}

		// turn on/off GPIOs.
		if ( keys_pressed & KEY_L ) {
			// toggle
			REG_RCNT ^= GPIO_SC;
			redraw(6, 'C', 'L', STATE(REG_RCNT & GPIO_SC));
		}

		// turn on/off GPIOs.
		if ( keys_pressed & KEY_R ) {
			// toggle
			REG_RCNT ^= GPIO_SD;
			redraw(7, 'D', 'R', STATE(REG_RCNT & GPIO_SD));
		}

		// turn on/off 
		if ( keys_pressed & KEY_START ) {
			// toggle
			REG_TM0CNT_H ^= TIMER_START;
			redrawTimer(STATE(REG_TM0CNT_H & TIMER_START));
		}

		/*if ( keys_released & KEY_A ) {
			REG_RCNT &= ~GPIO_SO;
		}*/
		
	}
}
