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

    vram_adr(NTADR_A(2, 26));
    vram_put(HUD_GEMS);
    vram_put(HUD_GEMS+1);
    vram_put(HUD_GEMS+2);
    vram_put(HUD_GEMS+3);
    vram_put(HUD_COLON);
    vram_put(HUD_NUMBERS);
    vram_put(HUD_NUMBERS + gemCount);

    vram_adr(NTADR_A(22, 26));
    vram_put(HUD_SP);
    vram_put(HUD_SP+1);
    vram_put(HUD_E);
    vram_put(HUD_E);
    vram_put(HUD_D);
    vram_put(HUD_COLON);
    vram_put(HUD_NUMBERS);
    vram_put(HUD_NUMBERS + ((DEFAULT_SPEED - gemCount) > 0 ? DEFAULT_SPEED - gemCount : 0));

}

void banked_update_hud() {
    screenBuffer[0] = MSB(NTADR_A(9, 26));
    screenBuffer[1] = LSB(NTADR_A(9, 26));
    screenBuffer[2] = 1u;
    screenBuffer[3] = HUD_NUMBERS + gemCount;
    screenBuffer[4] = MSB(NTADR_A(8, 26));
    screenBuffer[5] = LSB(NTADR_A(29, 26));
    screenBuffer[6] = 1u;
    screenBuffer[7] = ((DEFAULT_SPEED - gemCount) > 0 ? DEFAULT_SPEED - gemCount : 0);
    screenBuffer[8] = NT_UPD_EOF;
    set_vram_update(screenBuffer);
}