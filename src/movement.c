#include "src/globals.h"
#include "lib/neslib.h"
#include "src/sprite.h"
#include "src/movement.h"
#pragma rodataseg ("ROM_00")
#pragma codeseg ("ROM_00")

unsigned int get_bunny_speed() {
    switch (gemCount) {
        case 6:
            return 0;
        case 5: 
            return 1;
        case 4: 
            return 2;
        case 3: 
            return 4;
        case 2: 
            return 4;
        case 1: 
            return 4;
        case 0: 
            return 4;
    }
}
unsigned char get_lock_time() {
    switch (gemCount) {
        case 6:
            return 0;
        case 5: 
            return 48;
        case 4:
            return 48;
        case 3:
            return 48;
        case 2:
            return 64;
        case 1:
            return 80;
        case 0:
            return 96;
    }
}

void banked_do_movement() {
    unsigned int maxVelocity = get_bunny_speed();
    unsigned char lockTime = get_lock_time();

    #if DEBUG
		if (currentPadState & PAD_START && currentPadState & PAD_SELECT) {
			gameState = GAME_STATE_LEVEL_COMPLETE;
			return;
		}
	#endif


	if (staticPadState & PAD_START) {
		gameState = GAME_STATE_PAUSE;
		sfx_play(SFX_PAUSE_UP, 2);
		return; // Don't do anything else.
    }

    // Try to force alignment to the grid
    if (!playerVelocityLockTime) {
        if (playerDirection == PLAYER_DIRECTION_LEFT || playerDirection == PLAYER_DIRECTION_RIGHT) {
            scratch = (playerX >> 2) % 16;
            if (playerXVelocity != 0 && scratch != 0) {
                playerVelocityLockTime = 1;
            }
        }
        if (playerDirection == PLAYER_DIRECTION_UP || playerDirection == PLAYER_DIRECTION_DOWN) {
            scratch = (playerY >> 2) % 16;
            if (playerYVelocity != 0 && scratch != 0) {
                playerVelocityLockTime = 1;
            }
        }
    }
    
    if (!playerVelocityLockTime) {
        playerVelocityLockTime = lockTime;
        playerXVelocity = 0;
        playerYVelocity = 0;
        if (currentPadState & PAD_RIGHT) {
            playerDirection = PLAYER_DIRECTION_RIGHT;
        } else if (currentPadState & PAD_LEFT) {
            playerDirection = PLAYER_DIRECTION_LEFT;
        } else if (currentPadState & PAD_UP) {
            playerDirection = PLAYER_DIRECTION_UP;
        } else if (currentPadState & PAD_DOWN) {
            playerDirection = PLAYER_DIRECTION_DOWN;
        } else {
            // lol j/k
            playerVelocityLockTime = 0;
        }
    }

	if (playerVelocityLockTime) {
		if (playerDirection == PLAYER_DIRECTION_RIGHT) {
			if (playerXVelocity < maxVelocity) {
				playerXVelocity += PLAYER_VELOCITY_ACCEL;
			} else if (playerXVelocity > maxVelocity) {
				playerXVelocity -= PLAYER_VELOCITY_ACCEL;
			}
		} else if (playerDirection == PLAYER_DIRECTION_LEFT) {
			if (abs(playerXVelocity) < maxVelocity) {
				playerXVelocity -= PLAYER_VELOCITY_ACCEL;
			} else if (abs(playerXVelocity) > maxVelocity) {
				playerXVelocity += PLAYER_VELOCITY_ACCEL;
			}
		} else if (playerXVelocity != 0) {
			if (playerXVelocity > 0) {
				playerXVelocity -= PLAYER_VELOCITY_ACCEL*2;
			} else {
				playerXVelocity += PLAYER_VELOCITY_ACCEL*2;
			}
		}

		if (playerDirection == PLAYER_DIRECTION_UP) {
			if (abs(playerYVelocity) < maxVelocity) {
				playerYVelocity -= PLAYER_VELOCITY_ACCEL;
			} else if (abs(playerYVelocity) > maxVelocity) {
				playerYVelocity += PLAYER_VELOCITY_ACCEL;
			}
		} else if (playerDirection == PLAYER_DIRECTION_DOWN) {
			if (playerYVelocity < maxVelocity) {
				playerYVelocity += PLAYER_VELOCITY_ACCEL;
			} else if (playerYVelocity > maxVelocity) {
				playerYVelocity -= PLAYER_VELOCITY_ACCEL;
			}
		} else if (playerYVelocity != 0) {
			if (playerYVelocity > 0) {
				playerYVelocity -= PLAYER_VELOCITY_ACCEL*2;
			} else {
				playerYVelocity += PLAYER_VELOCITY_ACCEL*2;
			}
			if (abs(playerYVelocity) == 1) {
				playerYVelocity = 0;
			}
		}
		
        playerAnimState++;
        --playerVelocityLockTime;
    } else {
        playerAnimState = 0;
    }
    
	// Okay, collision test time. Hold onto yer butts!
	if (playerYVelocity != 0) {
		scratch = (playerY + playerYVelocity)>>2;
		scratch2 = (playerX>>2) + 4;
		if (playerYVelocity & 0x80) {
			if (test_collision(currentLevel[(scratch2>>4)+((((scratch)>>4))<<4)], 1)) {
                playerYVelocity = 0;
                playerVelocityLockTime = 0;
            }
            playerDirection = PLAYER_DIRECTION_UP;
		} else {
			if (test_collision(currentLevel[((scratch2)>>4)+((((scratch+PLAYER_HEIGHT)>>4))<<4)], 1)) {
                playerYVelocity = 0;
                playerVelocityLockTime = 0;
			}
            playerDirection = PLAYER_DIRECTION_DOWN;
		}
    }

	if (playerXVelocity != 0) {
		scratch = (playerX + playerXVelocity)>>2;
		scratch2 = (playerY>>2) + 4;
		if (playerXVelocity & 0x80) {
			if (test_collision(currentLevel[(scratch>>4)+((((scratch2)>>4))<<4)], 1)) {
                playerXVelocity = 0;
                playerVelocityLockTime = 0;
			}
            playerDirection = PLAYER_DIRECTION_LEFT;
		} else {
			if (test_collision(currentLevel[((scratch+PLAYER_WIDTH)>>4)+(((scratch2>>4))<<4)], 1)) {
                playerXVelocity = 0;
                playerVelocityLockTime = 0;
			}
            playerDirection = PLAYER_DIRECTION_RIGHT;
		}
	}

	do_sprite_collision();

	playerX += playerXVelocity;
	playerY += playerYVelocity;
	scratch = playerX>>2;
	scratch2 = playerY>>2;

	currentSpriteId = PLAYER_SPRITE_ID;
	scratch3 = PLAYER_SPRITE_TILE + ((playerAnimState & 0x04) >> 1) + playerDirection;

    currentSpriteId = oam_spr(scratch, scratch2, scratch3, 0, PLAYER_SPRITE_ID);
	currentSpriteId = oam_spr(scratch+8, scratch2, scratch3+1, 0, currentSpriteId);
	currentSpriteId = oam_spr(scratch, scratch2+8, scratch3+0x10, 0, currentSpriteId);
	currentSpriteId = oam_spr(scratch+8, scratch2+8, scratch3+0x11, 0, currentSpriteId);

}


void banked_do_sprite_collision() {
	scratch = playerX>>2;
	scratch2 = playerY>>2;
	for (i = 0; i < 12; ++i) {
        // scratch3 = (extendedSpriteData[(i<<2)+1] & SPRITE_SIZE_MASK) == SPRITE_SIZE_NORMAL ? 16 : 8; // TODO: this logic is probably simpler than needed
        scratch3 = 16;
		// Yes, I'm directly reading values from OAM without so much as a #define. Shut up.
		scratch4 = *(char*)(0x200 + FIRST_ENEMY_SPRITE_ID+3 + (i<<4));
		scratch5 = *(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4));
		// Make enemies a touch smaller so they're less likely to be hit by accident.
		if (extendedSpriteData[(i<<2)] == SPRITE_TYPE_ENEMY) {
			scratch4 += 5;
			scratch5 += 5;
			scratch3 = 6;
		}

		if (scratch < scratch4 + scratch3 && scratch + PLAYER_WIDTH > scratch4 && 
			scratch2 < scratch5 + scratch3 && scratch2 + PLAYER_WIDTH > scratch5) {
			// When we collide... 

			switch (extendedSpriteData[(i<<2)]) {
                case SPRITE_TYPE_ENEMY: 
                
					// Restart level. You lose!
                    music_play(SONG_DEAD);
                    gameState = GAME_STATE_LEVEL_LOST;
					break;

				case SPRITE_TYPE_GEM:
		
					// Mario toad voice "Baaaaii"
					(*(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4))) = 0xff;
					(*(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4)+4)) = 0xff;
					(*(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4)+8)) = 0xff;
					(*(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4)+12)) = 0xff;

                    gemCount++;

					sfx_play(SFX_GEM, 1);
					update_hud();



					break;

				default: 
					// Eh, do nothing. It shall live on.
					break;
			}
		}

	}

}