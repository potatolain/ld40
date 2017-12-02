// This file defines globals that can be used all over. You'll want common things in here, as the NES has very, very
// limited ram. 
extern unsigned char currentPadState;
extern unsigned char i, j, scratch;
extern char currentMessage[16];
extern unsigned char gameState;

void put_str(unsigned int adr, const char *str);

#define SHOW_VERSION_INFO 1 

#define GAME_STATE_RUNNING 0
#define GAME_STATE_POST_START 1
#define GAME_STATE_PAUSED 100
#define GAME_STATE_TITLE 201
#define GAME_STATE_INIT 200
