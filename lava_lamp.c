#include "lib/neslib.h"
#include "lib/boilerplate.h"
#include "bin/build_info.h"
#include "src/title.h"
#include "src/level.h"
#include "src/globals.h"
#include "src/hud.h"
#include "src/sprite.h"
#include "src/movement.h"
#include "levels/processed/lvl1_tiles.h"

// Suggestion: Define smart names for your banks and use defines like this. 
// This is just to make a clear example, and I didn't want to suggest using bank #s directly.
#define ROM_0 0
#define ROM_1 1

// Ditto, same advice here.
#define CHR_BANK_0 0
#define CHR_BANK_1 2 // NOTE: We have two copies of the same 4k data in the 8k .chr files (because I'm lazy, ok?) so we use bank 2 to get the inverted one.

const unsigned char levelPalette[16]={ 0x0f,0x00,0x10,0x30,0x0f,0x11,0x21,0x31,0x0f,0x05,0x25,0x35,0x0f,0x09,0x19,0x29 };
const unsigned char spritePalette[16]={ 0x0f,0x00,0x10,0x30,0x0f,0x11,0x21,0x31,0x0f,0x05,0x15,0x25,0x0f,0x09,0x19,0x29 };


// Globals! Defined as externs in src/globals.h
#pragma bssseg (push,"ZEROPAGE")
#pragma dataseg(push,"ZEROPAGE")
unsigned char currentPadState, staticPadState;
unsigned char i, j;
int scratch, scratch2, scratch3, scratch4, scratch5;
unsigned char gameState;
unsigned char currentLevelId;
unsigned char playerOverworldPosition;
unsigned char currentSpriteId;
unsigned int scratchInt;
unsigned char gemCount, antiGemCount;
unsigned char playerDirection, playerAnimState, playerVelocityLockTime, playerInvulnTime;
unsigned int deathCounter;
int playerX, playerY, playerXVelocity, playerYVelocity;
unsigned char gemsInLevel;
int xDelta, yDelta, scratchX, scratchY;
char collisionTemp;
unsigned char iShifted;

// Movement stuff - don't use outside.
unsigned int maxVelocity;
unsigned char lockTime;


#pragma bssseg (pop)
#pragma dataseg(pop)

char currentLevel[256];
char extendedSpriteData[56];

const unsigned char BYTE_TO_BIT[] = {
	0x01, 0x02, 0x04, 0x08,
	0x10, 0x20, 0x40, 0x80
};


// Quick-n-dirty convert integer to string, to show an error code on our error screen.
// Hat tip: http://stackoverflow.com/questions/9655202/how-to-convert-integer-to-string-in-c
char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
	int shifter;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

// Local to this file.
static unsigned char playMusic;
static unsigned char mirrorMode;
static char screenBuffer[48];


// Put a string on the screen at X/Y coordinates given in adr.
void put_str(unsigned int adr, const char *str) {
	vram_adr(adr);
	while(1) {
		if(!*str) break;
		vram_put((*str++)-0x20);//-0x20 because ASCII code 0x20 is placed in tile 0 of the CHR
	}
}

void clear_screen() {
	// Clear the screen to start
	vram_adr(0x2000);
	vram_fill(HUD_BLANK, 0x03c0);
	vram_fill(0, 0x040);
	vram_adr(0x2800);
	vram_fill(HUD_BLANK, 0x03c0);
	vram_fill(0, 0x040);
}

void animate_fadeout(unsigned char _delay) {
	pal_bright(3);
	delay(_delay);
	pal_bright(2);
	delay(_delay);
	pal_bright(1);
	delay(_delay);
	pal_bright(0);
}

void animate_fadein(unsigned char _delay) {
	pal_bright(1);
	delay(_delay);
	pal_bright(2);
	delay(_delay);
	pal_bright(3);
	delay(_delay);
	pal_bright(4);

}

void write_screen_buffer(unsigned char x, unsigned char y, char* data) {
	screenBuffer[0] = MSB(NTADR_A(x, y)) | NT_UPD_HORZ;
	screenBuffer[1] = LSB(NTADR_A(x, y));
	screenBuffer[2] = 16u;
	for (i = 0; data[i] != '\0'; ++i) 
		screenBuffer[i+3u] = data[i]-0x20;
	screenBuffer[19] = NT_UPD_EOF;
	set_vram_update(screenBuffer);
}

void draw_title() {
	set_prg_bank(TITLE_BANK);
	banked_draw_title();
}

void do_title() {
	set_prg_bank(TITLE_BANK);
	banked_do_title();
}

void show_level_complete() {
	set_prg_bank(TITLE_BANK);
	banked_show_level_complete();
}

void do_level_complete() {
	set_prg_bank(TITLE_BANK);
	banked_do_level_complete();
}

void draw_level() {
	set_prg_bank(LEVEL_BANK);
	banked_draw_level();
}

void draw_hud() {
	set_prg_bank(HUD_BANK);
	banked_draw_hud();
}

void draw_win() {
	set_prg_bank(TITLE_BANK);
	banked_draw_win();
}

void do_win() {
	set_prg_bank(TITLE_BANK);
	banked_do_win();
}

void update_hud() {
    screenBuffer[0] = MSB(NTADR_A(26, 26));
    screenBuffer[1] = LSB(NTADR_A(26, 26));
    screenBuffer[2] = HUD_NUMBERS + gemCount;
    screenBuffer[3] = MSB(NTADR_A(9, 26));
    screenBuffer[4] = LSB(NTADR_A(9, 26));
	screenBuffer[5] = HUD_NUMBERS + ((DEFAULT_SPEED - gemCount - antiGemCount) > 0 ? DEFAULT_SPEED - gemCount - antiGemCount : 0);
	// HACK: Find and replace the door
	if (gemCount == gemsInLevel) {
		scratchInt = NAMETABLE_A;
		for (i = 0; i != MAP_TILE_SIZE; ++i) {
			if ((currentLevel[i] & 0x3f) == TILE_DOOR) {
				scratchInt = NAMETABLE_A + ((i % 16) << 1) + ((i / 16) << 6);
				currentLevel[i] = TILE_DOOR_OPEN;
				break;
			}
		}
		if (scratchInt != NAMETABLE_A) {
			screenBuffer[6] = MSB(scratchInt);
			screenBuffer[7] = LSB(scratchInt);
			screenBuffer[8] = TILE_DOOR_OPEN_ABS;
			scratchInt += 1;
			screenBuffer[9] = MSB(scratchInt);
			screenBuffer[10] = LSB(scratchInt);
			screenBuffer[11] = TILE_DOOR_OPEN_ABS + 1;
			scratchInt += 31;		
			screenBuffer[12] = MSB(scratchInt);
			screenBuffer[13] = LSB(scratchInt);
			screenBuffer[14] = TILE_DOOR_OPEN_ABS + 16;
			scratchInt += 1;
			screenBuffer[15] = MSB(scratchInt);
			screenBuffer[16] = LSB(scratchInt);
			screenBuffer[17] = TILE_DOOR_OPEN_ABS + 17;
		}

		screenBuffer[18] = NT_UPD_EOF;
	} else {
		screenBuffer[6] = NT_UPD_EOF;
	}
    set_vram_update(screenBuffer);
}

void do_pause() {
	set_prg_bank(HUD_BANK);
	banked_do_pause();
}

void draw_pause() {
	set_prg_bank(HUD_BANK);
	banked_draw_pause();
}

void draw_dead() {
	set_prg_bank(TITLE_BANK);
	banked_draw_dead();
}
void do_dead() {
	set_prg_bank(TITLE_BANK);
	banked_do_dead();
}

void draw_instructions() {
	set_prg_bank(TITLE_BANK);
	banked_draw_instructions();
}

void do_instructions() {
	set_prg_bank(TITLE_BANK);
	banked_do_instructions();
}

void draw_sprites() {
	set_prg_bank(SPRITE_BANK);
	banked_draw_sprites();
}

void update_sprites() {
	set_prg_bank(SPRITE_BANK);
	banked_update_sprites();
}

void do_movement() {
	set_prg_bank(MOVEMENT_BANK);
	banked_do_movement();
}

void do_sprite_collision() {
	set_prg_bank(MOVEMENT_BANK);
	banked_do_sprite_collision();
}

unsigned char test_collision(unsigned char tileId, unsigned char isPlayer) {
	collisionTemp = tileId & 0x3f;
	if (collisionTemp > 7 && collisionTemp < 16) {
		return 1;
	}
	if (collisionTemp > 19 && collisionTemp < 23) {
		return 1;
	}
	if (isPlayer && collisionTemp == TILE_DOOR_OPEN) {
		// HACK- make sure we're aligned to the grid before we ok it
		if (
			((playerDirection == PLAYER_DIRECTION_LEFT || playerDirection == PLAYER_DIRECTION_RIGHT ) && (playerX >> 2) % 16 > 6 && (playerX >> 2) % 16 < 10) ||
			((playerDirection == PLAYER_DIRECTION_UP || playerDirection == PLAYER_DIRECTION_DOWN ) && (playerY >> 2) % 16 > 6 && (playerY >> 2) % 16 < 10)
			
		) { 
			gameState = GAME_STATE_LEVEL_COMPLETE;
		}
	}
	return 0;
}

// Main entry point for the application.
void main(void) {

	playMusic = 0;
	mirrorMode = MIRROR_HORIZONTAL;
	scroll(0, 0);
	

	ppu_on_all();
	gameState = GAME_STATE_INIT;


	// Now we wait for input from the user, and do dumb things!
	while(1) {
		switch (gameState) {
			case GAME_STATE_INIT:

				gameState = GAME_STATE_TITLE;
				draw_title();
				break;
			case GAME_STATE_TITLE:
				do_title();

				break;
			case GAME_STATE_POST_START:
				draw_instructions();
				do_instructions();
				currentLevelId = 0;
				deathCounter = 0;
				playerOverworldPosition = FIRST_LEVEL;
				gameState = GAME_STATE_LEVEL_START;
				break;
			case GAME_STATE_LEVEL_START: 
				animate_fadeout(5);
				scroll(0, 0);				
				gemsInLevel = 0;
				set_prg_bank(BANK_FIRST_LEVEL+currentLevelId);

				// NOTE: Yes, this says lvl1 - it'll line up with whatever we get though.
				memcpy(currentLevel, lvl1 + (playerOverworldPosition << 8), 256);

				ppu_off();
				pal_bg(levelPalette);
				pal_spr(spritePalette);
				set_chr_bank_0(CHR_BANK_MAIN);
				set_chr_bank_1(CHR_BANK_MAIN+1);	
				
				// TODO: Move this bit to level instead (make sure to turn off ppu!)
				// NOTE: Order here is important - hud relies on the draw_sprites method and count
				gemCount = 0;
				antiGemCount = 0;		
				playerXVelocity = 0;
				playerYVelocity = 0;
				playerVelocityLockTime = 0;		
				draw_level();
				draw_sprites();
				draw_hud();				
				// TODO: Nice fade anim into level(s)
				ppu_on_all();
				animate_fadein(5);
				gameState = GAME_STATE_RUNNING;
				music_play(SONG_DUMB);
				music_pause(0);
				break;
			
			case GAME_STATE_RUNNING:
				staticPadState = pad_trigger(0);		
				currentPadState = pad_state(0);
				
				// Cheap trick - make the game move at double speed
				do_movement();
				update_sprites();
				do_movement();
				update_sprites();

				bank_spr(!((FRAME_COUNTER >> 3) & 0x1f));
				bank_bg(!((FRAME_COUNTER >> 3) & 0x1f));

				break;
			case GAME_STATE_LEVEL_LOST:
				animate_fadeout(5);
				draw_dead();
				deathCounter++;
				animate_fadein(5);
				do_dead();
				gameState = GAME_STATE_LEVEL_START;
				break;
			case GAME_STATE_LEVEL_COMPLETE:
				music_play(SONG_COMPLETE);			
				animate_fadeout(5);
				show_level_complete();
				animate_fadein(5);
				do_level_complete();
				gameState = GAME_STATE_LEVEL_START;
				playerOverworldPosition++;
				if (playerOverworldPosition == LAST_LEVEL) {
					gameState = GAME_STATE_WON;
				}
				break;
			case GAME_STATE_PAUSE:
				animate_fadeout(5);
				music_pause(1);
				ppu_off();
				draw_pause();
				scroll(0, 240);
				ppu_on_bg();
				animate_fadein(5);
				do_pause();
				// If you did reset, the level stuff will take care of this for you.
				if (gameState == GAME_STATE_RUNNING) {
					animate_fadeout(5);
					scroll(0, 0);
					ppu_on_all();
					animate_fadein(5);
					music_pause(0);
				}
				break;
			case GAME_STATE_WON:
				animate_fadeout(5);
				ppu_off();
				draw_win();
				ppu_on_all();
				animate_fadein(5);
				do_win();
				animate_fadeout(5);
				break;
			default:
				ppu_off();
				put_str(NTADR_A(2,20), "Unknown state encountered");
				ppu_on_all();
		}
		ppu_wait_frame();
	}
}
