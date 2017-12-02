#include "src/globals.h"
#include "lib/neslib.h"
#include "lib/boilerplate.h"
#include "bin/build_info.h"
#pragma rodataseg ("ROM_00")
#pragma codeseg ("ROM_00")
const unsigned char titlePalette[16]={ 0x0f,0x01,0x21,0x31,0x0f,0x01,0x21,0x31,0x0f,0x06,0x16,0x26,0x0f,0x09,0x19,0x29 };


void banked_draw_title() {
	ppu_off();
	pal_bg(titlePalette);
	// Show a message to the user.
	put_str(NTADR_A(2,8), "No Name Yet");
	put_str(NTADR_A(2,12), "Ludum Dare 40 Entry");
	put_str(NTADR_A(2,20), "Press Start to play");

	// Also show some cool build info because we can.
	#if SHOW_VERSION_INFO
		put_str(NTADR_A(2,24), "Built: " BUILD_DATE);
		put_str(NTADR_A(2,25), "Build #" BUILD_NUMBER_STR " (" GIT_COMMIT_ID_SHORT " - " GIT_BRANCH ")");
		put_str(NTADR_A(2,26), "Commit counter: " COMMIT_COUNT_STR);
	#endif
	ppu_on_all();	
}

void banked_do_title() {
	scratch = pad_trigger(0);
	if (scratch & (PAD_A | PAD_START)) {
		gameState = GAME_STATE_POST_START;
	}

}