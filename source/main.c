
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <gba_sio.h>

#include <stdio.h>

#define STATE(x) ((bool)x ? "on" : "off")

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
	
	while (1) {
		VBlankIntrWait();
		iprintf("\x1b[2J");
		iprintf("\x1b[1;1HGPIO Tester");
		iprintf("\x1b[2;1H-----------");
		// SO
		iprintf("\x1b[4;3HSO (A): %s", STATE(REG_RCNT & GPIO_SO));

		scanKeys();

		keys_pressed = keysDown();
		keys_released = keysUp();

		// turn on/off GPIOs.
		if ( keys_pressed & KEY_A ) {
			// toggle
			REG_RCNT ^= GPIO_SO;
		}

		/*if ( keys_released & KEY_A ) {
			REG_RCNT &= ~GPIO_SO;
		}*/
		
	}
}

