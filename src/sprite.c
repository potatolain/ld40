#include "src/globals.h"
#include "lib/neslib.h"
#include "src/sprite.h"

#define PLAYER_START_ID 0

#pragma rodataseg ("ROM_01")
#pragma codeseg ("ROM_01")

const unsigned char sprite_data[] = {
	// Player Start
	SPRITE_TYPE_IGNORED, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_1 | SPRITE_ANIM_NONE, 0xee, 0,
    // Gem
    // TODO: animate? (Or maybe we just chr swap)
	SPRITE_TYPE_GEM, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_2 | SPRITE_ANIM_NONE, 0x00, SPRITE_DIRECTION_DOWN,
	// Alien
    SPRITE_TYPE_ENEMY, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_3 | SPRITE_ANIM_FULL, 0x68, 3,
    // Fast freakin' alien
    SPRITE_TYPE_ENEMY, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_2 | SPRITE_ANIM_FULL, 0x68, 6,
};

static char sprite_directions[12];
static char sprite_direction_time[12];


void banked_draw_sprites() {
	for (i = 0; i < 12; i++) {
        sprite_directions[i] = SPRITE_DIRECTION_UNDEF;
		sprite_direction_time[i] = 0;
    }

	for (i = 0; i < 12 && currentLevel[MAP_TILE_SIZE + (i<<1)] != 255; ++i) {
		scratch = currentLevel[MAP_TILE_SIZE + (i<<1)];
		scratch5 = currentLevel[MAP_TILE_SIZE + (i<<1)+1];

		extendedSpriteData[(i<<2)  ] = sprite_data[(scratch5<<2)];
		extendedSpriteData[(i<<2)+1] = sprite_data[(scratch5<<2)+1];
		extendedSpriteData[(i<<2)+2] = sprite_data[(scratch5<<2)+2];
		extendedSpriteData[(i<<2)+3] = sprite_data[(scratch5<<2)+3];

        // Scratch 1 is the position. Need to do some hax to get it to 16x16 coords
        // Why -1? Seems to align things to tiles, never figured that out in detail. Game jams make me do bad things >_>
		scratch2 = ((scratch >> 4) << 4)-1; // Y
		scratch = ((scratch % 16) << 4); // X
		scratch3 = extendedSpriteData[(i<<2)+2];
		// I'm so, so sorry.
        scratch4 = sprite_data[((currentLevel[MAP_TILE_SIZE + (i<<1)+1])<<2)+1] & SPRITE_PALETTE_MASK;
        
        if (extendedSpriteData[i<<2] == SPRITE_TYPE_GEM) {
            ++gemsInLevel;
        }

        if (scratch5 == PLAYER_START_ID) { 
            playerX = scratch << 2;
            playerY = scratch2 << 2;
            // Just hide me, then set the params for the player's starting position.
            oam_spr(NESLIB_SPRITE_GONE, NESLIB_SPRITE_GONE, scratch3, scratch4, FIRST_ENEMY_SPRITE_ID + (i << 4));
            oam_spr(NESLIB_SPRITE_GONE, NESLIB_SPRITE_GONE, scratch3+1, scratch4, (FIRST_ENEMY_SPRITE_ID + 4) + (i << 4));
            oam_spr(NESLIB_SPRITE_GONE, NESLIB_SPRITE_GONE, scratch3+0x10, scratch4, (FIRST_ENEMY_SPRITE_ID + 8) + (i << 4));
            currentSpriteId = oam_spr(NESLIB_SPRITE_GONE, NESLIB_SPRITE_GONE, scratch3+0x11, scratch4, (FIRST_ENEMY_SPRITE_ID + 12) + (i << 4));    
        } else {
            oam_spr(scratch, scratch2, scratch3, scratch4, FIRST_ENEMY_SPRITE_ID + (i << 4));
            oam_spr(scratch+8, scratch2, scratch3+1, scratch4, (FIRST_ENEMY_SPRITE_ID + 4) + (i << 4));
            oam_spr(scratch, scratch2+8, scratch3+0x10, scratch4, (FIRST_ENEMY_SPRITE_ID + 8) + (i << 4));
            currentSpriteId = oam_spr(scratch+8, scratch2+8, scratch3+0x11, scratch4, (FIRST_ENEMY_SPRITE_ID + 12) + (i << 4));    
        }

	}
	oam_hide_rest(currentSpriteId);

}

void banked_update_sprites() {
    
}