#include "lib/neslib.h"
#include "src/globals.h"


void banked_draw_hud() {
    // Presumes ppu is already off.
    // Fill all palettes out
    vram_adr(0x23f0);
    for (i = 0; i < 16; i++) {
        vram_put(0x55);
    }

    vram_adr(NTADR_A(0, 24));
    vram_put(HUD_TL);
    vram_fill(HUD_H, 30);
    vram_put(HUD_TR);
    for (i = 0; i < 3; i++) {
        vram_put(HUD_V);
        vram_fill(HUD_BLANK, 30);
        vram_put(HUD_V);
    }
    vram_put(HUD_BL);
    vram_fill(HUD_H, 30);
    vram_put(HUD_BR);

    vram_adr(NTADR_A(20, 26));
    vram_put(HUD_GEMS);
    vram_put(HUD_GEMS+1);
    vram_put(HUD_GEMS+2);
    vram_put(HUD_GEMS+3);
    vram_put(HUD_COLON);
    vram_put(HUD_NUMBERS);
    vram_put(HUD_NUMBERS + gemCount);
    vram_put(HUD_SLASH);
    vram_put(HUD_NUMBERS);
    vram_put(HUD_NUMBERS + gemsInLevel);

    vram_adr(NTADR_A(2, 26));
    vram_put(HUD_SP);
    vram_put(HUD_SP+1);
    vram_put(HUD_E);
    vram_put(HUD_E);
    vram_put(HUD_D);
    vram_put(HUD_COLON);
    vram_put(HUD_NUMBERS);
    vram_put(HUD_NUMBERS + ((DEFAULT_SPEED - gemCount) > 0 ? DEFAULT_SPEED - gemCount : 0));

}

void banked_draw_pause() {
	set_vram_update(NULL); // STOP HELPING, ENGINE
    vram_adr(NAMETABLE_C + 0x03c0);
    vram_fill(0xaa, 0x40);
	oam_clear();

	vram_adr(NTADR_C(6, 11));
	vram_put(HUD_TL);
	vram_fill(HUD_H, 16);
	vram_put(HUD_TR);
	vram_adr(NTADR_C(6, 12));
	vram_put(HUD_V);
	vram_fill(HUD_BLANK, 16);
	vram_put(HUD_V);
	vram_adr(NTADR_C(6, 13));
	vram_put(HUD_V);
    vram_fill(HUD_BLANK, 5);
    vram_put(HUD_P);
    vram_put(HUD_A);
    vram_put(HUD_U);
    vram_put(HUD_GEMS+3);
    vram_put(HUD_E);
    vram_put(HUD_D);
    vram_fill(HUD_BLANK, 5);
	vram_put(HUD_V);
	vram_adr(NTADR_C(6, 14));
	vram_put(HUD_V);
	vram_fill(HUD_BLANK, 16);
	vram_put(HUD_V);
	vram_adr(NTADR_C(6, 15));
	vram_put(HUD_BL);
	vram_fill(HUD_H, 16);
	vram_put(HUD_BR);

}

void banked_do_pause() {
    while(1) {
        if (pad_trigger(0) & PAD_START) {
            sfx_play(SFX_PAUSE_DOWN, 2);
            break;
        }
        ppu_wait_frame();
    }
}