#include "src/globals.h"
#include "lib/neslib.h"
#include "lib/boilerplate.h"
#include "bin/build_info.h"
#pragma rodataseg ("ROM_00")
#pragma codeseg ("ROM_00")
const unsigned char titlePalette[16] = { 0x0f,0x21,0x21,0x31,0x0f,0x01,0x21,0x31,0x0f,0x06,0x16,0x26,0x0f,0x09,0x19,0x29 };



void banked_draw_title() {
	ppu_off();
	pal_bg(titlePalette);

	set_chr_bank_0(CHR_BANK_TITLE);
	set_chr_bank_1(CHR_BANK_TITLE+1);
	clear_screen();

	// Show a message to the user.
	put_str(NTADR_A(2,8), "No Name Yet");
	put_str(NTADR_A(2,12), "Ludum Dare 40 Entry");
	put_str(NTADR_A(2,20), "Press Start to play");

	// Also show some cool build info because we can.
	#if SHOW_VERSION_INFO
		put_str(NTADR_A(2,24), "Built: " BUILD_DATE);
		put_str(NTADR_A(2,25), "Build #" BUILD_NUMBER_STR " (" GIT_COMMIT_ID_SHORT " - " GIT_BRANCH ")");
		put_str(NTADR_A(2,26), "Commit counter: " COMMIT_COUNT_STR);
		#if DEBUG
			put_str(NTADR_A(6,28), "DEBUG MODE ENABLED");
		#endif
	#endif
	ppu_on_all();	
}

void banked_do_title() {
	scratch = pad_trigger(0);
	if (scratch & (PAD_A | PAD_START)) {
		gameState = GAME_STATE_POST_START;
	}
	ppu_wait_frame();

}

void banked_show_level_complete() {

	ppu_off();
	pal_bg(titlePalette);
	set_chr_bank_0(CHR_BANK_TITLE);
	set_chr_bank_1(CHR_BANK_TITLE+1);

	set_vram_update(NULL); // STOP HELPING, ENGINE
	clear_screen();
	oam_clear();

	vram_adr(NTADR_A(6, 11));
	vram_put(HUD_TL);
	vram_fill(HUD_H, 17);
	vram_put(HUD_TR);
	vram_adr(NTADR_A(6, 12));
	vram_put(HUD_V);
	vram_fill(HUD_BLANK, 17);
	vram_put(HUD_V);
	vram_adr(NTADR_A(6, 13));
	vram_put(HUD_V);
	put_str(NTADR_A(8, 13), "Level Complete!");
	vram_put(HUD_BLANK);
	vram_put(HUD_V);
	vram_adr(NTADR_A(6, 14));
	vram_put(HUD_V);
	vram_fill(HUD_BLANK, 17);
	vram_put(HUD_V);
	vram_adr(NTADR_A(6, 15));
	vram_put(HUD_BL);
	vram_fill(HUD_H, 17);
	vram_put(HUD_BR);

	ppu_on_all();

}

void banked_do_level_complete() {
	// TODO: timeout after x rounds.
	while (1) {
		scratch = pad_trigger(0);
		if (scratch & (PAD_A | PAD_START)) {
			gameState = GAME_STATE_POST_START;
			break;
		}
		ppu_wait_frame();
	}
}

void banked_draw_dead() {
	ppu_off();
	pal_bg(titlePalette);
	set_chr_bank_0(CHR_BANK_TITLE);
	set_chr_bank_1(CHR_BANK_TITLE+1);

	set_vram_update(NULL); // STOP HELPING, ENGINE
	clear_screen();
	oam_clear();

	vram_adr(NTADR_A(6, 11));
	vram_put(HUD_TL);
	vram_fill(HUD_H, 17);
	vram_put(HUD_TR);

	vram_adr(NTADR_A(6, 12));
	vram_put(HUD_V);
	vram_fill(HUD_BLANK, 17);
	vram_put(HUD_V);

	vram_adr(NTADR_A(6, 13));
	vram_put(HUD_V);
	put_str(NTADR_A(9, 13), "Bunny Didn't"); 
	vram_fill(HUD_BLANK, 3);
	vram_put(HUD_V);

	vram_adr(NTADR_A(6, 14));
	vram_put(HUD_V);
	put_str(NTADR_A(10, 14), "Make it...");
	vram_fill(HUD_BLANK, 4);
	vram_put(HUD_V);


	vram_adr(NTADR_A(6, 15));
	vram_put(HUD_V);
	vram_fill(HUD_BLANK, 17);
	vram_put(HUD_V);

	vram_adr(NTADR_A(6, 16));
	vram_put(HUD_BL);
	vram_fill(HUD_H, 17);
	vram_put(HUD_BR);

	ppu_on_all();
}

void banked_do_dead() {
	// TODO: timeout after x rounds.
	while (1) {
		scratch = pad_trigger(0);
		if (scratch & (PAD_A | PAD_START)) {
			gameState = GAME_STATE_POST_START;
			break;
		}
		ppu_wait_frame();
	}

}