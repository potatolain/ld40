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