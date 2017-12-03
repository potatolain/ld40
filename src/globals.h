// This file defines globals that can be used all over. You'll want common things in here, as the NES has very, very
// limited ram. 
extern unsigned char currentPadState, staticPadState;
extern unsigned char i, j;
extern int scratch, scratch2, scratch3, scratch4, scratch5;
extern unsigned char gameState;
extern unsigned char currentLevelId;
extern unsigned char playerOverworldPosition;
extern unsigned int scratchInt;
extern unsigned char gemCount;
extern unsigned char currentSpriteId;
extern unsigned char playerDirection, playerAnimState, playerVelocityLockTime, playerInvulnTime;
extern int playerX, playerY, playerXVelocity, playerYVelocity;
extern unsigned char gemsInLevel;
extern int xDelta, yDelta, scratchX, scratchY;
extern unsigned int deathCounter;

extern char currentLevel[256];
extern char screenBuffer[48];
extern char extendedSpriteData[56];

extern unsigned char FRAME_COUNTER;

extern const unsigned char BYTE_TO_BIT[8];


// Get at the stuff in level data from whatever bank is loaded. (Loads const data from assembly)
extern const char lvl_details[4];

void put_str(unsigned int adr, const char *str);
unsigned char test_collision(unsigned char tileId, unsigned char isPlayer);
void do_sprite_collision();
void update_hud();
void clear_screen();
void animate_fadeout(unsigned char _delay);
void animate_fadein(unsigned char _delay);
char* itoa(int i, char b[]);

// Crappy macro to get absolute value in an absolutely disgusting way
#define abs(x) (x > 0 ? x : 0-x)

#define SHOW_VERSION_INFO 1 
#define DEBUG 1
#define FIRST_LEVEL 0
#define LAST_LEVEL 3

#define BANK_FIRST_LEVEL 2

#define CHR_BANK_TITLE 0
#define CHR_BANK_MAIN 2

#define GAME_STATE_RUNNING 0
#define GAME_STATE_POST_START 1
#define GAME_STATE_LEVEL_START 10
#define GAME_STATE_LEVEL_LOST 40
#define GAME_STATE_LEVEL_COMPLETE 50
#define GAME_STATE_PAUSE 100
#define GAME_STATE_TITLE 201
#define GAME_STATE_INIT 200
#define GAME_STATE_WON 250

#define PLAYER_SPRITE_TILE 0x60

#define DEFAULT_SPEED 6

#define HUD_BLANK 0xff
#define HUD_NUMBERS 0xf3
#define HUD_H 0xf2
#define HUD_V 0xe2
#define HUD_TL 0xe0
#define HUD_TR 0xe1
#define HUD_BL 0xf0
#define HUD_BR 0xf1
#define HUD_GEMS 0xe3
#define HUD_COLON 0xea
#define HUD_SLASH 0xeb
#define HUD_E 0xe4
#define HUD_SP 0xe7
#define HUD_D 0xe9
#define HUD_A 0xec
#define HUD_U 0xed
#define HUD_P 0xfd
#define HUD_ARROW 0xa8
#define HUD_R 0xa9
#define HUD_T 0xaa
#define HUD_C 0xab
#define HUD_O 0xac
#define HUD_N 0xad
#define HUD_I 0xae

#define MAP_TILE_SIZE 192

#define PLAYER_SPRITE_ID 0x10
#define FIRST_ENEMY_SPRITE_ID 0x20

#define PLAYER_DIRECTION_LEFT 0x24
#define PLAYER_DIRECTION_RIGHT 0x20
#define PLAYER_DIRECTION_UP 4
#define PLAYER_DIRECTION_DOWN 0
#define PLAYER_SPRITE_EMPTY 0xee

#define PLAYER_VELOCITY 4
#define PLAYER_MAX_VELOCITY 8
#define PLAYER_VELOCITY_ACCEL 1

#define PLAYER_WIDTH 16
#define PLAYER_HEIGHT 16

#define TILE_DOOR 6
#define TILE_DOOR_ABS 0x0c
#define TILE_DOOR_OPEN 7
#define TILE_DOOR_OPEN_ABS 0x0e

#define SFX_PAUSE_UP 2
#define SFX_PAUSE_DOWN 1
#define SFX_GEM 3
#define SFX_PAUSE_SWAP 4
#define SFX_DEAD 5

#define SONG_DUMB 0
#define SONG_DEAD 1
#define SONG_COMPLETE 2

#pragma zpsym ("currentPadState")
#pragma zpsym ("staticPadState")
#pragma zpsym ("i")
#pragma zpsym ("j")
#pragma zpsym ("scratchInt")
#pragma zpsym ("scratch")
#pragma zpsym ("scratch2")
#pragma zpsym ("scratch3")
#pragma zpsym ("scratch4")
#pragma zpsym ("scratch5")
#pragma zpsym ("gameState")
#pragma zpsym ("currentLevelId")
#pragma zpsym ("playerOverworldPosition")
#pragma zpsym ("gemCount")
#pragma zpsym ("currentSpriteId")
#pragma zpsym ("playerX")
#pragma zpsym ("playerY")
#pragma zpsym ("playerDirection")
#pragma zpsym ("playerAnimState")
#pragma zpsym ("playerXVelocity")
#pragma zpsym ("playerYVelocity")
#pragma zpsym ("playerVelocityLockTime")
#pragma zpsym ("gemsInLevel")
#pragma zpsym ("FRAME_COUNTER")
#pragma zpsym ("scratchX")
#pragma zpsym ("scratchY")
#pragma zpsym ("xDelta")
#pragma zpsym ("yDelta")
#pragma zpsym ("deathCounter")