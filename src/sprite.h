#define SPRITE_BANK 1

#define SPRITE_PALETTE_0 0
#define SPRITE_PALETTE_1 1
#define SPRITE_PALETTE_2 2
#define SPRITE_PALETTE_3 3

#define SPRITE_DIRECTION_LEFT 0x24
#define SPRITE_DIRECTION_RIGHT 0x20
#define SPRITE_DIRECTION_UP 4
#define SPRITE_DIRECTION_DOWN 0
#define SPRITE_DIRECTION_UNDEF 0xff

#define SPRITE_SIZE_NORMAL 0

#define SPRITE_TYPE_IGNORED 0x11
#define SPRITE_TYPE_ENEMY 0x12
#define SPRITE_TYPE_GEM 0x13

#define SPRITE_PALETTE_MASK 0x03
#define SPRITE_ANIM_MASK 0x30

#define SPRITE_ANIM_NONE 0
#define SPRITE_ANIM_DEFAULT 0x10
#define SPRITE_ANIM_FULL 0x20

#define NESLIB_SPRITE_GONE 0xf0

// TODO: Do we wanna be smarter about sprite size?
#define SPRITE_WIDTH 16
#define SPRITE_HEIGHT 16


void banked_draw_sprites();
void banked_update_sprites();