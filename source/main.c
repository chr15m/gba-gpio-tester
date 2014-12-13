
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <gba_sio.h>

#include <stdio.h>

#define STATE(x) ((bool)x ? "on " : "off")

void redraw(int line, char id, char keyname, char* state) {
	iprintf("\x1b[%d;3HS%c (%c): %s", line, id, keyname, state);
}

//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	int keys_pressed, keys_released;
	
	irqInit();
	irqEnable(IRQ_VBLANK);
	
	consoleDemoInit();
	
	// set the base GPIO flags
	REG_RCNT = R_GPIO | GPIO_SC_IO | GPIO_SD_IO | GPIO_SI_IO | GPIO_SO_IO;
	
	iprintf("\x1b[2J");
	iprintf("\x1b[1;1HGPIO Tester");
	iprintf("\x1b[2;1H-----------");
	redraw(4, 'O', 'A', STATE(REG_RCNT & GPIO_SO));
	redraw(5, 'I', 'B', STATE(REG_RCNT & GPIO_SI));
	redraw(6, 'C', 'L', STATE(REG_RCNT & GPIO_SC));
	redraw(7, 'D', 'R', STATE(REG_RCNT & GPIO_SD));
	
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

		/*if ( keys_released & KEY_A ) {
			REG_RCNT &= ~GPIO_SO;
		}*/
		
	}
}
