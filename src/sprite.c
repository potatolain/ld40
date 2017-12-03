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
	SPRITE_TYPE_GEM, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_2 | SPRITE_ANIM_NONE, 0x00, SPRITE_DIRECTION_DOWN,
	// Alien
    SPRITE_TYPE_ENEMY, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_3 | SPRITE_ANIM_FULL, 0x68, 3,
    // Fast freakin' alien
    SPRITE_TYPE_ENEMY, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_2 | SPRITE_ANIM_FULL, 0x68, 5,
    // Evil space bunny
    SPRITE_TYPE_ENEMY, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_2 | SPRITE_ANIM_FULL, 0x60, 7,
    // Slow marble
    SPRITE_TYPE_ENEMY, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_0 | SPRITE_ANIM_DEFAULT, 0xc0, 1,
    // Medium marble
    SPRITE_TYPE_ENEMY, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_3 | SPRITE_ANIM_DEFAULT, 0xc0, 2,
    // Fast marble
    SPRITE_TYPE_ENEMY, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_1 | SPRITE_ANIM_DEFAULT, 0xc0, 6,
    // Anti gem
    SPRITE_TYPE_ANTI_GEM, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_0 | SPRITE_ANIM_NONE, 0x04, 0
    
};

static char sprite_directions[12];
static char sprite_speeds[12];
static char sprite_distances[12];
static char expected_sprite_distances[12];

void banked_draw_sprites() {
	for (i = 0; i < 12; i++) {
        sprite_directions[i] = SPRITE_DIRECTION_UNDEF;
        sprite_speeds[i] = 0;
        sprite_distances[i] = 0;
        expected_sprite_distances[i] = 0;
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
		scratch2 = (scratch & 0xf0)-1; // Y
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

    for (i = 0; i < 12; i++) {
		// Skipping the nice oam functions here because... well, they do more than I want right now.
		if (*(unsigned char*)(0x200 + FIRST_ENEMY_SPRITE_ID+(i<<4)) == NESLIB_SPRITE_GONE)
            continue;
         
        // Enemies move at the same time the player does
        if (!playerVelocityLockTime) {

            sprite_directions[i] = SPRITE_DIRECTION_UNDEF;
            sprite_speeds[i] = 0;
            sprite_distances[i] = 0;
            expected_sprite_distances[i] = 0;
            continue;
        }

        scratchInt = (0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4));
        iShifted = i<<2;

		// If you're an enemy, we can move you around! Imagine that...
		if (extendedSpriteData[iShifted] == SPRITE_TYPE_ENEMY) {

            if (sprite_distances[i] <= expected_sprite_distances[i]) {
                if ((extendedSpriteData[(iShifted)+1] & SPRITE_ANIM_MASK) == SPRITE_ANIM_DEFAULT) {
                    scratch = extendedSpriteData[(iShifted)+2];
                    scratch += (FRAME_COUNTER & 0x08) ? 0 : 2;
                    *(unsigned char*)(scratchInt+1) = scratch;
                    *(unsigned char*)(scratchInt+5) = scratch+1;
                    *(unsigned char*)(scratchInt+9) = scratch+0x10;
                    *(unsigned char*)(scratchInt+13) = scratch+0x11;
                } else if ((extendedSpriteData[(iShifted)+1] & SPRITE_ANIM_MASK) == SPRITE_ANIM_FULL && sprite_directions[i] != SPRITE_DIRECTION_UNDEF) {
                    scratch = extendedSpriteData[(iShifted)+2];
                    scratch += (FRAME_COUNTER & 0x08) ? 0 : 2;
                    if (sprite_directions[i] != SPRITE_DIRECTION_UNDEF)
                        scratch += sprite_directions[i];
                    else
                        scratch += SPRITE_DIRECTION_DOWN;
                    *(unsigned char*)(scratchInt+1) = scratch;
                    *(unsigned char*)(scratchInt+5) = scratch+1;
                    *(unsigned char*)(scratchInt+9) = scratch+0x10;
                    *(unsigned char*)(scratchInt+13) = scratch+0x11;
                }
            }
        

			scratch = scratch4 = *(unsigned char*)(scratchInt+3); // X
			scratch2 = scratch5 = *(unsigned char*)(scratchInt); // Y
            // scratch3 is whether to switch direction... 
            // FIXME: Work with player stuffs.
			if (sprite_directions[i] == SPRITE_DIRECTION_UNDEF) {
                xDelta = (playerX >> 2) - scratch;
                yDelta = (playerY >> 2) - scratch2;

                sprite_distances[i] = 0;

                if (abs(xDelta) > abs(yDelta)) {
                    if (xDelta > 0) {
                        scratch3 = SPRITE_DIRECTION_RIGHT;
                    } else {
                        scratch3 = SPRITE_DIRECTION_LEFT;
                    }
                } else {
                    if (yDelta > 0) {
                        scratch3 = SPRITE_DIRECTION_DOWN;
                    } else {
                        scratch3 = SPRITE_DIRECTION_UP;
                    }
                }
        
				sprite_directions[i] = scratch3;

                // Okay, while the player's moving for playerVelocityLockTime, we need to move extendedSpriteData[x+3] tiles, or ESD[x+3]*16 pixels. Math.
                sprite_speeds[i] = (extendedSpriteData[(iShifted)+3]<<4) / playerVelocityLockTime;
                expected_sprite_distances[i] = extendedSpriteData[(iShifted)+3]<<4;
                if (sprite_speeds[i] == 0)
                    sprite_speeds[i] = 1;
            }
            
            if (playerVelocityLockTime == 1) {
                scratch4 = (scratch4 + 8) & 0xf0;
                scratch5 = (scratch5 + 8) & 0xf0;
            }

			if (sprite_distances[i] <= expected_sprite_distances[i] && sprite_directions[i] != SPRITE_DIRECTION_UNDEF) {
				switch (sprite_directions[i]) {
					case SPRITE_DIRECTION_LEFT:
                        if (playerVelocityLockTime != 1)
                            scratch4 -= sprite_speeds[i];
                        scratchX = scratch4 + SPRITE_X_FUZZ;
                        scratchY = scratch5 + SPRITE_Y_FUZZ;
						if (test_collision(currentLevel[(scratchX>>4)+((((scratchY)>>4))<<4)], 0) || test_collision(currentLevel[(scratchX>>4)+((((scratchY+SPRITE_HEIGHT)>>4))<<4)], 0)) {
							scratch4 = scratch;
						}

						break;
					case SPRITE_DIRECTION_RIGHT:
                        if (playerVelocityLockTime != 1)
                            scratch4 += sprite_speeds[i];
                        scratchX = scratch4 + SPRITE_X_FUZZ;
                        scratchY = scratch5 + SPRITE_Y_FUZZ;
						if (test_collision(currentLevel[((scratchX+SPRITE_WIDTH)>>4)+(((scratchY>>4))<<4)], 0) || test_collision(currentLevel[((scratchX+SPRITE_WIDTH)>>4)+((((scratchY+SPRITE_HEIGHT)>>4))<<4)], 0)) {
							scratch4 = scratch;
						}

						break;
                    case SPRITE_DIRECTION_UP:
                        if (playerVelocityLockTime != 1)
                            scratch5 -= sprite_speeds[i];
                        scratchX = scratch4 + SPRITE_X_FUZZ;
                        scratchY = scratch5 + SPRITE_Y_FUZZ;
						if (test_collision(currentLevel[(scratchX>>4)+((((scratchY)>>4))<<4)], 0) || test_collision(currentLevel[((scratchX+SPRITE_WIDTH)>>4)+(((scratchY>>4))<<4)], 0)) {
							scratch5 = scratch2;
						}
						break;
					case SPRITE_DIRECTION_DOWN:
                        if (playerVelocityLockTime != 1)
                            scratch5 += sprite_speeds[i];
                        scratchX = scratch4 + SPRITE_X_FUZZ;
                        scratchY = scratch5 + SPRITE_Y_FUZZ;

						if (test_collision(currentLevel[((scratchX)>>4)+((((scratchY+SPRITE_HEIGHT)>>4))<<4)], 0) || test_collision(currentLevel[((scratchX+SPRITE_WIDTH)>>4)+((((scratchY+SPRITE_HEIGHT)>>4))<<4)], 0)) {
							scratch5 = scratch2;
						}

						break;
                }
                ++sprite_distances[i];

			}
			

			*(unsigned char*)(scratchInt) = scratch5;
			*(unsigned char*)(scratchInt+4) = scratch5;
			*(unsigned char*)(scratchInt+8) = scratch5+8;
			*(unsigned char*)(scratchInt+12) = scratch5+8;

			*(unsigned char*)(scratchInt+3) = scratch4;
			*(unsigned char*)(scratchInt+7) = scratch4+8;
			*(unsigned char*)(scratchInt+11) = scratch4;
			*(unsigned char*)(scratchInt+15) = scratch4+8;

		}


	}
}