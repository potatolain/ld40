#include "lib/neslib.h"
#include "src/globals.h"
#include "src/sprite.h"
#pragma rodataseg ("ROM_00")
#pragma codeseg ("ROM_00")

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
    vram_put(HUD_NUMBERS + ((DEFAULT_SPEED - gemCount - antiGemCount) > 0 ? DEFAULT_SPEED - gemCount - antiGemCount : 0));

}

void banked_draw_pause() {
	set_vram_update(NULL); // STOP HELPING, ENGINE
    vram_adr(NAMETABLE_C + 0x03c0);
    vram_fill(0x55, 0x40);

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
	vram_put(HUD_V);
	vram_fill(HUD_BLANK, 16);
    vram_put(HUD_V);

    
    vram_adr(NTADR_C(6, 16));
    vram_put(HUD_V);
    vram_fill(HUD_BLANK, 4);
    vram_put(HUD_C);
    vram_put(HUD_O);
    vram_put(HUD_N);
    vram_put(HUD_T);
    vram_put(HUD_I);
    vram_put(HUD_N);
    vram_put(HUD_U);
    vram_put(HUD_E);
    vram_fill(HUD_BLANK, 4);
    vram_put(HUD_V);

    vram_adr(NTADR_C(6, 17));
    vram_put(HUD_V);
    vram_fill(HUD_BLANK, 4);
    vram_put(HUD_R);
    vram_put(HUD_E);
    vram_put(HUD_GEMS+3);
    vram_put(HUD_E);
    vram_put(HUD_T);
    vram_fill(HUD_BLANK, 7);
    vram_put(HUD_V);


    vram_adr(NTADR_C(6, 18));
    vram_put(HUD_V);
    vram_fill(HUD_BLANK, 16);
    vram_put(HUD_V);

	vram_adr(NTADR_C(6, 19));
	vram_put(HUD_BL);
	vram_fill(HUD_H, 16);
	vram_put(HUD_BR);

}

void banked_do_pause() {
    scratch = 0;
    while(1) {
        scratch2 = pad_trigger(0);
        if (scratch == 0 && scratch2 & PAD_DOWN) {
            scratch = 1;
            sfx_play(SFX_PAUSE_SWAP, 2);
        }
        if (scratch == 1 && scratch2 & PAD_UP) {
            scratch = 0;
            sfx_play(SFX_PAUSE_SWAP, 2);
        }

        screenBuffer[0] = MSB(NTADR_C(8, 16));
        screenBuffer[1] = LSB(NTADR_C(8, 16));
        screenBuffer[2] = scratch ? HUD_BLANK : HUD_ARROW;
        screenBuffer[3] = MSB(NTADR_C(8, 17));
        screenBuffer[4] = LSB(NTADR_C(8, 17));
        screenBuffer[5] = scratch ? HUD_ARROW : HUD_BLANK;
        screenBuffer[6] = NT_UPD_EOF;
        set_vram_update(screenBuffer);

        if (scratch2 & PAD_START) {
            sfx_play(SFX_PAUSE_DOWN, 2);
            break;
        }
        ppu_wait_frame();
    }
    set_vram_update(NULL);
    if (scratch) {
        gameState = GAME_STATE_LEVEL_START;
    } else {
        gameState = GAME_STATE_RUNNING;
    }
}